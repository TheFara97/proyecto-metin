#pragma once
#include <string>
#include <cstdint>
#include <ctime>
#include <array>
#include <cstring>
#include <cctype>
#include "ecdh.h"

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
#include <cpuid.h>
#endif
#include <immintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#endif

#if !defined(BS_CONSTEXPR14)
#if __cplusplus >= 201402L
#define BS_CONSTEXPR14 constexpr
#else
#define BS_CONSTEXPR14 inline
#endif
#endif

namespace ByteShield
{
	namespace consts
	{
		//CHANGE ONLY THIS BASED ON SETTINGS WHICH YOU CAN CHANGE IN BYTESHIELD ADMIN PANEL
		//
		//
		constexpr char SIGN_KEY[] = "N8f1zyuhGDnEzvg0qnFICbfCiiCXUPVe"; //Random 32 characters, hmac key
		constexpr char ECDH_PRIV_KEY[] = "HmauiHfl0zBU2ckUrCIocA4vrkAdWD0zJsww2VdpOUQ="; //NIST256 - Base64 private key
		//
		//

		constexpr int JWT_CLOCK_SKEW = 10;
		constexpr int INIT_JWT_TOKEN_LIFETIME_SECONDS = 30;

		constexpr int MAX_USERID_LEN = 50; //Set this variable to the maximum userId length. Make sure the userId does not include any of these invalid characters: ["'\\]

		constexpr int MAX_TIMESTAMP_LEN = 12;
		constexpr int MAX_SESSION_ID_LEN = 21;

		constexpr int MAX_GAME_TOKEN_PAYLOAD_LEN = 70 + (3 * MAX_TIMESTAMP_LEN) + MAX_USERID_LEN + MAX_SESSION_ID_LEN;
		constexpr int MAX_GAME_TOKEN_PAYLOAD_BASE64_LEN = (MAX_GAME_TOKEN_PAYLOAD_LEN + 2) / 3 * 4;
		constexpr int MAX_SIGNATURE_BASE64_LEN = (32 + 2) / 3 * 4;

		constexpr char JWT_HEADER_BASE64[] = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.";

		constexpr int MAX_GAME_TOKEN_BASE64_LEN = sizeof(JWT_HEADER_BASE64) + MAX_GAME_TOKEN_PAYLOAD_BASE64_LEN + MAX_SIGNATURE_BASE64_LEN + 1;
	}

	namespace sha256
	{
		struct sha256_buff
		{
			uint64_t data_size;
			uint32_t h[8];
			uint8_t last_chunk[64];
			uint8_t chunk_size;
		};

		static const uint32_t k[64] = {
		   0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
		   0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
		   0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
		   0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
		   0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		   0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		   0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
		   0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
		};

		static inline uint32_t rotate_r(uint32_t val, uint32_t bits)
		{
			return (val >> bits | val << (32 - bits));
		}

