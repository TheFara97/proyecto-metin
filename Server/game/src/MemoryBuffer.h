#ifndef MEMORYBUFFER_H
#define MEMORYBUFFER_H
#include <mutex>
#include <functional>

namespace net {
    class MemoryBuffer {
    public:
        // Flags for the buffer behavior
        enum Flags {
            BUFFER_LOCK = 1 << 0,  // Lock the buffer during read/write operations
            BUFFER_GROW = 1 << 1   // Allow automatic growth of the buffer
        };

        // Constructor and destructor
        MemoryBuffer(std::size_t initialSize);

        ~MemoryBuffer();

        // Buffer operations
        bool Write(const void* data, std::size_t len);

        bool Write(const char* data, std::size_t len);

        template <class T>
        bool Write(const T& data) {
            return Write(reinterpret_cast<const char*>(&data), sizeof(T));
        }

        bool ReadByOffset(size_t offset, char* dest, std::size_t len, bool advance);

        bool Read(char* dest, std::size_t len, bool advance = true);
        bool Peek(const size_t offset, const size_t len) const;
        void Reset();

        void Truncate();

        bool Resize(std::size_t newSize);

        // Flag management
        void SetFlags(int flags);

        void ClearFlags(int flags);

        // Getters for buffer properties
        std::size_t GetReadPosition() const;

        std::size_t GetWritePosition() const;

        std::size_t GetSize() const;

        // Static method to set the error handler
        static void SetErrorHandler(std::function<void(const std::string&)> errorHandler);

        // Returns a pointer to the current read position in the buffer
        char* GetReadPointer();

        int64_t GetSpace();

        // Returns a pointer to the current write position in the buffer
        char* GetWritePointer();

        // Advances the read position by the specified number of bytes
        void AdvanceReadPosition(std::size_t numBytes);

        // Advances the write position by the specified number of bytes
        void AdvanceWritePosition(std::size_t numBytes);

        /* Static method to get a temporary buffer
         * This method is not thread-safe and should be used with caution
         * Each call with truncate the positions
         */
        static MemoryBuffer& GetTempBuffer(bool truncate = true, const uint32_t slot = 0);

    private:
        // Buffer memory and state
        char* m_buffer;
        std::size_t m_size;
        std::size_t m_readPos;
        std::size_t m_writePos;
        int m_flags;

        // Thread safety
        mutable std::mutex m_mutex;

        // Static error handler
        static std::function<void(const std::string&)> m_errorHandler;

        // Error reporting helper
        void ReportError(const std::string& errorMessage) const;
    };
}

#endif  // MEMORYBUFFER_H
