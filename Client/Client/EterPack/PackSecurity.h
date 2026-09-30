#pragma once
#include <array>
#include <cstdint>
#include <immintrin.h>
#include <intrin.h>
#include <mutex>
#include <utility>

#ifdef _MSC_VER
#define PACK_FORCEINLINE __forceinline
#else
#define PACK_FORCEINLINE __attribute__((always_inline)) inline
#endif

namespace PackSecurity {

    inline bool g_has_avx512f = false;
    inline bool g_has_avx512bw = false;
    inline bool g_has_avx2 = false;
    inline bool g_has_sse42 = false;

    PACK_FORCEINLINE void DetectCPUFeatures() {
        int cpu_info[4];
        __cpuid(cpu_info, 1);

        g_has_sse42 = (cpu_info[2] & (1 << 20)) != 0;
        bool osxsave = (cpu_info[2] & (1 << 27)) != 0;

        if (osxsave) {
            unsigned long long xcr0 = _xgetbv(0);
            if ((xcr0 & 6) == 6) {
                __cpuidex(cpu_info, 7, 0);
                g_has_avx2 = (cpu_info[1] & (1 << 5)) != 0;

                if ((xcr0 & 0xE6) == 0xE6) {
                    g_has_avx512f = (cpu_info[1] & (1 << 16)) != 0;
                    g_has_avx512bw = (cpu_info[1] & (1 << 30)) != 0;
                }
            }
        }
    }

    constexpr uint64_t XXH3_SALT = 0x50E3FDAC7EAA493DULL;
    constexpr uint64_t HASH_OBFUSCATION_KEY = 0x90C50925A6EF3D27ULL;
    constexpr uint64_t HASH_MULTIPLIER = 0xC9A0755894E986FBULL;

    constexpr std::array<uint8_t, 16> XOR_KEY1 = { 0x5B, 0xDB, 0x5A, 0xBB, 0xED, 0x46, 0xBA, 0x94, 0xF4, 0x38, 0x45, 0xB7, 0x25, 0x4B, 0x5E, 0x13 };
    constexpr std::array<uint8_t, 16> XOR_KEY2 = { 0x0F, 0xE4, 0x9E, 0xAF, 0x07, 0xA2, 0xE6, 0x47, 0x5A, 0xF5, 0x13, 0x68, 0xE9, 0x09, 0x24, 0xE4 };
    constexpr std::array<uint8_t, 16> XOR_KEY3 = { 0x0A, 0x01, 0x0D, 0x05, 0x03, 0x08, 0x0B, 0x0C, 0x07, 0x09, 0x0E, 0x02, 0x04, 0x0F, 0x00, 0x06 };
    constexpr std::array<uint8_t, 16> XOR_KEY4 = { 0xE7, 0xD7, 0xC2, 0x9E, 0xCA, 0xD4, 0x85, 0x9B, 0xE7, 0xD1, 0x7C, 0x95, 0xE1, 0xFB, 0x8B, 0x30 };
    constexpr std::array<uint8_t, 16> CACHE_XOR_KEY = { 0x1B, 0xCA, 0x96, 0x6A, 0x3E, 0x07, 0x2C, 0x17, 0x5F, 0x97, 0x21, 0xF1, 0x1B, 0x0A, 0x3E, 0x50 };
    constexpr uint8_t CACHE_ROL_AMOUNT = 23;
    constexpr std::array<uint8_t, 16> ENCRYPTED_PACK_KEY = { 0xF9, 0x8A, 0x85, 0x10, 0x6C, 0x5F, 0xCC, 0x8D, 0xEA, 0x30, 0x27, 0x7B, 0x13, 0x3F, 0x59, 0xD3 };

    constexpr uint32_t CHACHA_CONST_0 = 0xCC58C2B7;
    constexpr uint32_t CHACHA_CONST_1 = 0xD7165D56;
    constexpr uint32_t CHACHA_CONST_2 = 0x06BE3961;
    constexpr uint32_t CHACHA_CONST_3 = 0x5235D3D8;
    constexpr std::array<uint32_t, 4> CHACHA_KEY = { 0x6C08C3F8, 0x5FDB21C3, 0x4A5A6331, 0x8D8B6B57 };

