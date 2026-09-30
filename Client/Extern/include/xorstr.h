// Copyright (c) 2010-2014, Sebastien Andrivet
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from this
//    software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#pragma once
#include <Windows.h>
#include <string>
#include <array>
#include <utility>
#include <cstdarg>

#include <stdbool.h>
#include <stdint.h>

namespace xorstr_impl {

#ifdef _MSC_VER
#define XORSTR_INLINE __forceinline
#else
#define XORSTR_INLINE inline
#endif

	constexpr auto time = __TIME__;
	constexpr auto seed = static_cast<int>(time[7]) +
		static_cast<int>(time[6]) * 10 +
		static_cast<int>(time[4]) * 60 +
		static_cast<int>(time[3]) * 600 +
		static_cast<int>(time[1]) * 3600 +
		static_cast<int>(time[0]) * 36000;

	// 1988, Stephen Park and Keith Miller
	// "Random Number Generators: Good Ones Are Hard To Find", considered as "minimal standard"
	// Park-Miller 31 bit pseudo-random number generator, implemented with G. Carta's optimisation:
	// with 32-bit math and without division

	template <int N>
	struct random_generator
	{
	private:
		static constexpr unsigned a = 16807;  // 7^5
		static constexpr unsigned m = 2147483647;  // 2^31 - 1
		static constexpr unsigned s = random_generator<N - 1>::value;
		static constexpr unsigned lo = a * (s & 0xFFFF);  // multiply lower 16 bits by 16807
		static constexpr unsigned hi = a * (s >> 16);  // multiply higher 16 bits by 16807
		static constexpr unsigned lo2 = lo + ((hi & 0x7FFF) << 16);  // combine lower 15 bits of hi with lo's upper bits
		static constexpr unsigned hi2 = hi >> 15;  // discard lower 15 bits of hi
		static constexpr unsigned lo3 = lo2 + hi;

	public:
		static constexpr unsigned max = m;
		static constexpr unsigned value = lo3 > m ? lo3 - m : lo3;
	};

	template <>
	struct random_generator<0>
	{
		static constexpr unsigned value = seed;
	};

	template <int N, int M>
	struct random_int
	{
		static constexpr auto value = random_generator<N + 1>::value % M;
	};

	template <int N, class T>
	struct random_char
	{
		static constexpr T value = static_cast<T>(1 + random_int<N, (1 << ((sizeof(T) * 8) - 1)) - 1>::value);
	};

	template <class T, size_t N, int K>
	struct basic_string
	{
	private:
		using _T = std::decay_t<T>;

		const _T key_;
		std::array<_T, N + 1> encrypted_;

		constexpr _T enc(_T c) const
		{
			return c ^ key_;
		}

		_T dec(_T c) const
		{
			return c ^ key_;
		}

	public:
		template <size_t... Is>
		constexpr XORSTR_INLINE basic_string(const _T* str, std::index_sequence<Is...>) :
			key_(random_char<K, _T>::value), encrypted_{ { enc(str[Is])... } } {}

		XORSTR_INLINE decltype(auto) decrypt()
		{
			for (size_t i = 0; i < N; ++i) {
				encrypted_[i] = dec(encrypted_[i]);
			}
			encrypted_[N] = static_cast<_T>(0);
			return (const _T*)encrypted_.data();
		}
	};

#undef XORSTR_INLINE

}  // namespace xorstr_impl

#define xorenc(s) (xorstr_impl::basic_string<decltype(s[0]), sizeof(s) - 1, \
  __COUNTER__>(s, std::make_index_sequence<sizeof(s) - 1>()))

#define xorstr(s) (xorenc(s).decrypt())

