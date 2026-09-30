#pragma once
#include "CompileTimeValue.hpp"
#include <array>
#include <cstdint>
#include <immintrin.h>
#include <atomic>
#include <cstring>
#include <intrin.h>

#ifdef _MSC_VER
#define RX_FORCEINLINE __forceinline
#else
#define RX_FORCEINLINE __attribute__((always_inline)) inline
#endif

namespace rx {
    namespace security {
        inline std::atomic<std::uint64_t> g_session_key{ 0 };

        RX_FORCEINLINE void init_session() {
            volatile std::uint64_t stack_var;
            std::uint64_t seed = __rdtsc();
            seed ^= reinterpret_cast<std::uint64_t>(&stack_var);
            seed ^= (seed << 13) ^ (seed >> 7);
            g_session_key.store(seed, std::memory_order_relaxed);
        }

        RX_FORCEINLINE std::uint64_t get_session_key() {
            std::uint64_t key = g_session_key.load(std::memory_order_relaxed);
            if (key == 0) {
                init_session();
                key = g_session_key.load(std::memory_order_relaxed);
            }
            return key;
        }
    }

    namespace detail {
        static constexpr std::uint64_t KEY_SEED = 0x4A1B2C3D4E5F6071ULL;
        static constexpr std::uint64_t OBFUSCATOR_A = 0xB9A8C7D6E5F40312ULL;

        RX_FORCEINLINE std::uint64_t splitmix64(std::uint64_t x) {
            x ^= x >> 30;
            x *= 0xbf58476d1ce4e5b9ULL;
            x ^= x >> 27;
            x *= 0x94d049bb133111ebULL;
            x ^= x >> 31;
            return x;
        }

        RX_FORCEINLINE std::uint64_t generate_salt() {
            volatile std::uint64_t stack_var;
            return splitmix64(__rdtsc() ^ reinterpret_cast<std::uint64_t>(&stack_var) ^ security::get_session_key());
        }

        RX_FORCEINLINE std::uint64_t rotate_key(std::uint64_t key, std::uint8_t shift) {
            return (key << shift) | (key >> (64 - shift));
        }

        RX_FORCEINLINE void generate_key_multilayer(std::uint64_t salt, std::uint64_t address_entropy, void* key_buffer, size_t key_size) {
            std::uint64_t global_secret = security::get_session_key();
            std::uint64_t mix = salt ^ global_secret ^ address_entropy;
            std::uint64_t state1 = KEY_SEED ^ mix;
            std::uint64_t state2 = splitmix64(mix) ^ OBFUSCATOR_A;

            auto* out = static_cast<std::uint8_t*>(key_buffer);
            size_t offset = 0;
            size_t i = 0;

            for (; offset + sizeof(std::uint64_t) <= key_size; offset += sizeof(std::uint64_t), ++i) {
                state1 = splitmix64(state1);
                state2 = splitmix64(state2);
                const std::uint64_t block = state1 ^ rotate_key(state2, static_cast<std::uint8_t>((i * 7) & 63));
                std::memcpy(out + offset, &block, sizeof(block));
            }

            if (offset < key_size) {
                state1 = splitmix64(state1);
                state2 = splitmix64(state2);
                const std::uint64_t block = state1 ^ rotate_key(state2, static_cast<std::uint8_t>((i * 7) & 63));
                std::memcpy(out + offset, &block, key_size - offset);
            }
        }

        RX_FORCEINLINE void secure_wipe(void* ptr, size_t size) {
            volatile char* p = static_cast<volatile char*>(ptr);
            while (size--) *p++ = 0;
            _mm_mfence();
        }

        struct CpuFlags {
            bool has_avx2;
            bool has_sse42;

            CpuFlags() {
                int info[4];
                __cpuid(info, 0);
                int nIds = info[0];

                __cpuid(info, 1);
                has_sse42 = (info[2] & (1 << 20)) != 0;
                bool osxsave = (info[2] & (1 << 27)) != 0;

                has_avx2 = false;
                if (osxsave && nIds >= 7) {
                    unsigned long long xcr0 = _xgetbv(0);
                    if ((xcr0 & 6) == 6) {
                        __cpuidex(info, 7, 0);
                        has_avx2 = (info[1] & (1 << 5)) != 0;
                    }
                }
            }
        };

        RX_FORCEINLINE const CpuFlags& GetCpu() {
            static CpuFlags cpu;
            return cpu;
        }

