#pragma once

#include <cstdint>

class DynamicPacket {
private:
    const char* data_;
    uint32_t size_;
    uint32_t extraSize_;

public:
    DynamicPacket(const char* data)
        : data_(data), size_(UINT32_MAX), extraSize_(0) {}

    DynamicPacket(const char* data, uint32_t size)
        : data_(data), size_(size), extraSize_(0) {}

    ~DynamicPacket() = default;

	template <typename T>
	const T* PeekPtr() {
		if (sizeof(T) > size_)
			return nullptr;

		return reinterpret_cast<const T*>(data_);
	}

	template <typename T>
	inline const T& Peek() {
		return *PeekPtr<T>();
	}

	template <typename T>
	inline const T* ReadPtr(uint32_t size, bool isExtraSize = true) {
		if (size > size_)
			return nullptr;

		auto ret = reinterpret_cast<const T*>(data_);

		data_ += size;
		size_ -= size;

		if (isExtraSize)
			extraSize_ += size;

		return ret;
	}

	template <typename T>
	inline const T& Read(uint32_t size, bool isExtraSize = true) {
		return *ReadPtr<T>(size, isExtraSize);
	}

    template <typename T>
    const T* GetPtr(bool isExtraSize = true) {
		return ReadPtr<T>(sizeof(T), isExtraSize);
    }

	template <typename T>
	inline const T& Get(bool isExtraSize = true) {
		return *GetPtr<T>(isExtraSize);
	}

    size_t GetLeftSize() const { return size_; }

    size_t GetExtraSize() const { return extraSize_; }
};
