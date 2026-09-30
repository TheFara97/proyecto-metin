#pragma once
#include <cstdint>
#include <utility>


#define CTV2_FORCEINLINE __forceinline

namespace ctv2
{
	CTV2_FORCEINLINE constexpr std::uint32_t hash(const char* str) noexcept
	{
		std::uint32_t h = 0x811C9DC5u;
		for (; *str; ++str)
			h = (h ^ static_cast<std::uint32_t>(*str)) * 0x01000193u;
		return h;
	}

	CTV2_FORCEINLINE constexpr std::uint32_t rotl32(std::uint32_t x, std::uint8_t n) noexcept
	{
		return (x << n) | (x >> (32 - n));
	}

	CTV2_FORCEINLINE constexpr std::uint32_t rotr32(std::uint32_t x, std::uint8_t n) noexcept
	{
		return (x >> n) | (x << (32 - n));
	}

	CTV2_FORCEINLINE constexpr std::uint32_t rng(std::uint32_t seed) noexcept
	{
		return rotl32(seed * 0x27BB2EE7u + 0xB504F32Du, 8);
	}

	CTV2_FORCEINLINE constexpr std::uint32_t op_count(std::uint32_t seed) noexcept
	{
		return (rng(seed + 0x1261u) % 3) + 6;
	}

	CTV2_FORCEINLINE constexpr std::uint32_t op_type(std::uint32_t seed, std::size_t idx) noexcept
	{
		return rng(seed + static_cast<std::uint32_t>(0x1FBBACu * idx)) % 6;
	}

	CTV2_FORCEINLINE constexpr std::uint32_t mod_inverse(std::uint32_t a) noexcept
	{
		std::uint32_t inv = a;
		for (int i = 0; i < 5; ++i)
			inv = inv * (2 - a * inv);
		return inv;
	}

	template<bool Decrypt, std::uint32_t Seed,
		std::size_t OpIdx, std::size_t OpCount>
	CTV2_FORCEINLINE constexpr std::uint32_t perform_op(std::uint32_t data) noexcept
	{
		constexpr auto Key = static_cast<std::uint32_t>(
			Decrypt ? (OpCount - OpIdx - 1) : OpIdx);
		constexpr auto Type = op_type(Seed, Key);

		if constexpr (Type == 0)
		{
			constexpr auto Val = rng(Seed + rotr32(Key * 0x87A8C2Fu, 9));
			if constexpr (Decrypt) data -= Val; else data += Val;
		}
		else if constexpr (Type == 1)
		{
			constexpr auto Val = rng(Seed + rotr32(Key * 0x6BBC2E7u, 11));
			if constexpr (Decrypt) data += Val; else data -= Val;
		}
		else if constexpr (Type == 2)
		{
			data ^= rng(Seed + rotr32(Key * 0x07C0EAFu, 13));
		}
		else if constexpr (Type == 3)
		{
			constexpr auto Val = rng(Seed + rotr32(Key * 0xC4345FBFu, 7)) | 1u;
			if constexpr (Decrypt) data *= mod_inverse(Val); else data *= Val;
		}
		else if constexpr (Type == 4)
		{
			constexpr auto Shift = static_cast<std::uint8_t>(
				(rng(Seed + rotr32(Key * 0xB2BDEC7u, 15)) % 28) + 1);
			if constexpr (Decrypt) data = rotl32(data, Shift); else data = rotr32(data, Shift);
		}
		else
		{
			constexpr auto Shift = static_cast<std::uint8_t>(
				(rng(Seed + rotr32(Key * 0x27BFB0Fu, 17)) % 28) + 1);
			if constexpr (Decrypt) data = rotr32(data, Shift); else data = rotl32(data, Shift);
		}

		return data;
	}

	template<bool Decrypt, std::uint32_t Seed, std::size_t... Ops>
	CTV2_FORCEINLINE constexpr std::uint32_t chain_impl(
		std::uint32_t data, std::index_sequence<Ops...>) noexcept
	{
		((data = perform_op<Decrypt, Seed, Ops, sizeof...(Ops)>(data)), ...);
		return data;
	}

	template<bool Decrypt, std::uint32_t Seed>
	CTV2_FORCEINLINE constexpr std::uint32_t transform(std::uint32_t data) noexcept
	{
		return chain_impl<Decrypt, Seed>(
			data, std::make_index_sequence<op_count(Seed)>{});
	}
}