        RX_FORCEINLINE void xor_fast(void* data, size_t size, const void* key) {
            uint8_t* d = static_cast<uint8_t*>(data);
            const uint8_t* k = static_cast<const uint8_t*>(key);
            size_t i = 0;

            if (GetCpu().has_avx2) {
                for (; i + 32 <= size; i += 32) {
                    __m256i vd = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(d + i));
                    __m256i vk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(k + i));
                    _mm256_storeu_si256(reinterpret_cast<__m256i*>(d + i), _mm256_xor_si256(vd, vk));
                }
                _mm256_zeroupper();
            }
            else if (GetCpu().has_sse42) {
                for (; i + 16 <= size; i += 16) {
                    __m128i vd = _mm_loadu_si128(reinterpret_cast<const __m128i*>(d + i));
                    __m128i vk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(k + i));
                    _mm_storeu_si128(reinterpret_cast<__m128i*>(d + i), _mm_xor_si128(vd, vk));
                }
            }

            for (; i + 8 <= size; i += 8) {
                *reinterpret_cast<std::uint64_t*>(d + i) ^= *reinterpret_cast<const std::uint64_t*>(k + i);
            }
            for (; i < size; i++) {
                d[i] ^= k[i];
            }
        }
    }

    template <size_t MaxSize> class runtime_xorstr {
    private:
        static constexpr size_t STORAGE_SIZE = MaxSize;
        alignas(64) std::array<char, STORAGE_SIZE> _storage{};

        PROTECTED_VALUE(std::uint64_t, _salt, "rx_salt_v355");
        PROTECTED_VALUE(size_t, _size, "rx_size_v322");

        RX_FORCEINLINE void encrypt_in_place() {
            alignas(64) char stack_key[MaxSize];
            std::uint64_t salt = _salt.get_value();
            size_t size = _size.get_value();

            detail::generate_key_multilayer(salt, reinterpret_cast<std::uint64_t>(this), stack_key, size + 1);
            detail::xor_fast(_storage.data(), size + 1, stack_key);
            detail::secure_wipe(stack_key, MaxSize);
        }

        RX_FORCEINLINE void init_empty_encrypted() {
            _size.set_value(0);
            _salt.set_value(detail::generate_salt());
            detail::secure_wipe(_storage.data(), STORAGE_SIZE);
            _storage[0] = '\0';
            encrypt_in_place();
        }

        RX_FORCEINLINE void init_from_plain(const char* plainString) {
            if (!plainString) {
                init_empty_encrypted();
                return;
            }

            const size_t len = strnlen(plainString, MaxSize - 1);
            _size.set_value(len);
            _salt.set_value(detail::generate_salt());

            detail::secure_wipe(_storage.data(), STORAGE_SIZE);
            if (len > 0) {
                std::memcpy(_storage.data(), plainString, len);
            }
            _storage[len] = '\0';
            encrypt_in_place();
        }

        RX_FORCEINLINE void copy_from_other(const runtime_xorstr& other) {
            size_t size = other._size.get_value();
            if (size >= MaxSize) {
                size = MaxSize - 1;
            }

            alignas(64) char stack_key[MaxSize];
            alignas(64) char plain[MaxSize];

            const std::uint64_t other_salt = other._salt.get_value();
            detail::generate_key_multilayer(other_salt, reinterpret_cast<std::uint64_t>(&other), stack_key, size + 1);
            std::memcpy(plain, other._storage.data(), size + 1);
            detail::xor_fast(plain, size + 1, stack_key);
            detail::secure_wipe(stack_key, MaxSize);

            _size.set_value(size);
            _salt.set_value(detail::generate_salt());
            detail::secure_wipe(_storage.data(), STORAGE_SIZE);
            if (size > 0) {
                std::memcpy(_storage.data(), plain, size);
            }
            _storage[size] = '\0';
            encrypt_in_place();

            detail::secure_wipe(plain, MaxSize);
        }

    public:
        RX_FORCEINLINE runtime_xorstr() {
            init_empty_encrypted();
        }

        RX_FORCEINLINE runtime_xorstr(const char* plainString) {
            init_from_plain(plainString);
        }

        RX_FORCEINLINE runtime_xorstr(const runtime_xorstr& other) {
            copy_from_other(other);
        }

        RX_FORCEINLINE runtime_xorstr(runtime_xorstr&& other) noexcept {
            copy_from_other(other);
            other.init_empty_encrypted();
        }

        RX_FORCEINLINE runtime_xorstr& operator=(const runtime_xorstr& other) {
            if (this != &other) {
                copy_from_other(other);
            }
            return *this;
        }

        RX_FORCEINLINE runtime_xorstr& operator=(runtime_xorstr&& other) noexcept {
            if (this != &other) {
                copy_from_other(other);
                other.init_empty_encrypted();
            }
            return *this;
        }

        RX_FORCEINLINE ~runtime_xorstr() {
            detail::secure_wipe(_storage.data(), STORAGE_SIZE);
            _salt.set_value(0);
            _size.set_value(0);
        }

        class scoped_get {
        private:
            alignas(64) char _local_buffer[MaxSize];

#if !defined(_DEBUG)
            PROTECTED_VALUE(std::uint64_t, _canary, "_canary___1111111_v3");
            //std::uint64_t _canary;
#endif

        public:
            RX_FORCEINLINE explicit scoped_get(runtime_xorstr<MaxSize>& ref) {
#if !defined(_DEBUG)
                _canary = __rdtsc();
#endif
                alignas(64) char stack_key[MaxSize];
                std::uint64_t salt = ref._salt.get_value();
                size_t size = ref._size.get_value();

                detail::generate_key_multilayer(salt, reinterpret_cast<std::uint64_t>(&ref), stack_key, size + 1);
                memcpy(_local_buffer, ref._storage.data(), size + 1);
                detail::xor_fast(_local_buffer, size + 1, stack_key);
                detail::secure_wipe(stack_key, MaxSize);

#if !defined(_DEBUG)
                if ((__rdtsc() - _canary) > 24000000) {
                    detail::secure_wipe(_local_buffer, MaxSize);
                    _local_buffer[0] = '\0';
                }
#endif
            }

            RX_FORCEINLINE ~scoped_get() {
                detail::secure_wipe(_local_buffer, MaxSize);
            }

            RX_FORCEINLINE const char* c_str() const { return _local_buffer; }
            RX_FORCEINLINE operator const char* () const { return c_str(); }

            scoped_get(const scoped_get&) = delete;
            scoped_get& operator=(const scoped_get&) = delete;
        };

        RX_FORCEINLINE scoped_get get_scoped() { return scoped_get(*this); }
    };
}