//namespace xorstr_impl {
//
//#ifdef _MSC_VER
//
//#define XORSTR_INLINE __forceinline
//#else
//#define XORSTR_INLINE inline
//#endif
//
//	constexpr auto time = (__DATE__ __TIME__);
//	constexpr auto seed = static_cast<int>(time[18]) +
//		static_cast<int>(time[6]) * 10 +
//		static_cast<int>(time[4]) * 60 +
//		static_cast<int>(time[3]) * 600 +
//		static_cast<int>(time[1]) * 3600 +
//		static_cast<int>(time[0]) * 36000;
//
//	// 1988, Stephen Park and Keith Miller
//	// "Random Number Generators: Good Ones Are Hard To Find", considered as "minimal standard"
//	// Park-Miller 31 bit pseudo-random number generator, implemented with G. Carta's optimisation:
//	// with 32-bit math and without division
//
//	template <int N>
//	struct random_generator {
//	private:
//		static constexpr unsigned a = 16807;  // 7^5
//		static constexpr unsigned m = 2147483647;  // 2^31 - 1
//		static constexpr unsigned s = random_generator<N - 1>::value;
//		static constexpr unsigned lo = a * (s & 0xFFFF);  // multiply lower 16 bits by 16807
//		static constexpr unsigned hi = a * (s >> 16);  // multiply higher 16 bits by 16807
//		static constexpr unsigned lo2 = lo + ((hi & 0x7FFF) << 16);  // combine lower 15 bits of hi with lo's upper bits
//		static constexpr unsigned hi2 = hi >> 15;  // discard lower 15 bits of hi
//		static constexpr unsigned lo3 = lo2 + hi;
//
//	public:
//		static constexpr unsigned max = m;
//		static constexpr unsigned value = lo3 > m ? lo3 - m : lo3;
//	};
//
//	template <>
//	struct random_generator<0> {
//		static constexpr unsigned value = seed;
//	};
//
//	template <int N, int M>
//	struct random_int {
//		static constexpr auto value = random_generator<N + 1>::value % M;
//	};
//
//	template <int N>
//	struct random_char {
//		static const char value = static_cast<char>(1 + random_int<N, 0x7F - 1>::value);
//	};
//
//	template <size_t N, int K>
//	struct string {
//	private:
//		const char key_;
//		std::array<char, N + 1> encrypted_;
//
//		constexpr char enc(char c) const {
//			return c ^ key_;
//		}
//
//		char dec(char c) const {
//			return c ^ key_;
//		}
//
//	public:
//		template <size_t... Is>
//		constexpr XORSTR_INLINE string(const char* str, std::index_sequence<Is...>) :
//			key_(random_char<K>::value), encrypted_{ { enc(str[Is])... } } {}
//
//		XORSTR_INLINE decltype(auto) decrypt() {
//			for (size_t i = 0; i < N; ++i) {
//				encrypted_[i] = dec(encrypted_[i]);
//			}
//			encrypted_[N] = '\0';
//			return encrypted_.data();
//		}
//	};
//
//#undef XORSTR_INLINE
//
//}  // namespace xorstr_impl
//
//#define xorstr(s) (xorstr_impl::string<sizeof(s) - 1, \
//  __COUNTER__>(s, std::make_index_sequence<sizeof(s) - 1>()).decrypt())


// https://www.unknowncheats.me/forum/counterstrike-global-offensive/209624-fast-easy-netvar-manager.html
// https://stackoverflow.com/questions/15858141/conveniently-declaring-compile-time-strings-in-c
// This is used to XOR the strings so der cant be easily found in a debugger

typedef uint32_t fnv_t;
class CFnvHash
{
	static constexpr fnv_t FNV_PRIME = 16777619u;
	static constexpr fnv_t OFFSET_BASIS = 2166136261u;

	template <unsigned int N>
	static constexpr fnv_t fnvHashConst(const char(&str)[N], unsigned int I = N)
	{
		return static_cast<fnv_t>(1ULL * (I == 1 ? (OFFSET_BASIS ^ str[0]) : (fnvHashConst(str, I - 1) ^ str[I - 1])) * FNV_PRIME);
	}

	static __forceinline fnv_t fnvHash(const char* str)
	{
		const auto length = strlen(str) + 1;
		auto hash = OFFSET_BASIS;
		for (std::size_t i = 0; i < length; ++i)
		{
			hash ^= *str++;
			hash *= FNV_PRIME;
		}
		return hash;
	}

	struct Wrapper
	{
		Wrapper(const char* str) : str(str) { }
		const char* str;
	};

	fnv_t hash_value;
public:
	CFnvHash(Wrapper wrapper) : hash_value(fnvHash(wrapper.str)) { }

	template <unsigned int N>
	constexpr CFnvHash(const char(&str)[N]) : hash_value(fnvHashConst(str)) { }

	constexpr operator fnv_t() const { return this->hash_value; }
};

namespace NXorStringFuncs
{
	constexpr static const unsigned seed = CFnvHash(__DATE__ __TIME__);

	constexpr unsigned LinearCongruentGenerator(int Rounds, unsigned Seed = seed)
	{
		return static_cast<unsigned>(1013904223ULL + 1664525ULL * (Rounds > 0 ? LinearCongruentGenerator(Rounds - 1) : Seed));
	}

	constexpr unsigned RandomRounds(int rounds)
	{
		return LinearCongruentGenerator(rounds);
	}

	constexpr unsigned Random()
	{
		return RandomRounds(10);
	}

