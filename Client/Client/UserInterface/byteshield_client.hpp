#pragma once
#include <cinttypes>
#include <cstddef>

#define AC_HANDLERS_COUNT 32

#if defined(_MSC_VER)
#define AC_FORCEINLINE __forceinline
#define ACEXPORT __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ > 3
#define AC_FORCEINLINE inline __attribute__((__always_inline__))
#ifdef ARM
#define ACEXPORT     __attribute__((externally_visible,visibility("default")))
#else
#define ACEXPORT     __attribute__((visibility("default")))
#endif
#else
#define AC_FORCEINLINE inline
#define ACEXPORT
#endif

namespace ByteShield
{
	namespace hash
	{
		struct const_str
		{
			const char* data;
			unsigned long size;

			constexpr const_str(const char* d, unsigned long s)
				: data(d), size(s) {
			}
		};

		constexpr unsigned long cstrlen(const char* str)
		{
			return *str ? 1 + cstrlen(str + 1) : 0;
		}

		template<typename T>
		constexpr const_str type_name()
		{
#ifdef __clang__
			return const_str(__PRETTY_FUNCTION__ + 31, cstrlen(__PRETTY_FUNCTION__) - 31 - 1);
#elif defined(__GNUC__)
			return const_str(__PRETTY_FUNCTION__ + 46, cstrlen(__PRETTY_FUNCTION__) - 46 - 1);
#elif defined(_MSC_VER)
			return const_str(__FUNCSIG__ + 38, cstrlen(__FUNCSIG__) - 38 - 7);
#else
			return const_str("unsupported compiler", 20);
#endif
		}

		constexpr std::size_t fnv1a_append_impl(std::size_t hash, const char* str, unsigned long size, unsigned long idx = 0)
		{
			return idx == size
				? hash
				: fnv1a_append_impl(
					(hash ^ static_cast<unsigned char>(str[idx])) *
					(sizeof(std::size_t) == 8 ? 1099511628211ull : 16777619u),
					str,
					size,
					idx + 1
				);
		}

		constexpr std::size_t fnv1a_append(std::size_t hash, const_str str)
		{
			return fnv1a_append_impl(hash, str.data, str.size);
		}

		constexpr std::size_t make_offset()
		{
			return sizeof(std::size_t) == 8
				? 14695981039346656037ull
				: 2166136261u;
		}

		template<typename T>
		constexpr std::size_t type_hash()
		{
			return fnv1a_append(
				fnv1a_append(make_offset(), const_str(__DATE__, sizeof(__DATE__) - 1)),
				type_name<T>()
			);
		}

		template<typename T>
		struct type_hash_value
		{
			static constexpr std::size_t value = type_hash<T>();
		};

		template<typename T>
		constexpr std::size_t type_hash_value<T>::value;
	}

	enum class handler_index
	{
		Initialize = 1,
		GetToken,
		EncryptionExchangeKeys,
		EncryptionDecrypt,
		EncryptionEncrypt,
		DestroyByteShieldSession,
		EncryptDecryptPointer = 9
	};

#pragma pack(push, 1)
	struct BSEncryptionContext
	{
		std::uint8_t pub[64];

		std::uint8_t input_key[16];
		std::uint8_t output_key[16];

		std::uint8_t input_key_cursor;
		std::uint8_t output_key_cursor;

		bool initialized = false;
	};
#pragma pack(pop)

	extern "C" ACEXPORT const uint8_t BYTESHIELD_HANDLERS[];

	AC_FORCEINLINE bool Initialize(const char* token)
	{
		const auto addr = *(uintptr_t*)(ByteShield::BYTESHIELD_HANDLERS + (static_cast<uintptr_t>(ByteShield::handler_index::Initialize) * sizeof(uintptr_t)));
		if (!addr) return false;

		return reinterpret_cast<bool(__cdecl*)(const char*)>(addr)(token);
	}

	AC_FORCEINLINE bool GetToken(char* buffer, std::size_t* size)
	{
		const auto addr = *(uintptr_t*)(ByteShield::BYTESHIELD_HANDLERS + (static_cast<uintptr_t>(ByteShield::handler_index::GetToken) * sizeof(uintptr_t)));
		if (!addr) return false;

		return reinterpret_cast<bool(__cdecl*)(char*, std::size_t*)>(addr)(buffer, size);
	}