		static inline void sha256_init(struct sha256_buff* buff)
		{
			buff->h[0] = 0x6a09e667;
			buff->h[1] = 0xbb67ae85;
			buff->h[2] = 0x3c6ef372;
			buff->h[3] = 0xa54ff53a;
			buff->h[4] = 0x510e527f;
			buff->h[5] = 0x9b05688c;
			buff->h[6] = 0x1f83d9ab;
			buff->h[7] = 0x5be0cd19;
			buff->data_size = 0;
			buff->chunk_size = 0;
		}

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
		inline bool support_sha_instructions()
		{
#if defined(_MSC_VER)
			int info[4];
			__cpuidex(info, 7, 0);
			return (info[1] & (1 << 29)) != 0;

#elif defined(__GNUC__) || defined(__clang__)
			unsigned int eax, ebx, ecx, edx;

#if defined(__i386__) && defined(__PIC__)
			__asm__ volatile(
				"xchgl %%ebx, %1\n\t"
				"cpuid\n\t"
				"xchgl %%ebx, %1\n\t"
				: "=a"(eax), "=&r"(ebx), "=c"(ecx), "=d"(edx)
				: "a"(7), "c"(0)
				: "cc"
				);
#else
			__asm__ volatile(
				"cpuid"
				: "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
				: "a"(7), "c"(0)
				: "cc"
				);
#endif

			return (ebx & (1u << 29)) != 0;

#else
			return false;
#endif
		}

#if defined(__GNUC__) || defined(__clang__)
		__attribute__((target("sse4.1,ssse3,sha")))
#endif
			static inline void sha256_calc_chunk_intel(struct sha256_buff* buff, const uint8_t* chunk)
		{
			__m128i STATE0, STATE1;
			__m128i MSG, TMP;
			__m128i MSG0, MSG1, MSG2, MSG3;
			__m128i ABEF_SAVE, CDGH_SAVE;
			const __m128i MASK = _mm_set_epi64x(0x0c0d0e0f08090a0bULL, 0x0405060700010203ULL);

			/* Load initial values */
			TMP = _mm_loadu_si128((const __m128i*) & buff->h[0]);
			STATE1 = _mm_loadu_si128((const __m128i*) & buff->h[4]);

			TMP = _mm_shuffle_epi32(TMP, 0xB1);          /* CDAB */
			STATE1 = _mm_shuffle_epi32(STATE1, 0x1B);    /* EFGH */
			STATE0 = _mm_alignr_epi8(TMP, STATE1, 8);    /* ABEF */
			STATE1 = _mm_blend_epi16(STATE1, TMP, 0xF0); /* CDGH */

			{
				/* Save current state */
				ABEF_SAVE = STATE0;
				CDGH_SAVE = STATE1;

				/* Rounds 0-3 */
				MSG = _mm_loadu_si128((const __m128i*) (chunk + 0));
				MSG0 = _mm_shuffle_epi8(MSG, MASK);
				MSG = _mm_add_epi32(MSG0, _mm_set_epi64x(0xE9B5DBA5B5C0FBCFULL, 0x71374491428A2F98ULL));
				STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
				MSG = _mm_shuffle_epi32(MSG, 0x0E);
				STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

				/* Rounds 4-7 */
				MSG1 = _mm_loadu_si128((const __m128i*) (chunk + 16));
				MSG1 = _mm_shuffle_epi8(MSG1, MASK);
				MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0xAB1C5ED5923F82A4ULL, 0x59F111F13956C25BULL));
				STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
				MSG = _mm_shuffle_epi32(MSG, 0x0E);
				STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
				MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

				/* Rounds 8-11 */
				MSG2 = _mm_loadu_si128((const __m128i*) (chunk + 32));
				MSG2 = _mm_shuffle_epi8(MSG2, MASK);
				MSG = _mm_add_epi32(MSG2, _mm_set_epi64x(0x550C7DC3243185BEULL, 0x12835B01D807AA98ULL));
				STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
				MSG = _mm_shuffle_epi32(MSG, 0x0E);
				STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
				MSG1 = _mm_sha256msg1_epu32(MSG1, MSG2);

				/* Rounds 12-15 */
				MSG3 = _mm_loadu_si128((const __m128i*) (chunk + 48));
				MSG3 = _mm_shuffle_epi8(MSG3, MASK);
				MSG = _mm_add_epi32(MSG3, _mm_set_epi64x(0xC19BF1749BDC06A7ULL, 0x80DEB1FE72BE5D74ULL));
				STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
				TMP = _mm_alignr_epi8(MSG3, MSG2, 4);
				MSG0 = _mm_add_epi32(MSG0, TMP);
				MSG0 = _mm_sha256msg2_epu32(MSG0, MSG3);
				MSG = _mm_shuffle_epi32(MSG, 0x0E);
				STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
				MSG2 = _mm_sha256msg1_epu32(MSG2, MSG3);

				/* Rounds 16-19 */
				MSG = _mm_add_epi32(MSG0, _mm_set_epi64x(0x240CA1CC0FC19DC6ULL, 0xEFBE4786E49B69C1ULL));
				STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
				TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
				MSG1 = _mm_add_epi32(MSG1, TMP);
				MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
				MSG = _mm_shuffle_epi32(MSG, 0x0E);
				STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
				MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