	constexpr unsigned RandomNumber(int min, int max)
	{
		return min + Random() % (max - min + 1);
	}

	namespace variadic_toolbox
	{
		template <unsigned count,
			template<unsigned...> class meta_functor, unsigned... indices>
		struct apply_range
		{
			typedef typename apply_range<count - 1, meta_functor, count - 1, indices...>::result result;
		};

		template <template<unsigned...> class meta_functor, unsigned... indices>
		struct apply_range<0, meta_functor, indices...>
		{
			typedef typename meta_functor<indices...>::result result;
		};
	}

	namespace compile_time
	{
		constexpr unsigned char get_key(unsigned index, unsigned value)
		{
			return 0xFF & RandomRounds(10 + index + value);
		}

		constexpr unsigned short encrypt_string(const char* str, unsigned idx)
		{
			return static_cast<unsigned short>(0UL + (get_key(idx, str[idx]) << 8) + (str[idx] ^ get_key(idx, str[idx])));
		}

		template <unsigned short... str>
		struct string
		{
			static constexpr const unsigned short chars[sizeof...(str)] = { str... };
		};

		template <unsigned short... str>
		constexpr const unsigned short string<str...>::chars[sizeof...(str)];

		template <typename lambda_str_type>
		struct string_builder
		{
			template <unsigned... indices>
			struct produce
			{
				typedef string < encrypt_string(lambda_str_type{}.chars, indices)... > result;
			};
		};
	}
}

class CXString
{
public:
	constexpr CXString(std::size_t uiSize, const WORD* c_pwEncrypted) :
		m_uiSize((NXorStringFuncs::RandomRounds(10 + uiSize) << 16) + ((uiSize ^ NXorStringFuncs::RandomRounds(10 + uiSize)) & 0xFFFF)),
		m_pwEncrypted(c_pwEncrypted)
	{
	}

	void DecryptData(char* pDestData) const
	{
		WORD wSize = (m_uiSize & 0xFFFF) ^ (m_uiSize >> 16);

		for (std::size_t i = 0; i < wSize; ++i)
			pDestData[i] = (m_pwEncrypted[i] & 0xFF) ^ (m_pwEncrypted[i] >> 8);
	}

	std::string DecryptString() const
	{
		WORD wSize = (m_uiSize & 0xFFFF) ^ (m_uiSize >> 16);

		std::string szOutputString;
		szOutputString.resize(wSize);

		DecryptData(const_cast<char*>(szOutputString.data()));

		return szOutputString;
	}

private:
	std::size_t  m_uiSize;
	const WORD * m_pwEncrypted;
};


#define XOR(szInput)																					\
    CXString(sizeof(szInput), []{																		\
        struct  constexpr_string_type { const char * chars = szInput; };								\
        return  NXorStringFuncs::variadic_toolbox::apply_range<sizeof(szInput),							\
            NXorStringFuncs::compile_time::string_builder<constexpr_string_type>::produce>::result{};	\
    }().chars).DecryptString().c_str()


//junkstart

#define JUNK1(NAM1, NAM2, NAM3, V1, V2) double NAM1 = V1; \
	int NAM2 = V2; \
if (V2 != V2) {\
	int NAM3; \
for (NAM3 = 65; NAM3 > 0; NAM3--) {\
	continue; \
	} \
}

#define JUNK2(TYPE, NAME, v1, v2, v3, v4, v5, v6, v7, v8) TYPE NAME = v1; if (NAME == v2 && NAME == v4) NAME = v1 + v4 - (0xe2434b | 0x2313231 << 3) + 432131 - 0x213a4753 & 0b010101010101001 & 0b011101010101;if (NAME == v3 && NAME == v7) NAME = v6 ^ 0x9b5943 << 5 >> 8 ^ 0x1293014 >> 4 << 4 & 0xffff0000;if (NAME == v3 || NAME == v4) NAME = v2 << 5 >> 8 ^ 0x912391 >> 4 << 4 & 0xff00ff00;

#define JUNK_CODE_2 \
	__asm{push eax} \
	__asm{push edx} \
	__asm{xor edx, 0x90} \
	__asm{push edx} \
	__asm{sub edx, 0x80} \
	__asm{pop eax} \
	__asm{add eax, edx} \
	__asm{pop edx} \
	__asm{pop eax};

#define JUNK_CODE_ONE								\
		__asm {push eax}								\
		__asm {xor eax, eax}							\
		__asm {setpo al}								\
		__asm {push edx}								\
		__asm {xor edx, eax}							\
		__asm {sal edx, 2}								\
		__asm {xchg eax, edx}							\
		__asm {pop edx}									\
		__asm {or eax, ecx}								\
		__asm {pop eax}

