#include "stdafx.h"
//#include <shared/buffer/MemoryBuffer.h>

#include "MemoryBuffer.h"
#include <iostream>
#include <functional>
#include <cstring>
#include <cstdlib>  // for malloc, realloc, free
#include <utility>


// Initialize static error handler
std::function<void(const std::string&)> net::MemoryBuffer::m_errorHandler = nullptr;
constexpr std::size_t WORD_SIZE = 8; // Size of capnp::word in bytes

// Constructor: Allocate the initial buffer
net::MemoryBuffer::MemoryBuffer(std::size_t initialSize)
	: m_buffer(nullptr),
	m_size(initialSize),
	m_readPos(0),
	m_writePos(0),
	m_flags(0) {

	try {
		// Ensure the size is a multiple of WORD_SIZE for proper alignment
		std::size_t alignedSize = (m_size + WORD_SIZE - 1) / WORD_SIZE * WORD_SIZE;

#ifdef _WIN32
		// Use _aligned_malloc on Windows for aligned memory allocation
		m_buffer = static_cast<char*>(_aligned_malloc(alignedSize, WORD_SIZE));
		if (!m_buffer) {
			ReportError("Failed to allocate aligned memory on Windows.");
			throw std::bad_alloc();
		}
#else
		// Use posix_memalign on BSD and other POSIX systems for aligned memory allocation
		if (posix_memalign(reinterpret_cast<void**>(&m_buffer), WORD_SIZE, alignedSize) != 0) {
			ReportError("Failed to allocate aligned memory on BSD/POSIX.");
			throw std::bad_alloc();
		}
#endif
	}
	catch (const std::bad_alloc&) {
		ReportError("Failed to allocate memory.");
		throw;
	}

	if (!m_buffer) {
		ReportError("Failed to allocate memory.");
		throw std::bad_alloc();
	}
}

// Destructor: Free the buffer
// Destructor: Free the buffer
net::MemoryBuffer::~MemoryBuffer() {
#ifdef _WIN32
	// Free memory allocated with _aligned_malloc on Windows
	_aligned_free(m_buffer);
#else
	// Free memory allocated with posix_memalign on POSIX systems
	free(m_buffer);
#endif
}

bool net::MemoryBuffer::Write(const void* data, std::size_t len)
{
	return Write(static_cast<const char*>(data), len);
}

// Write data to the buffer, grow if necessary
bool net::MemoryBuffer::Write(const char* data, std::size_t len) {
	std::unique_lock<std::mutex> lock(m_mutex, std::defer_lock);

	if (m_flags & BUFFER_LOCK) {
		lock.lock();
	}

	if (m_writePos + len > m_size) {
		if (m_flags & BUFFER_GROW) {
			if (!Resize(m_writePos + len)) {
				return false;
			}
		}
		else {
			ReportError("Write operation exceeds buffer size.");
			return false;
		}
	}

	if (data)
		std::memcpy(m_buffer + m_writePos, data, len);

	m_writePos += len;
	return true;
}

bool net::MemoryBuffer::Peek(const size_t offset, const size_t len) const
{
	std::unique_lock<std::mutex> lock(m_mutex, std::defer_lock);

	if (m_flags & BUFFER_LOCK) {
		lock.lock();
	}

	if (m_readPos + offset + len > m_size || m_readPos + offset + len > m_writePos)
	{
		return false;
	}

	return true;
}


bool net::MemoryBuffer::ReadByOffset(size_t offset, char* dest, std::size_t len, bool advance)
{
	std::unique_lock<std::mutex> lock(m_mutex, std::defer_lock);

	if (m_flags & BUFFER_LOCK) {
		lock.lock();
	}

	const auto trackOffset = offset + m_readPos + len;

	if (trackOffset > m_size || trackOffset > m_writePos) {
		ReportError("Read operation exceeds available data.");
		return false;
	}

	if (dest)
		std::memcpy(dest, m_buffer + m_readPos + offset, len);

	if (advance)
		m_readPos += len;

	return true;
}

// Read data from the buffer
bool net::MemoryBuffer::Read(char* dest, std::size_t len, bool advance) {
	return ReadByOffset(0, dest, len, advance);
}

// Reset the buffer
void net::MemoryBuffer::Reset() {
	std::unique_lock<std::mutex> lock(m_mutex, std::defer_lock);

	if (m_flags & BUFFER_LOCK) {
		lock.lock();
	}

	m_readPos = 0;
	m_writePos = 0;
	std::memset(m_buffer, 0, m_size);
}

void net::MemoryBuffer::Truncate()
{
	std::unique_lock<std::mutex> lock(m_mutex, std::defer_lock);

	if (m_flags & BUFFER_LOCK) {
		lock.lock();
	}

	m_readPos = 0;
	m_writePos = 0;
}