				/* Rounds 20-23 */
				MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0x76F988DA5CB0A9DCULL, 0x4A7484AA2DE92C6FULL));
				STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
				TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
				MSG2 = _mm_add_epi32(MSG2, TMP);
				MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);
				MSG = _mm_shuffle_epi32(MSG, 0x0E);
				STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
				MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

				/* Rounds 24-27 */
				MSG = _mm_add_epi32(MSG2, _mm_set_epi64x(0xBF597FC7B00327C8ULL, 0xA831C66D983E5152ULL));
				STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
				TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
				MSG3 = _mm_add_epi32(MSG3, TMP);
				MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);
				MSG = _mm_shuffle_epi32(MSG, 0x0E);
				STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
				MSG1 = _mm_sha256msg1_epu32(MSG1, MSG2);

				/* Rounds 28-31 */
				MSG = _mm_add_epi32(MSG3, _mm_set_epi64x(0x1429296706CA6351ULL, 0xD5A79147C6E00BF3ULL));
				STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
				TMP = _mm_alignr_epi8(MSG3, MSG2, 4);
				MSG0 = _mm_add_epi32(MSG0, TMP);
				MSG0 = _mm_sha256msg2_epu32(MSG0, MSG3);
				MSG = _mm_shuffle_epi32(MSG, 0x0E);
				STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
				MSG2 = _mm_sha256msg1_epu32(MSG2, MSG3);

				/* Rounds 32-35 */
				MSG = _mm_add_epi32(MSG0, _mm_set_epi64x(0x53380D134D2C6DFCULL, 0x2E1B213827B70A85ULL));
				STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
				TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
				MSG1 = _mm_add_epi32(MSG1, TMP);
				MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
				MSG = _mm_shuffle_epi32(MSG, 0x0E);
				STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
				MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

				/* Rounds 36-39 */
				MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0x92722C8581C2C92EULL, 0x766A0ABB650A7354ULL));
				STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
				TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
				MSG2 = _mm_add_epi32(MSG2, TMP);
				MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);
				MSG = _mm_shuffle_epi32(MSG, 0x0E);
				STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
				MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

				/* Rounds 40-43 */
				MSG = _mm_add_epi32(MSG2, _mm_set_epi64x(0xC76C51A3C24B8B70ULL, 0xA81A664BA2BFE8A1ULL));
				STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
				TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
				MSG3 = _mm_add_epi32(MSG3, TMP);
				MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);
				MSG = _mm_shuffle_epi32(MSG, 0x0E);
				STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
				MSG1 = _mm_sha256msg1_epu32(MSG1, MSG2);

				/* Rounds 44-47 */
				MSG = _mm_add_epi32(MSG3, _mm_set_epi64x(0x106AA070F40E3585ULL, 0xD6990624D192E819ULL));
				STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
				TMP = _mm_alignr_epi8(MSG3, MSG2, 4);
				MSG0 = _mm_add_epi32(MSG0, TMP);
				MSG0 = _mm_sha256msg2_epu32(MSG0, MSG3);
				MSG = _mm_shuffle_epi32(MSG, 0x0E);
				STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
				MSG2 = _mm_sha256msg1_epu32(MSG2, MSG3);

				/* Rounds 48-51 */
				MSG = _mm_add_epi32(MSG0, _mm_set_epi64x(0x34B0BCB52748774CULL, 0x1E376C0819A4C116ULL));
				STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
				TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
				MSG1 = _mm_add_epi32(MSG1, TMP);
				MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
				MSG = _mm_shuffle_epi32(MSG, 0x0E);
				STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
				MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

				/* Rounds 52-55 */
				MSG = _mm_add_epi32(MSG1, _mm_set_epi64x(0x682E6FF35B9CCA4FULL, 0x4ED8AA4A391C0CB3ULL));
				STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
				TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
				MSG2 = _mm_add_epi32(MSG2, TMP);
				MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);
				MSG = _mm_shuffle_epi32(MSG, 0x0E);
				STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

				/* Rounds 56-59 */
				MSG = _mm_add_epi32(MSG2, _mm_set_epi64x(0x8CC7020884C87814ULL, 0x78A5636F748F82EEULL));
				STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
				TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
				MSG3 = _mm_add_epi32(MSG3, TMP);
				MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);
				MSG = _mm_shuffle_epi32(MSG, 0x0E);
				STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

				/* Rounds 60-63 */
				MSG = _mm_add_epi32(MSG3, _mm_set_epi64x(0xC67178F2BEF9A3F7ULL, 0xA4506CEB90BEFFFAULL));
				STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
				MSG = _mm_shuffle_epi32(MSG, 0x0E);
				STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

				/* Combine state  */
				STATE0 = _mm_add_epi32(STATE0, ABEF_SAVE);
				STATE1 = _mm_add_epi32(STATE1, CDGH_SAVE);

			}

			TMP = _mm_shuffle_epi32(STATE0, 0x1B);       /* FEBA */
			STATE1 = _mm_shuffle_epi32(STATE1, 0xB1);    /* DCHG */
			STATE0 = _mm_blend_epi16(TMP, STATE1, 0xF0); /* DCBA */
			STATE1 = _mm_alignr_epi8(STATE1, TMP, 8);    /* ABEF */

			/* Save state */
			_mm_storeu_si128((__m128i*) & buff->h[0], STATE0);
			_mm_storeu_si128((__m128i*) & buff->h[4], STATE1);
		}