// ..
#define JUNK_CODE_TWO_2(lineno, value)				\
		__asm {jz _1##lineno}							\
		__asm {jnz _1##lineno}							\
		__asm {_emit 0x##value}							\
		__asm {_1##lineno: }
#define JUNK_CODE_TWO_1(name, value) JUNK_CODE_TWO_2(name, value)
#define JUNK_CODE_TWO JUNK_CODE_TWO_1(__LINE__, __LINE__*1111%253)

// ..
#define JUNK_CODE_TWO_2_2(lineno)					\
		__asm {jz _112##lineno}							\
		__asm {jnz _112##lineno}						\
		__asm {_emit 0e8h}								\
		__asm {_112##lineno: }
#define JUNK_CODE_TWO_1_2(name) JUNK_CODE_TWO_2_2(name)
#define JUNK_CODE_TWO2 JUNK_CODE_TWO_1_2(__LINE__)

// ..
#define JUNK_CODE_TWO_2_3(lineno)					\
		__asm { xor eax, eax }							\
		__asm { test eax, eax }							\
		__asm {jz _1121##lineno}						\
		__asm {jnz _1120##lineno}						\
		__asm {_1120##lineno: }							\
		__asm {_emit 0e8h}								\
		__asm {_1121##lineno: }							\
		__asm { xor eax, 3 }							\
		__asm { add eax, 4 }							\
		__asm { xor eax, 5 }							
#define JUNK_CODE_TWO_1_3(name) JUNK_CODE_TWO_2_3(name)
#define JUNK_CODE_TWO3 JUNK_CODE_TWO_1_3(__LINE__)


// ..
#define JUNK_CODE_THREE_2(lineno, value1, value2)	\
		__asm {clc}										\
		__asm {jnb _3t##lineno}							\
		__asm {_emit 0x##value1}						\
		__asm {_emit 0x##value2}						\
		__asm {_3t##lineno: }
#define JUNK_CODE_THREE_1(name, value1, value2) JUNK_CODE_THREE_2(name, value1, value2)
#define JUNK_CODE_THREE JUNK_CODE_THREE_1(__LINE__, __LINE__*1222%253, __LINE__*1111%253)

// ..
#define JUNK_CODE_FOUR_2(lineno, value)				\
		__asm {jl _11f##lineno}							\
		__asm {_12f##lineno: }							\
		__asm {jmp _13f##lineno }						\
		__asm {_emit 0x##value }						\
		__asm {_11f##lineno: }							\
		__asm {jz _12f##lineno }						\
		__asm {_13f##lineno: }
#define JUNK_CODE_FOUR_1(name, value) JUNK_CODE_FOUR_2(name, value)
#define JUNK_CODE_FOUR JUNK_CODE_FOUR_1(__LINE__, __LINE__*1111%253)


// ..
#define JUNK_CODE_FIVE_2(lineno)					\
		__asm {pushf}									\
		__asm {push 0x0a}								\
		__asm {_51f##lineno: jnb _53f##lineno}			\
		__asm {jmp _52f##lineno}						\
		__asm {_52f##lineno: call _54f##lineno}			\
		__asm {_53f##lineno: jnb _52f##lineno}			\
		__asm {_54f##lineno: add esp,4}					\
		__asm {jmp _55f##lineno}						\
		__asm {_55f##lineno: }							\
		__asm {dec dword ptr [esp]}						\
		__asm {jno _56f##lineno}						\
		__asm {_56f##lineno: jns _51f##lineno}			\
		__asm {jp _57f##lineno}							\
		__asm {_57f##lineno: add esp,4}					\
		__asm {popf}									\
		__asm {jmp _58f##lineno}						\
		__asm {_58f##lineno: }
#define JUNK_CODE_FIVE_1(name) JUNK_CODE_FIVE_2(name)
#define JUNK_CODE_FIVE JUNK_CODE_FIVE_1(__LINE__)


#define Junk3 JUNK_CODE_FOUR JUNK_CODE_TWO3 JUNK_CODE_THREE JUNK_CODE_TWO JUNK_CODE_FIVE JUNK_CODE_2 JUNK_CODE_TWO2 JUNK_CODE_ONE
#define Junk4 JUNK_CODE_FIVE JUNK_CODE_THREE JUNK_CODE_2 JUNK_CODE_TWO JUNK_CODE_FOUR JUNK_CODE_ONE JUNK_CODE_TWO3 JUNK_CODE_TWO2


//junkend