    PACK_FORCEINLINE uint64_t splitmix64(uint64_t x) {
        x ^= x >> 30;
        x *= 0xbf58476d1ce4e5b9ULL;
        x ^= x >> 27;
        x *= 0x94d049bb133111ebULL;
        x ^= x >> 31;
        return x;
    }

    PACK_FORCEINLINE uint64_t obfuscate_hash(uint64_t hash) {
        hash ^= HASH_OBFUSCATION_KEY;
        hash = (hash << 13) | (hash >> 51);
        hash *= HASH_MULTIPLIER;
        hash ^= hash >> 33;
        hash = splitmix64(hash);
        return hash;
    }

    PACK_FORCEINLINE void chacha_quarter_round(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
        a += b; d ^= a; d = (d << 16) | (d >> 16);
        c += d; b ^= c; b = (b << 12) | (b >> 20);
        a += b; d ^= a; d = (d << 8) | (d >> 24);
        c += d; b ^= c; b = (b << 7) | (b >> 25);
    }

    PACK_FORCEINLINE void corrupt_header(uint8_t* data, size_t size) {
        constexpr size_t CORRUPT_SIZE = 0x80;
        const size_t actual_size = (size < CORRUPT_SIZE) ? size : CORRUPT_SIZE;
        constexpr uint8_t CORRUPT_MULT = 0x9D;

        if (actual_size < 16) {
            for (size_t i = 0; i < actual_size; ++i) {
                data[i] ^= static_cast<uint8_t>((i * CORRUPT_MULT) ^ (CHACHA_KEY[i & 3] >> (i & 7)));
            }
            return;
        }

        uint32_t a = CHACHA_CONST_0 + CHACHA_KEY[0];
        uint32_t b = CHACHA_CONST_1 + CHACHA_KEY[1];
        uint32_t c = CHACHA_CONST_2 + CHACHA_KEY[2];
        uint32_t d = CHACHA_CONST_3 + CHACHA_KEY[3];
        size_t offset = 0;


        if (g_has_avx512f && g_has_avx512bw) {
            while (offset + 64 <= actual_size) {
                chacha_quarter_round(a, b, c, d); uint32_t k1 = a ^ b ^ c ^ d;
                chacha_quarter_round(a, b, c, d); uint32_t k2 = a ^ b ^ c ^ d;
                chacha_quarter_round(a, b, c, d); uint32_t k3 = a ^ b ^ c ^ d;
                chacha_quarter_round(a, b, c, d); uint32_t k4 = a ^ b ^ c ^ d;
                __m512i keystream = _mm512_set_epi32(k4, k4, k4, k4, k3, k3, k3, k3, k2, k2, k2, k2, k1, k1, k1, k1);
                __m512i data_vec = _mm512_loadu_si512(reinterpret_cast<const void*>(data + offset));
                data_vec = _mm512_xor_si512(data_vec, keystream);
                _mm512_storeu_si512(reinterpret_cast<void*>(data + offset), data_vec);
                offset += 64;
            }
            // AVX-512 tail: use AVX2 for remaining 32-byte chunk
            if (offset + 32 <= actual_size) {
                chacha_quarter_round(a, b, c, d); uint32_t k1 = a ^ b ^ c ^ d;
                chacha_quarter_round(a, b, c, d); uint32_t k2 = a ^ b ^ c ^ d;
                __m256i keystream = _mm256_set_epi32(k2, k2, k2, k2, k1, k1, k1, k1);
                __m256i data_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + offset));
                data_vec = _mm256_xor_si256(data_vec, keystream);
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(data + offset), data_vec);
                offset += 32;
            }
            _mm256_zeroupper();
        }
        else if (g_has_avx2) {
            while (offset + 32 <= actual_size) {
                chacha_quarter_round(a, b, c, d); uint32_t k1 = a ^ b ^ c ^ d;
                chacha_quarter_round(a, b, c, d); uint32_t k2 = a ^ b ^ c ^ d;
                __m256i keystream = _mm256_set_epi32(k2, k2, k2, k2, k1, k1, k1, k1);
                __m256i data_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + offset));
                data_vec = _mm256_xor_si256(data_vec, keystream);
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(data + offset), data_vec);
                offset += 32;
            }
            _mm256_zeroupper();
        }

        if (g_has_sse42) {
            while (offset + 16 <= actual_size) {
                chacha_quarter_round(a, b, c, d); uint32_t k1 = a ^ b ^ c ^ d;
                __m128i keystream = _mm_set1_epi32(k1);
                __m128i data_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + offset));
                data_vec = _mm_xor_si128(data_vec, keystream);
                _mm_storeu_si128(reinterpret_cast<__m128i*>(data + offset), data_vec);
                offset += 16;
            }
        }
        else {
            while (offset + 16 <= actual_size) {
                chacha_quarter_round(a, b, c, d); uint32_t k1 = a ^ b ^ c ^ d;
                for (size_t i = 0; i < 4; ++i) {
                    *reinterpret_cast<uint32_t*>(data + offset + i * 4) ^= k1;
                }
                offset += 16;
            }
        }

        if (offset < actual_size) {
            chacha_quarter_round(a, b, c, d); uint32_t k1 = a ^ b ^ c ^ d;
            __m128i keystream = _mm_set1_epi32(k1);
            uint8_t ks_buffer[16];
            _mm_storeu_si128(reinterpret_cast<__m128i*>(ks_buffer), keystream);
            for (size_t i = 0; i < actual_size - offset; ++i) {
                data[offset + i] ^= ks_buffer[i];
            }
        }
    }

    PACK_FORCEINLINE void restore_header(uint8_t* data, size_t size) {
        corrupt_header(data, size);
    }

    PACK_FORCEINLINE void apply_xor(std::array<uint8_t, 16>& data, const std::array<uint8_t, 16>& key) {
        __m128i data_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data.data()));
        __m128i key_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(key.data()));
        __m128i result = _mm_xor_si128(data_vec, key_vec);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(data.data()), result);
    }

    PACK_FORCEINLINE void apply_byteswap(std::array<uint8_t, 16>& data) {
        __m128i vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data.data()));
        __m128i shuffle_mask = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        vec = _mm_shuffle_epi8(vec, shuffle_mask);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(data.data()), vec);
    }

    PACK_FORCEINLINE void apply_add(std::array<uint8_t, 16>& data, const std::array<uint8_t, 16>& key) {
        __m128i data_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data.data()));
        __m128i key_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(key.data()));
        __m128i result = _mm_sub_epi32(data_vec, key_vec);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(data.data()), result);
    }

    PACK_FORCEINLINE void apply_rol(std::array<uint8_t, 16>& data) {
        __m128i vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data.data()));
        __m128i result = _mm_or_si128(_mm_srli_epi32(vec, 19), _mm_slli_epi32(vec, 13));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(data.data()), result);
    }

    PACK_FORCEINLINE void apply_shuffle(std::array<uint8_t, 16>& data, const std::array<uint8_t, 16>& key) {
        __m128i data_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data.data()));
        __m128i key_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(key.data()));
        __m128i shuffled = _mm_shuffle_epi8(data_vec, key_vec);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(data.data()), shuffled);
    }

    PACK_FORCEINLINE void apply_cache_encryption(std::array<uint8_t, 16>& data) {
        const __m128i magic_const = _mm_set1_epi64x(0x4FC267FD91C69DA0ULL);
        __m128i vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data.data()));
        __m128i key_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(CACHE_XOR_KEY.data()));
        vec = _mm_xor_si128(vec, key_vec);
        vec = _mm_or_si128(_mm_slli_epi32(vec, CACHE_ROL_AMOUNT), _mm_srli_epi32(vec, 32 - CACHE_ROL_AMOUNT));
        vec = _mm_xor_si128(vec, magic_const);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(data.data()), vec);
    }

    PACK_FORCEINLINE void apply_cache_decryption(std::array<uint8_t, 16>& data) {
        const __m128i magic_const = _mm_set1_epi64x(0x4FC267FD91C69DA0ULL);
        __m128i vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data.data()));
        vec = _mm_xor_si128(vec, magic_const);
        vec = _mm_or_si128(_mm_srli_epi32(vec, CACHE_ROL_AMOUNT), _mm_slli_epi32(vec, 32 - CACHE_ROL_AMOUNT));
        __m128i key_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(CACHE_XOR_KEY.data()));
        vec = _mm_xor_si128(vec, key_vec);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(data.data()), vec);
    }

    namespace detail {
        inline std::array<uint8_t, 16> cached_key_encrypted;
        inline std::once_flag init_flag;

        PACK_FORCEINLINE void decrypt_full(std::array<uint8_t, 16>& out_key) {
            out_key = ENCRYPTED_PACK_KEY;
            apply_xor(out_key, XOR_KEY4);
            apply_shuffle(out_key, XOR_KEY3);
            apply_rol(out_key);
            apply_xor(out_key, XOR_KEY2);
            apply_byteswap(out_key);
            apply_add(out_key, XOR_KEY1);
            apply_xor(out_key, XOR_KEY1);
            apply_xor(out_key, XOR_KEY3);
            apply_byteswap(out_key);
        }
    }

    PACK_FORCEINLINE void GetDecryptedMasterKey(std::array<uint8_t, 16>& out_key) {
        std::call_once(detail::init_flag, []() {
            detail::decrypt_full(detail::cached_key_encrypted);
            apply_cache_encryption(detail::cached_key_encrypted);
            });
        out_key = detail::cached_key_encrypted;
        apply_cache_decryption(out_key);
    }

    PACK_FORCEINLINE void GetDecryptedPackKey(std::array<uint8_t, 16>& out_key) {
        GetDecryptedMasterKey(out_key);
    }

    PACK_FORCEINLINE void derive_pack_key(const std::array<uint8_t, 16>& master_key, const uint8_t* pack_iv, std::array<uint8_t, 16>& out_pack_key) {
        std::copy(master_key.begin(), master_key.end(), out_pack_key.begin());
        for (size_t i = 0; i < 16; ++i) {
            out_pack_key[i] ^= pack_iv[i];
        }
        __m128i key_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(out_pack_key.data()));
        __m128i iv_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pack_iv));
        key_vec = _mm_or_si128(_mm_slli_epi32(key_vec, 13), _mm_srli_epi32(key_vec, 19));
        key_vec = _mm_xor_si128(key_vec, iv_vec);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(out_pack_key.data()), key_vec);
    }

    PACK_FORCEINLINE void derive_file_key(uint64_t file_hash, const std::array<uint8_t, 16>& pack_key, std::array<uint8_t, 16>& out_file_key) {
        std::copy(pack_key.begin(), pack_key.end(), out_file_key.begin());
        uint64_t* key_as_u64 = reinterpret_cast<uint64_t*>(out_file_key.data());
        key_as_u64[0] ^= file_hash;
        key_as_u64[1] ^= ~file_hash;
        __m128i key_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(out_file_key.data()));
        __m128i hash_vec = _mm_set1_epi64x(static_cast<long long>(file_hash));
        key_vec = _mm_or_si128(_mm_slli_epi32(key_vec, 7), _mm_srli_epi32(key_vec, 25));
        key_vec = _mm_xor_si128(key_vec, hash_vec);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(out_file_key.data()), key_vec);
    }

    template <typename Container> PACK_FORCEINLINE void SecureErase(Container& c) {
        volatile uint8_t* p = reinterpret_cast<volatile uint8_t*>(c.data());
        size_t size = c.size() * sizeof(typename Container::value_type);
        while (size--) *p++ = 0;
    }

    PACK_FORCEINLINE void SecureErase(void* ptr, size_t size) {
        volatile uint8_t* p = reinterpret_cast<volatile uint8_t*>(ptr);
        while (size--) *p++ = 0;
    }
}