#endif

		static inline void sha256_calc_chunk(struct sha256_buff* buff, const uint8_t* chunk)
		{
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
			static bool use_sha_ni = support_sha_instructions();
			if (use_sha_ni) return sha256_calc_chunk_intel(buff, chunk);
#endif
			uint32_t w[64];
			uint32_t tv[8];
			uint32_t i;

			for (i = 0; i < 16; ++i) {
				w[i] = (uint32_t)chunk[0] << 24 | (uint32_t)chunk[1] << 16 | (uint32_t)chunk[2] << 8 | (uint32_t)chunk[3];
				chunk += 4;
			}

			for (i = 16; i < 64; ++i) {
				uint32_t s0 = rotate_r(w[i - 15], 7) ^ rotate_r(w[i - 15], 18) ^ (w[i - 15] >> 3);
				uint32_t s1 = rotate_r(w[i - 2], 17) ^ rotate_r(w[i - 2], 19) ^ (w[i - 2] >> 10);
				w[i] = w[i - 16] + s0 + w[i - 7] + s1;
			}

			for (i = 0; i < 8; ++i)
				tv[i] = buff->h[i];

			for (i = 0; i < 64; ++i) {
				uint32_t S1 = rotate_r(tv[4], 6) ^ rotate_r(tv[4], 11) ^ rotate_r(tv[4], 25);
				uint32_t ch = (tv[4] & tv[5]) ^ (~tv[4] & tv[6]);
				uint32_t temp1 = tv[7] + S1 + ch + k[i] + w[i];
				uint32_t S0 = rotate_r(tv[0], 2) ^ rotate_r(tv[0], 13) ^ rotate_r(tv[0], 22);
				uint32_t maj = (tv[0] & tv[1]) ^ (tv[0] & tv[2]) ^ (tv[1] & tv[2]);
				uint32_t temp2 = S0 + maj;

				tv[7] = tv[6];
				tv[6] = tv[5];
				tv[5] = tv[4];
				tv[4] = tv[3] + temp1;
				tv[3] = tv[2];
				tv[2] = tv[1];
				tv[1] = tv[0];
				tv[0] = temp1 + temp2;
			}

			for (i = 0; i < 8; ++i)
				buff->h[i] += tv[i];
		}

		static inline void sha256_update(struct sha256_buff* buff, const void* data, size_t size) {
			const uint8_t* ptr = (const uint8_t*)data;
			buff->data_size += size;
			/* If there is data left in buff, concatenate it to process as new chunk */
			if (size + buff->chunk_size >= 64) {
				uint8_t tmp_chunk[64];
				memcpy(tmp_chunk, buff->last_chunk, buff->chunk_size);
				memcpy(tmp_chunk + buff->chunk_size, ptr, 64 - buff->chunk_size);
				ptr += (64 - buff->chunk_size);
				size -= (64 - buff->chunk_size);
				buff->chunk_size = 0;
				sha256_calc_chunk(buff, tmp_chunk);
			}

			/* Run over data chunks */
			while (size >= 64) {
				sha256_calc_chunk(buff, ptr);
				ptr += 64;
				size -= 64;
			}

			/* Save remaining data in buff, will be reused on next call or finalize */
			memcpy(buff->last_chunk + buff->chunk_size, ptr, size);
			buff->chunk_size += static_cast<std::uint8_t>(size);
		}

		static inline void sha256_finalize(struct sha256_buff* buff)
		{
			buff->last_chunk[buff->chunk_size] = 0x80;
			buff->chunk_size++;
			memset(buff->last_chunk + buff->chunk_size, 0, 64 - buff->chunk_size);

			/* If there isn't enough space to fit int64, pad chunk with zeroes and prepare next chunk */
			if (buff->chunk_size > 56) {
				sha256_calc_chunk(buff, buff->last_chunk);
				memset(buff->last_chunk, 0, 64);
			}

			/* Add total size as big-endian int64 x8 */
			uint64_t size = buff->data_size * 8;
			int i;
			for (i = 8; i > 0; --i) {
				buff->last_chunk[55 + i] = size & 255;
				size >>= 8;
			}

			sha256_calc_chunk(buff, buff->last_chunk);
		}

		static inline void sha256_read(const struct sha256_buff* buff, uint8_t* hash)
		{
			uint32_t i;
			for (i = 0; i < 8; i++) {
				hash[i * 4] = (buff->h[i] >> 24) & 255;
				hash[i * 4 + 1] = (buff->h[i] >> 16) & 255;
				hash[i * 4 + 2] = (buff->h[i] >> 8) & 255;
				hash[i * 4 + 3] = buff->h[i] & 255;
			}
		}

		namespace hmac
		{
			static inline void hmac_jwt_sha256(const void* key,
				const size_t keylen,
				const void* payload,
				const size_t payloadlen,
				void* out)
			{
				uint8_t k[64];
				uint8_t k_ipad[64];
				uint8_t k_opad[64];
				uint8_t ihash[256 / 8];

				memset(k, 0, sizeof(k));
				memset(k_ipad, 0x36, 64);
				memset(k_opad, 0x5c, 64);

				if (keylen > 64)
				{
					struct sha256_buff buff;
					sha256_init(&buff);
					sha256_update(&buff, key, keylen);
					sha256_finalize(&buff);
					sha256_read(&buff, k);
				}
				else
				{
					memcpy(k, key, keylen);
				}

				for (int i = 0; i < 64; i++)
				{
					k_ipad[i] ^= k[i];
					k_opad[i] ^= k[i];
				}

				{
					struct sha256_buff buff;
					sha256_init(&buff);
					sha256_update(&buff, k_ipad, sizeof(k_ipad));
					sha256_update(&buff, payload, payloadlen);
					sha256_finalize(&buff);
					sha256_read(&buff, ihash);
				}

				{
					struct sha256_buff buff;
					sha256_init(&buff);
					sha256_update(&buff, k_opad, sizeof(k_opad));
					sha256_update(&buff, ihash, sizeof(ihash));
					sha256_finalize(&buff);
					sha256_read(&buff, (uint8_t*)out);
				}
			};
		};
	};

	namespace random
	{
		static inline uint32_t xorshift32(uint32_t state)
		{
			state ^= state << 13;
			state ^= state >> 17;
			state ^= state << 5;
			return state;
		}

		static inline void random_string(char* buffer, int length, uint32_t seed)
		{
			const char characters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

			for (int i = 0; i < length; ++i)
			{
				seed = xorshift32(seed);
				buffer[i] = characters[seed % (sizeof(characters) - 1)];
			}

			buffer[length] = '\0';
		}
	}

	namespace base64
	{
		static const char* base64_chars[2] = {
				 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				 "abcdefghijklmnopqrstuvwxyz"
				 "0123456789"
				 "+/",

				 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				 "abcdefghijklmnopqrstuvwxyz"
				 "0123456789"
				 "-_" };

		BS_CONSTEXPR14 unsigned int pos_of_char(const unsigned char chr)
		{
			//
			// Return the position of chr within base64_encode()
			//

			if (chr >= 'A' && chr <= 'Z') return chr - 'A';
			else if (chr >= 'a' && chr <= 'z') return chr - 'a' + ('Z' - 'A') + 1;
			else if (chr >= '0' && chr <= '9') return chr - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
			else if (chr == '+' || chr == '-') return 62; // Be liberal with input and accept both url ('-') and non-url ('+') base 64 characters (
			else if (chr == '/' || chr == '_') return 63; // Ditto for '/' and '_'
			else return 0; //Exceptions sucks
		}

		static inline size_t base64_encode(char* buffer, size_t buffer_size, unsigned char const* bytes_to_encode, size_t in_len, bool url)
		{
			size_t len_encoded = (in_len + 2) / 3 * 4;
			if (len_encoded > buffer_size) return 0;

			unsigned char trailing_char = url ? '.' : '=';

			const char* base64_chars_ = base64_chars[url];

			size_t cursor = 0;
			unsigned int pos = 0;

			while (pos < in_len)
			{
				buffer[cursor++] = (base64_chars_[(bytes_to_encode[pos + 0] & 0xfc) >> 2]);

				if (pos + 1 < in_len)
				{
					buffer[cursor++] = (base64_chars_[((bytes_to_encode[pos + 0] & 0x03) << 4) + ((bytes_to_encode[pos + 1] & 0xf0) >> 4)]);

					if (pos + 2 < in_len)
					{
						buffer[cursor++] = (base64_chars_[((bytes_to_encode[pos + 1] & 0x0f) << 2) + ((bytes_to_encode[pos + 2] & 0xc0) >> 6)]);
						buffer[cursor++] = (base64_chars_[bytes_to_encode[pos + 2] & 0x3f]);
					}
					else
					{
						buffer[cursor++] = (base64_chars_[(bytes_to_encode[pos + 1] & 0x0f) << 2]);

						if (trailing_char != '.') buffer[cursor++] = (trailing_char);
					}
				}
				else
				{

					buffer[cursor++] = (base64_chars_[(bytes_to_encode[pos + 0] & 0x03) << 4]);
					if (trailing_char != '.') buffer[cursor++] = (trailing_char);
					if (trailing_char != '.') buffer[cursor++] = (trailing_char);
				}

				pos += 3;
			}


			return cursor;
		}

		static inline size_t base64_decode(unsigned char* buffer, size_t buffer_size, const char* encoded_string, const size_t length_of_string)
		{
			size_t approx_length_of_decoded_string = length_of_string / 4 * 3;

			if (approx_length_of_decoded_string > buffer_size) return 0;

			size_t pos = 0;
			size_t cursor = 0;

			while (pos < length_of_string)
			{
				size_t pos_of_char_1 = pos_of_char(encoded_string[pos + 1]);

				buffer[cursor++] = (static_cast<std::string::value_type>(((pos_of_char(encoded_string[pos + 0])) << 2) + ((pos_of_char_1 & 0x30) >> 4)));

				if ((pos + 2 < length_of_string) &&
					encoded_string[pos + 2] != '=' &&
					encoded_string[pos + 2] != '.'
					)
				{
					unsigned int pos_of_char_2 = pos_of_char(encoded_string[pos + 2]);
					buffer[cursor++] = (static_cast<std::string::value_type>(((pos_of_char_1 & 0x0f) << 4) + ((pos_of_char_2 & 0x3c) >> 2)));

					if ((pos + 3 < length_of_string) &&
						encoded_string[pos + 3] != '=' &&
						encoded_string[pos + 3] != '.'
						)
					{
						buffer[cursor++] = (static_cast<std::string::value_type>(((pos_of_char_2 & 0x03) << 6) + pos_of_char(encoded_string[pos + 3])));
					}
				}

				pos += 4;
			}

			return cursor;
		}

		template<size_t N>
		BS_CONSTEXPR14 std::array<std::uint8_t, 32> decode_compiletime(const char(&in)[N])
		{
			std::array<std::uint8_t, 32> out = { {0} };
			size_t pos = 0;
			size_t cur = 0;
			const size_t in_len = N - 1;

			while (pos < in_len && cur < 32)
			{
				int c0 = pos_of_char(in[pos + 0]);
				int c1 = pos_of_char(in[pos + 1]);

				out[cur++] = (std::uint8_t)((c0 << 2) | ((c1 & 0x30) >> 4));

				if (pos + 2 < in_len && in[pos + 2] != '=' && in[pos + 2] != '.') {
					int c2 = pos_of_char(in[pos + 2]);
					out[cur++] = (std::uint8_t)(((c1 & 0x0F) << 4) | ((c2 & 0x3C) >> 2));

					if (pos + 3 < in_len && in[pos + 3] != '=' && in[pos + 3] != '.') {
						int c3 = pos_of_char(in[pos + 3]);
						out[cur++] = (std::uint8_t)(((c2 & 0x03) << 6) | c3);
					}
				}

				pos += 4;
			}

			return out;
		}
	};

	namespace encryption
	{
		const std::array<std::uint8_t, 32> ecdh_private_key = base64::decode_compiletime(consts::ECDH_PRIV_KEY);

		// AES S-Box for cryptographically strong non-linearity
		static constexpr std::uint8_t SBOX[256] = {
			0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
			0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
			0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
			0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
			0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
			0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
			0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
			0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
			0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
			0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
			0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
			0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
			0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
			0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
			0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
			0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
		};

		struct BSEncryptionContext
		{
			std::uint8_t input_key[16];
			std::uint8_t output_key[16];

			std::uint8_t input_key_cursor;
			std::uint8_t output_key_cursor;

			bool initialized = false;
		};

		inline void rotate_key(std::uint8_t* key)
		{
			std::uint8_t tmp[16];

			for (std::size_t i = 0; i < 16; ++i)
				tmp[i] = SBOX[key[i] ^ key[(i + 5) & 0xF]];

			for (std::size_t i = 0; i < 16; ++i)
				key[i] = tmp[i] ^ tmp[(i + 7) & 0xF] ^ tmp[(i + 11) & 0xF];

			for (std::size_t i = 0; i < 16; ++i)
				key[i] = SBOX[key[i]];
		}

		inline void xor_cipher(std::uint8_t* key, std::uint8_t& cursor, std::uint8_t* buffer, std::size_t size, bool encrypt)
		{
			for (std::size_t i = 0; i < size; ++i)
			{
				if (encrypt)
				{
					buffer[i] ^= key[cursor];
					key[cursor] ^= buffer[i];
				}
				else
				{
					const auto old = buffer[i];
					buffer[i] ^= key[cursor];
					key[cursor] ^= old;
				}

				++cursor;

				if (cursor == 16)
				{
					rotate_key(key);
					cursor = 0;
				}
			}
		}

		inline bool exchange_keys(BSEncryptionContext* context, std::uint8_t* client_pub_key)
		{
			std::uint8_t secret[32];

			if (p256_ecdh_shared_secret(secret, ecdh_private_key.data(), client_pub_key) != P256_SUCCESS) return false;

			std::uint8_t key[32];
			{
				struct sha256::sha256_buff buff;
				sha256::sha256_init(&buff);
				sha256::sha256_update(&buff, secret, 32);
				sha256::sha256_finalize(&buff);
				sha256::sha256_read(&buff, key);
			}

			memcpy(context->input_key, key + 16, 16);
			memcpy(context->output_key, key, 16);

			context->input_key_cursor = 0;
			context->output_key_cursor = 0;

			std::memset(secret, 0, 32);

			return true;
		}
	};

	namespace jwt
	{
		static inline size_t parse_unquoted(const char* s, size_t size, std::size_t& i)
		{
			std::size_t start = i;
			while (i < size)
			{
				char c = s[i];
				if (c == ',' || c == '}' || std::isspace((unsigned char)c)) break;
				++i;
			}
			return i - start;
		}

		static inline size_t json_escape_write(char* buffer, size_t cursor, const char* data)
		{
			for (size_t i = 0u; ; i++)
			{
				char c = data[i];
				if (!c) break;

				if (c < 0x20)
					continue;

				switch (c) {
				case '\"':
				case '\\':
				case '\b':
				case '\f':
				case '\n':
				case '\r':
				case '\t':  break;
				default:
					buffer[cursor++] = c;
				}
			}

			return cursor;
		}

		static inline size_t lltoa_fast(long long value, char* buffer, size_t size)
		{
			char* p = buffer;
			const char* end = buffer + size;

			bool negative = value < 0;
			unsigned long long v = negative ? -value : value;

			char* start = p;
			do {
				if (p == end) break;
				*p++ = '0' + (v % 10);
				v /= 10;
			} while (v > 0);

			if (negative && p != end) *p++ = '-';

			{
				char* left = start;
				char* right = p - 1;

				while (left < right) {
					char tmp = *left;
					*left = *right;
					*right = tmp;
					++left;
					--right;
				}
			}

			return p - buffer;
		}

		static inline long long atoll_n(const char* buf, size_t len)
		{
			const char* p = buf;
			const char* end = buf + len;

			while (p < end && (*p == ' ' || *p == '\t' || *p == '\n' ||
				*p == '\r' || *p == '\v' || *p == '\f')) {
				p++;
			}

			int neg = 0;
			if (p < end) {
				if (*p == '-') {
					neg = 1;
					p++;
				}
				else if (*p == '+') {
					p++;
				}
			}

			long long v = 0;
			while (p < end) {
				unsigned char c = (unsigned char)*p;
				if (c < '0' || c > '9')
					break;

				v = v * 10 + (long long)(c - '0');
				p++;
			}

			return neg ? -v : v;
		}
	}

	static inline size_t generate_byteshield_token(char* buffer, const char* user_id, const char* session_id)
	{
		char payload[consts::MAX_GAME_TOKEN_PAYLOAD_LEN];
		char iat[consts::MAX_TIMESTAMP_LEN];
		char exp[consts::MAX_TIMESTAMP_LEN];

		std::time_t timestamp = std::time(nullptr);

		size_t buffer_cursor = sizeof(consts::JWT_HEADER_BASE64) - 1;
		memcpy(buffer, consts::JWT_HEADER_BASE64, sizeof(consts::JWT_HEADER_BASE64) - 1);

		size_t payload_cursor = 0;

		size_t iat_size = jwt::lltoa_fast(timestamp, iat, sizeof(iat));
		size_t exp_size = jwt::lltoa_fast(timestamp + consts::INIT_JWT_TOKEN_LIFETIME_SECONDS, exp, sizeof(exp));

		payload[payload_cursor++] = '{';
		{
			memcpy(payload + payload_cursor, "\"userId\":\"", 10); payload_cursor += 10;
			payload_cursor = jwt::json_escape_write(payload, payload_cursor, user_id);
			memcpy(payload + payload_cursor, "\",", 2); payload_cursor += 2;

			memcpy(payload + payload_cursor, "\"userSession\":\"", 15); payload_cursor += 15;
			payload_cursor = jwt::json_escape_write(payload, payload_cursor, session_id);
			memcpy(payload + payload_cursor, "\",", 2); payload_cursor += 2;

			memcpy(payload + payload_cursor, "\"nbf\":", 6); payload_cursor += 6;
			memcpy(payload + payload_cursor, iat, iat_size); payload_cursor += iat_size;
			payload[payload_cursor++] = ',';

			memcpy(payload + payload_cursor, "\"exp\":", 6); payload_cursor += 6;
			memcpy(payload + payload_cursor, exp, exp_size); payload_cursor += exp_size;
			payload[payload_cursor++] = ',';

			memcpy(payload + payload_cursor, "\"iat\":", 6); payload_cursor += 6;
			memcpy(payload + payload_cursor, iat, iat_size); payload_cursor += iat_size;
			payload[payload_cursor++] = ',';

			memcpy(payload + payload_cursor, "\"iss\":\"GameServer\"", 18); payload_cursor += 18;
		}
		payload[payload_cursor++] = '}';

		std::uint8_t signature_hash[32];

		buffer_cursor += base64::base64_encode(buffer + buffer_cursor, consts::MAX_GAME_TOKEN_PAYLOAD_BASE64_LEN, reinterpret_cast<const unsigned char*>(payload), payload_cursor, true);

		sha256::hmac::hmac_jwt_sha256(consts::SIGN_KEY, sizeof(consts::SIGN_KEY) - 1, buffer, buffer_cursor, signature_hash);

		buffer[buffer_cursor++] = '.';
		buffer_cursor += base64::base64_encode(buffer + buffer_cursor, consts::MAX_SIGNATURE_BASE64_LEN, signature_hash, sizeof(signature_hash), true);
		buffer[buffer_cursor] = '\0';

		return buffer_cursor;
	}

	struct token_info
	{
		char user_id[consts::MAX_USERID_LEN + 1];
		char session_id[consts::MAX_SESSION_ID_LEN + 1];
	};

	static inline bool decode_byteshield_anticheat_token(const char* token, size_t token_size, token_info& result)
	{
		const char* token_end = token + token_size;
		const char* message = strchr(token, '.');
		if (!message) return false;

		const char* signature = strchr(message + 1, '.');
		if (!signature) return false;

		std::uint8_t signature_hash[32 + 2];

		const auto signature_hash_size = base64::base64_decode(signature_hash, sizeof(signature_hash), signature + 1, token_end - signature - 1);
		if (signature_hash_size != 32) return false;

		std::uint8_t signature_hash_calc[32];
		sha256::hmac::hmac_jwt_sha256(consts::SIGN_KEY, sizeof(consts::SIGN_KEY) - 1, token, signature - token, signature_hash_calc);
		if (memcmp(signature_hash, signature_hash_calc, sizeof(signature_hash_calc)) != 0) return false;

		std::time_t timestamp = std::time(nullptr);
		bool issuer_validated = false;
		bool exp_time_validated = false;
		bool nbf_time_validated = false;

		char payload[consts::MAX_GAME_TOKEN_PAYLOAD_LEN + 2];
		size_t payload_size = base64::base64_decode((unsigned char*)payload, sizeof(payload), message + 1, signature - message - 1);

		size_t i = 0;

		if (i < payload_size && payload[i] == '{') ++i;

		while (i < payload_size)
		{
			if (i >= payload_size || payload[i] == '}') break;

			if (payload[i] != '"')
			{
				++i;
				continue;
			}

			char* key = payload + i + 1;
			const char* key_end = strchr(key, '\"');
			if (!key_end) break;

			size_t key_length = key_end - key;

			i += key_end - key + 2;

			if (i >= payload_size || payload[i] != ':')
			{
				break;
			}

			++i;

			size_t value_length = 0;
			char* value = payload + i;
			if (i < payload_size && payload[i] == '"')
			{
				value++;
				const char* value_end = strchr(value, '\"');
				if (!value_end) break;

				value_length = value_end - value;
				i += value_end - value + 2;
			}
			else
			{
				value_length = jwt::parse_unquoted(payload, payload_size, i);
			}

			if (strncmp(key, "iss", key_length) == 0 && strncmp(value, "Anticheat", value_length) == 0) issuer_validated = true;
			else if (strncmp(key, "exp", key_length) == 0)
			{
				const auto exp = jwt::atoll_n(value, value_length) + consts::JWT_CLOCK_SKEW;
				if (exp >= timestamp) exp_time_validated = true;
			}
			else if (strncmp(key, "nbf", key_length) == 0)
			{
				const auto nbf = jwt::atoll_n(value, value_length) - consts::JWT_CLOCK_SKEW;
				if (timestamp >= nbf) nbf_time_validated = true;
			}
			else if (strncmp(key, "id", key_length) == 0)
			{
				memcpy(result.user_id, value, value_length);
				result.user_id[value_length] = '\0';
			}
			else if (strncmp(key, "session", key_length) == 0)
			{
				memcpy(result.session_id, value, value_length);
				result.session_id[value_length] = '\0';
			}

			if (i < payload_size && payload[i] == ',')
			{
				++i;
				continue;
			}

			if (i < payload_size && payload[i] == '}') break;
		}

		if (!issuer_validated || !exp_time_validated || !nbf_time_validated)
			return false;

		return true;
	}
};