#pragma once
// CompileTimeValue STUB (CTV OFF) - zero-overhead, API-compatible wrapper
#include <cstdint>
#include <cstring>
#include <type_traits>

#define PACK_FORCEINLINE __forceinline

namespace compile_time_value
{
    PACK_FORCEINLINE constexpr std::uint64_t protected_value_hash(const char* str) noexcept
    {
        std::uint64_t value = 1229782938247303823ull;
        for (;;) { const char c = *str++; if (!c) return value; value = (value ^ c) * 419441075047ull; }
    }
    PACK_FORCEINLINE constexpr std::uint64_t protected_value_hash_static(const char* str) noexcept
    {
        std::uint64_t value = 122909923ull;
        for (;;) { const char c = *str++; if (!c) return value; value = (value ^ c) * 2311ull; }
    }
    template<std::size_t Size>
    PACK_FORCEINLINE constexpr std::size_t _buffer_size() { return ((Size / 8) + (Size % 8 != 0)); }
}

#define PROT_VAL_HASH(str) compile_time_value::protected_value_hash(str)
#define PROT_VAL_HASH_STATIC(str) compile_time_value::protected_value_hash_static(str)

template<typename T, std::uint64_t GlobalSeed>
class compiletime_protected_value
{
public:
    PACK_FORCEINLINE void set_value(const T& value) noexcept { _val = value; }
    PACK_FORCEINLINE T get_value() const noexcept { return _val; }

    PACK_FORCEINLINE operator T() const noexcept { return _val; }
    PACK_FORCEINLINE compiletime_protected_value() = default;
    PACK_FORCEINLINE compiletime_protected_value(const T& value) : _val(value) {}

    PACK_FORCEINLINE compiletime_protected_value& operator=(const T& value) noexcept { _val = value; return *this; }
    PACK_FORCEINLINE compiletime_protected_value& operator=(const compiletime_protected_value& o) noexcept { _val = o._val; return *this; }

    PACK_FORCEINLINE compiletime_protected_value& operator+=(const T& v) noexcept { _val += v; return *this; }
    PACK_FORCEINLINE compiletime_protected_value& operator-=(const T& v) noexcept { _val -= v; return *this; }
    PACK_FORCEINLINE compiletime_protected_value& operator*=(const T& v) noexcept { _val *= v; return *this; }
    PACK_FORCEINLINE compiletime_protected_value& operator/=(const T& v) noexcept { _val /= v; return *this; }

    PACK_FORCEINLINE compiletime_protected_value& operator++() noexcept { ++_val; return *this; }
    PACK_FORCEINLINE compiletime_protected_value operator++(int) noexcept { auto t = *this; ++_val; return t; }
    PACK_FORCEINLINE compiletime_protected_value& operator--() noexcept { --_val; return *this; }
    PACK_FORCEINLINE compiletime_protected_value operator--(int) noexcept { auto t = *this; --_val; return t; }

    template<typename U = T>
    PACK_FORCEINLINE typename std::enable_if<std::is_integral<U>::value, compiletime_protected_value&>::type
        operator|=(const T& v) noexcept { _val |= v; return *this; }
    template<typename U = T>
    PACK_FORCEINLINE typename std::enable_if<std::is_integral<U>::value, compiletime_protected_value&>::type
        operator&=(const T& v) noexcept { _val &= v; return *this; }
    template<typename U = T>
    PACK_FORCEINLINE typename std::enable_if<std::is_integral<U>::value, compiletime_protected_value&>::type
        operator^=(const T& v) noexcept { _val ^= v; return *this; }

private:
    T _val{};
};

#define PROTECTED_VALUE(Type, Name, StringSeed) compiletime_protected_value<Type, PROT_VAL_HASH(StringSeed)> Name
#define PROTECTED_VALUE_STATIC(Type, Name, StringSeed) compiletime_protected_value<Type, PROT_VAL_HASH_STATIC(StringSeed)> Name