	AC_FORCEINLINE bool EncryptionExchangeKeys(BSEncryptionContext* context)
	{
		const auto addr = *(uintptr_t*)(ByteShield::BYTESHIELD_HANDLERS + (static_cast<uintptr_t>(ByteShield::handler_index::EncryptionExchangeKeys) * sizeof(uintptr_t)));
		if (!addr) return false;

		return reinterpret_cast<bool(__cdecl*)(BSEncryptionContext*)>(addr)(context);
	}

	AC_FORCEINLINE bool EncryptionDecrypt(BSEncryptionContext* context, std::uint8_t* buffer, std::size_t size)
	{
		const auto addr = *(uintptr_t*)(ByteShield::BYTESHIELD_HANDLERS + (static_cast<uintptr_t>(ByteShield::handler_index::EncryptionDecrypt) * sizeof(uintptr_t)));
		if (!addr) return false;

		return reinterpret_cast<bool(__cdecl*)(BSEncryptionContext*, std::uint8_t*, std::size_t)>(addr)(context, buffer, size);
	}

	AC_FORCEINLINE bool EncryptionEncrypt(BSEncryptionContext* context, std::uint8_t* buffer, std::size_t size)
	{
		const auto addr = *(uintptr_t*)(ByteShield::BYTESHIELD_HANDLERS + (static_cast<uintptr_t>(ByteShield::handler_index::EncryptionEncrypt) * sizeof(uintptr_t)));
		if (!addr) return false;

		return reinterpret_cast<bool(__cdecl*)(BSEncryptionContext*, std::uint8_t*, std::size_t)>(addr)(context, buffer, size);
	}

	AC_FORCEINLINE bool DestroyByteShieldSession()
	{
		const auto addr = *(uintptr_t*)(ByteShield::BYTESHIELD_HANDLERS + (static_cast<uintptr_t>(ByteShield::handler_index::DestroyByteShieldSession) * sizeof(uintptr_t)));
		if (!addr) return false;

		return reinterpret_cast<bool(__cdecl*)()>(addr)();
	}

	AC_FORCEINLINE std::uintptr_t EncryptDecryptPointer(std::uintptr_t ptr, std::size_t seed, bool decrypt)
	{
		const auto addr = *(uintptr_t*)(ByteShield::BYTESHIELD_HANDLERS + (static_cast<uintptr_t>(ByteShield::handler_index::EncryptDecryptPointer) * sizeof(uintptr_t)));
		if (!addr) return ptr;

		return reinterpret_cast<std::uintptr_t(__cdecl*)(std::uintptr_t, std::size_t, bool)>(addr)(ptr, seed, decrypt);
	}

	template <typename T, std::uintptr_t Seed = 0>
	struct EncryptedProp
	{
	private:
		static constexpr auto enc_prop_hash = ByteShield::hash::type_hash_value<T>::value + Seed;

		template<std::size_t Size>
		static inline constexpr std::size_t _buffer_size()
		{
			return ((Size / sizeof(std::uintptr_t)) + (Size % sizeof(std::uintptr_t) != 0));
		}
	public:
		AC_FORCEINLINE void set_value(const T& value) noexcept
		{
			memset(transformed_data, 0, sizeof(transformed_data));
			memcpy(transformed_data, &value, sizeof(T));

			perform_crypto_transform(transformed_data, false);
		}

		AC_FORCEINLINE T get_value() const noexcept
		{
			std::uintptr_t value_buffer[_buffer_size<sizeof(T)>()];
			memcpy(value_buffer, transformed_data, sizeof(transformed_data));

			perform_crypto_transform(value_buffer, true);

			return *reinterpret_cast<T*>(value_buffer);
		}

		AC_FORCEINLINE operator T() const noexcept
		{
			return get_value();
		}

		AC_FORCEINLINE EncryptedProp()
		{

		}

		AC_FORCEINLINE EncryptedProp(const T& value)
		{
			set_value(value);
		}
	protected:
		AC_FORCEINLINE constexpr void perform_crypto_transform(std::uintptr_t* data, bool IsDecrypt)  const
		{
			for (auto i = 0u; i < _buffer_size<sizeof(T)>(); i++)
			{
				if (IsDecrypt)
				{
					data[i] = EncryptDecryptPointer(data[i], enc_prop_hash, true);
					data[i] ^= (enc_prop_hash * 0xA4B82E39);
				}
				else
				{
					data[i] ^= (enc_prop_hash * 0xA4B82E39);
					data[i] = EncryptDecryptPointer(data[i], enc_prop_hash, false);
				}
			}
		}
		std::uintptr_t transformed_data[_buffer_size<sizeof(T)>()];
	};
};