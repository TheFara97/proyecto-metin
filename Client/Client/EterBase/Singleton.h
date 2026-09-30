#ifndef __INC_ETERLIB_SINGLETON_H__
#define __INC_ETERLIB_SINGLETON_H__

#include <assert.h>
#include "../UserInterface/byteshield_client.hpp"


template <typename T>
class CSingleton
{
	static uintptr_t ms_singleton;
	static constexpr auto ms_singleton_hash = ByteShield::hash::type_hash_value<T>::value;

public:
	CSingleton()
	{
		assert(!ms_singleton);
		intptr_t offset = (intptr_t)(T*)1 - (intptr_t)(CSingleton <T>*)(T*)1;
		ms_singleton = ByteShield::EncryptDecryptPointer(((intptr_t)this + offset), ms_singleton_hash, false) ^ (ms_singleton_hash * 0xB4B82E39);
	}

	virtual ~CSingleton()
	{
		assert(ms_singleton);
		ms_singleton = 0;
	}

	__forceinline static T& Instance()
	{
		assert(ms_singleton);
		return *(T*)(ByteShield::EncryptDecryptPointer(ms_singleton ^ (ms_singleton_hash * 0xB4B82E39), ms_singleton_hash, true));
	}

	__forceinline static T* InstancePtr()
	{
		return (T*)(ByteShield::EncryptDecryptPointer(ms_singleton ^ (ms_singleton_hash * 0xB4B82E39), ms_singleton_hash, true));
	}

	__forceinline static T& instance()
	{
		assert(ms_singleton);
		return *(T*)(ByteShield::EncryptDecryptPointer(ms_singleton ^ (ms_singleton_hash * 0xB4B82E39), ms_singleton_hash, true));
	}
};

template <typename T> uintptr_t CSingleton <T>::ms_singleton = 0;

//
// singleton for non-hungarian
//
template <typename T>
class singleton
{
	static uintptr_t ms_singleton;
	static constexpr auto ms_singleton_hash = ByteShield::hash::type_hash_value<T>::value;

public:
	singleton()
	{
		assert(!ms_singleton);
		intptr_t offset = (intptr_t)(T*)1 - (intptr_t)(singleton <T>*)(T*)1;
		ms_singleton = ByteShield::EncryptDecryptPointer(((intptr_t)this + offset), ms_singleton_hash, false) ^ (ms_singleton_hash * 0xB4B82E39);
	}

	virtual ~singleton()
	{
		assert(ms_singleton);
		ms_singleton = 0;
	}

	__forceinline static T& Instance()
	{
		assert(ms_singleton);
		return *(T*)(ByteShield::EncryptDecryptPointer(ms_singleton ^ (ms_singleton_hash * 0xB4B82E39), ms_singleton_hash, true));
	}

	__forceinline static T* InstancePtr()
	{
		return (T*)(ByteShield::EncryptDecryptPointer(ms_singleton ^ (ms_singleton_hash * 0xB4B82E39), ms_singleton_hash, true));
	}

	__forceinline static T& instance()
	{
		assert(ms_singleton);
		return *(T*)(ByteShield::EncryptDecryptPointer(ms_singleton ^ (ms_singleton_hash * 0xB4B82E39), ms_singleton_hash, true));
	}
};

template <typename T> uintptr_t singleton <T>::ms_singleton = 0;

#endif
//martysama0134's ceqyqttoaf71vasf9t71218