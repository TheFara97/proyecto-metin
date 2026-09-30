#pragma once
#include <string>

struct TempOrOutputBuffer
{
	static const uint32_t kLocalSize = 8 * 1024;

	TempOrOutputBuffer(uint8_t* output): ptr(output), output(output)
	{
	}

	~TempOrOutputBuffer()
	{
		if (ptr != output && ptr != local)
			delete[] ptr;
	}

	void MakeTemporaryBuffer(std::size_t size)
	{
		if (size < sizeof(local))
			ptr = local;
		else
			ptr = new uint8_t[size];
	}

	uint8_t* ptr;
	uint8_t* output;
	uint8_t local[kLocalSize];
};