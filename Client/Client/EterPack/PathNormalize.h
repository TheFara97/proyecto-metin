#pragma once
#include <array>
#include <cstddef>
#include <cstring>
#include <immintrin.h>
#include "PackSecurity.h"

namespace PathUtils {

    PACK_FORCEINLINE constexpr std::array<char, 256> make_tolower_table() noexcept {
        std::array<char, 256> table{};
        for (int i = 0; i < 256; ++i) {
            if (i >= 'A' && i <= 'Z')
                table[i] = static_cast<char>(i + ('a' - 'A'));
            else
                table[i] = static_cast<char>(i);
        }
        return table;
    }
    static constexpr std::array<char, 256> TOLOWER_TABLE = make_tolower_table();

    PACK_FORCEINLINE void NormalizePathForHashing(const char* input, size_t input_size, char* output) noexcept {
        size_t i = 0;


        if (PackSecurity::g_has_avx512f && PackSecurity::g_has_avx512bw && input_size >= 64) {
            const __m512i slash_mask = _mm512_set1_epi8('\\');
            const __m512i fslash = _mm512_set1_epi8('/');
            const __m512i upper_A = _mm512_set1_epi8('A' - 1);
            const __m512i upper_Z = _mm512_set1_epi8('Z' + 1);
            const __m512i to_lower = _mm512_set1_epi8(32);

            for (; i + 64 <= input_size; i += 64) {
                __m512i data = _mm512_loadu_si512(reinterpret_cast<const void*>(input + i));
                __mmask64 is_slash = _mm512_cmpeq_epi8_mask(data, slash_mask);
                data = _mm512_mask_blend_epi8(is_slash, data, fslash);
                __mmask64 gt_A = _mm512_cmpgt_epi8_mask(data, upper_A);
                __mmask64 lt_Z = _mm512_cmpgt_epi8_mask(upper_Z, data);
                __mmask64 is_upper = gt_A & lt_Z;
                data = _mm512_mask_add_epi8(data, is_upper, data, to_lower);
                _mm512_storeu_si512(reinterpret_cast<void*>(output + i), data);
            }
            // AVX-512 tail: remaining 32-byte chunks
            if (i + 32 <= input_size) {
                const __m256i slash_mask256 = _mm256_set1_epi8('\\');
                const __m256i fslash256 = _mm256_set1_epi8('/');
                const __m256i upper_A256 = _mm256_set1_epi8('A' - 1);
                const __m256i upper_Z256 = _mm256_set1_epi8('Z' + 1);
                const __m256i to_lower256 = _mm256_set1_epi8(32);
                for (; i + 32 <= input_size; i += 32) {
                    __m256i data = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(input + i));
                    __m256i is_slash = _mm256_cmpeq_epi8(data, slash_mask256);
                    data = _mm256_blendv_epi8(data, fslash256, is_slash);
                    __m256i gt_A = _mm256_cmpgt_epi8(data, upper_A256);
                    __m256i lt_Z = _mm256_cmpgt_epi8(upper_Z256, data);
                    __m256i is_upper = _mm256_and_si256(gt_A, lt_Z);
                    data = _mm256_add_epi8(data, _mm256_and_si256(is_upper, to_lower256));
                    _mm256_storeu_si256(reinterpret_cast<__m256i*>(output + i), data);
                }
            }
            _mm256_zeroupper();
        }
        else if (PackSecurity::g_has_avx2 && input_size >= 32) {
            const __m256i slash_mask = _mm256_set1_epi8('\\');
            const __m256i fslash = _mm256_set1_epi8('/');
            const __m256i upper_A = _mm256_set1_epi8('A' - 1);
            const __m256i upper_Z = _mm256_set1_epi8('Z' + 1);
            const __m256i to_lower = _mm256_set1_epi8(32);

            for (; i + 32 <= input_size; i += 32) {
                __m256i data = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(input + i));
                __m256i is_slash = _mm256_cmpeq_epi8(data, slash_mask);
                data = _mm256_blendv_epi8(data, fslash, is_slash);
                __m256i gt_A = _mm256_cmpgt_epi8(data, upper_A);
                __m256i lt_Z = _mm256_cmpgt_epi8(upper_Z, data);
                __m256i is_upper = _mm256_and_si256(gt_A, lt_Z);
                data = _mm256_add_epi8(data, _mm256_and_si256(is_upper, to_lower));
                _mm256_storeu_si256(reinterpret_cast<__m256i*>(output + i), data);
            }
            _mm256_zeroupper();
        }

        if (PackSecurity::g_has_sse42 && input_size >= 16) {
            const __m128i slash_mask = _mm_set1_epi8('\\');
            const __m128i fslash = _mm_set1_epi8('/');
            const __m128i upper_A = _mm_set1_epi8('A' - 1);
            const __m128i upper_Z = _mm_set1_epi8('Z' + 1);
            const __m128i to_lower = _mm_set1_epi8(32);

            for (; i + 16 <= input_size; i += 16) {
                __m128i data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(input + i));
                __m128i is_slash = _mm_cmpeq_epi8(data, slash_mask);
                data = _mm_blendv_epi8(data, fslash, is_slash);
                __m128i gt_A = _mm_cmpgt_epi8(data, upper_A);
                __m128i lt_Z = _mm_cmpgt_epi8(upper_Z, data);
                __m128i is_upper = _mm_and_si128(gt_A, lt_Z);
                data = _mm_add_epi8(data, _mm_and_si128(is_upper, to_lower));
                _mm_storeu_si128(reinterpret_cast<__m128i*>(output + i), data);
            }
        }

        for (; i < input_size; ++i) {
            char c = input[i];
            output[i] = (c == '\\') ? '/' : TOLOWER_TABLE[static_cast<unsigned char>(c)];
        }
    }

    PACK_FORCEINLINE size_t NormalizePathBasic(const char* input, size_t input_size, char* output) noexcept {
        NormalizePathForHashing(input, input_size, output);
        return input_size;
    }

    PACK_FORCEINLINE size_t PreparePathForHashing(const char* input, size_t input_size, char* output, bool strip_prefix) noexcept {
        NormalizePathForHashing(input, input_size, output);
        static const char* prefix = "d:/ymir work/";
        static const size_t prefix_len = 13;
        if (strip_prefix && input_size >= prefix_len && std::memcmp(output, prefix, prefix_len) == 0) {
            const size_t new_size = input_size - prefix_len;
            std::memmove(output, output + prefix_len, new_size);
            return new_size;
        }
        return input_size;
    }
}