// Resize the buffer using `new[]`
bool net::MemoryBuffer::Resize(std::size_t newSize) {
	std::unique_lock<std::mutex> lock(m_mutex, std::defer_lock);

	if (m_flags & BUFFER_LOCK) {
		lock.lock();
	}

	if (newSize <= m_size) {
		return true;
	}

	std::size_t growSize = std::max(newSize, m_size + (m_size / 2));

	// Ensure the size is a multiple of WORD_SIZE for proper alignment
	std::size_t alignedSize = (growSize + WORD_SIZE - 1) / WORD_SIZE * WORD_SIZE;

	char* newBuffer = nullptr;

#ifdef _WIN32
	// Use _aligned_malloc on Windows for aligned memory allocation
	newBuffer = static_cast<char*>(_aligned_malloc(alignedSize, WORD_SIZE));
	if (!newBuffer) {
		ReportError("Failed to allocate aligned memory on Windows.");
		return false;
	}
#else
	// Use posix_memalign on BSD and other POSIX systems for aligned memory allocation
	if (posix_memalign(reinterpret_cast<void**>(&newBuffer), WORD_SIZE, alignedSize) != 0) {
		ReportError("Failed to allocate aligned memory on BSD/POSIX.");
		return false;
	}
#endif

	// Copy data from the old buffer to the new buffer
	std::memcpy(newBuffer, m_buffer, m_size);

	// Free the old buffer
#ifdef _WIN32
	_aligned_free(m_buffer);
#else
	free(m_buffer);
#endif

	m_buffer = newBuffer;
	m_size = alignedSize; // Update size to the aligned size
	return true;
}


// Set buffer flags
void net::MemoryBuffer::SetFlags(int flags) {
	std::unique_lock<std::mutex> lock(m_mutex);
	m_flags = flags;
}

// Clear specific buffer flags
void net::MemoryBuffer::ClearFlags(int flags) {
	std::unique_lock<std::mutex> lock(m_mutex);
	m_flags &= ~flags;
}

// Get the current read position
std::size_t net::MemoryBuffer::GetReadPosition() const {
	std::unique_lock<std::mutex> lock(m_mutex);
	return m_readPos;
}

// Get the current write position
std::size_t net::MemoryBuffer::GetWritePosition() const {
	std::unique_lock<std::mutex> lock(m_mutex);
	return m_writePos;
}

// Get the current buffer size
std::size_t net::MemoryBuffer::GetSize() const {
	std::unique_lock<std::mutex> lock(m_mutex);
	return m_size;
}

// Set the static error handler
void net::MemoryBuffer::SetErrorHandler(std::function<void(const std::string&)> errorHandler) {
	m_errorHandler = std::move(errorHandler);
}

char* net::MemoryBuffer::GetReadPointer()
{
	return m_buffer + m_readPos;
}

int64_t net::MemoryBuffer::GetSpace()
{
	return static_cast<int64_t>(m_writePos) - static_cast<int64_t>(m_readPos);
}

char* net::MemoryBuffer::GetWritePointer()
{
	return m_buffer + m_writePos;
}

void net::MemoryBuffer::AdvanceReadPosition(std::size_t numBytes)
{
	// Ensure the read position does not advance beyond the write position
	{
		std::unique_lock<std::mutex> lock(m_mutex, std::defer_lock);

		if (m_flags & BUFFER_LOCK) {
			lock.lock();
		}

		// Ensure the read position does not advance beyond the write position
		m_readPos = std::min<size_t>(m_readPos + numBytes, m_writePos);


		if (m_flags & BUFFER_LOCK) {
			lock.unlock();
		}
	}

	// Set the positions back to zero
	auto readPos = GetReadPosition();
	auto writePos = GetWritePosition();
	if (readPos == writePos && readPos > 0 && writePos > 0)
	{
		Truncate();
	}
}

void net::MemoryBuffer::AdvanceWritePosition(std::size_t numBytes)
{

	if (numBytes > m_size - m_writePos) {
		// Handle the case where the number of bytes exceeds available space
		// Optionally, throw an error or log the situation
		numBytes = m_size - m_writePos;
	}

	// Safely advance the write position
	m_writePos += numBytes;
}

net::MemoryBuffer& net::MemoryBuffer::GetTempBuffer(const bool truncate, const uint32_t slot)
{
	static std::unordered_map<uint32_t, MemoryBuffer*> s_BufferMap;

	if (!s_BufferMap.contains(slot))
	{
		// The temp buffer has a size of Mb
		s_BufferMap[slot] = new MemoryBuffer(8096 * 4);
		s_BufferMap[slot]->SetFlags(BUFFER_GROW);
	}

	//static MemoryBuffer* tempBuffer = nullptr;

	//if (!tempBuffer)
	//{
	//    // The temp buffer has a size of Mb
	//    tempBuffer = new MemoryBuffer(8096 * 4);
	//    tempBuffer->SetFlags(BUFFER_GROW);
	//}

	if (truncate)
	{
		// Truncate the buffer before returning it
		s_BufferMap[slot]->Truncate();
	}

	return *s_BufferMap[slot];
}

// Report error using the static error handler or std::cerr if no handler is set
void net::MemoryBuffer::ReportError(const std::string& errorMessage) const {
	if (m_errorHandler) {
		m_errorHandler(errorMessage);
	}
	else {
		std::cerr << errorMessage << std::endl;
	}
}