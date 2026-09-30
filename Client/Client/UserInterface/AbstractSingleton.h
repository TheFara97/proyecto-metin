#pragma once

#include <assert.h>
#include "../UserInterface/byteshield_client.hpp"

template <typename T>
class TAbstractSingleton
{
	static uintptr_t ms_singleton;
	static constexpr auto ms_singleton_hash = ByteShield::hash::type_hash_value<T>::value;

public:
	TAbstractSingleton()
	{
		assert(!ms_singleton);
		intptr_t offset = (intptr_t)(T*)1 - (intptr_t)(TAbstractSingleton <T>*)(T*)1;
		ms_singleton = ByteShield::EncryptDecryptPointer(((intptr_t)this + offset), ms_singleton_hash, false) ^ (ms_singleton_hash * 0xB4B82E39);
	}

	virtual ~TAbstractSingleton()
	{
		assert(ms_singleton);
		ms_singleton = 0;
	}

	__forceinline static T& GetSingleton()
	{
		assert(ms_singleton != NULL);
		return *(T*)(ByteShield::EncryptDecryptPointer(ms_singleton ^ (ms_singleton_hash * 0xB4B82E39), ms_singleton_hash, true));
	}
};

template <typename T> uintptr_t TAbstractSingleton <T>::ms_singleton = 0;
//martysama0134's ceqyqttoaf71vasf9t71218