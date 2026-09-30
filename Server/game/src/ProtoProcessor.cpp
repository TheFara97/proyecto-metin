#include "stdafx.h"
#ifdef __REMOTE_PROTOS__
#include "char.h"
#include "item_manager.h"
#include "mob_manager.h"
#include "desc.h"
#include "packet.h"
#include <zstd/zstd.h>
#include "ProtoProcessor.h"

#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

namespace ProtoProcessor
{
	std::map<EProtos, std::pair<DWORD, std::function<PProtoStruct()>>> m_proto_config = {
		{EProtos::ITEM_PROTO, {MAKEFOURCC('M', 'I', 'P', 'X'), []() -> PProtoStruct { return ITEM_MANAGER::instance().GetItemProto(); }}},
		{EProtos::MOB_PROTO, {MAKEFOURCC('M', 'I', 'P', 'T'), []() -> PProtoStruct { return CMobManager::instance().GetMobProto(); }}},
	};
	const WORD wChunkSize = 8192;

	ZSTDObject::ZSTDObject(SProtoVec& vProto) : objectData{nullptr}, dataSize{0}
	{
		dataSize = sizeof(vProto[0]) * vProto.size();
		objectData = new BYTE[dataSize]{};
		std::memcpy(objectData, &vProto[0], dataSize);
	}

	ZSTDObject::~ZSTDObject()
	{
		Clean();
	}

	bool ZSTDObject::CompressData(int iCompressionLevel)
	{
		if (!dataSize)
		{
			sys_err("Compresion data size is set to 0!");
			return false;
		}

		// Estimating compression bound
		const auto cBuffSize = ZSTD_compressBound(dataSize);

		// Creating buffer to contain compressed data
		BYTE* compressBuffer = new BYTE[cBuffSize]{};

		// Compressing
		const auto compressSize = ZSTD_compress((void*) compressBuffer, cBuffSize, objectData, dataSize, iCompressionLevel);
		if (!compressSize)
		{
			delete[] compressBuffer;
			return false;
		}

		// Swap pointers
		Clean();
		objectData = compressBuffer;
		dataSize = compressSize;
		return true;
	}

	bool ZSTDObject::DecompressData()
	{
		if (!dataSize)
		{
			sys_err("Decompression data size is set to 0!");
			return false;
		}

		// Getting decompressed data size by a header
		const auto cBuffSize = ZSTD_getFrameContentSize(objectData, dataSize);
		if (cBuffSize == ZSTD_CONTENTSIZE_ERROR || cBuffSize == ZSTD_CONTENTSIZE_UNKNOWN)
			return false;

		// Creating buffer to contain decompressed data
		BYTE* decompressBuffer = new BYTE[cBuffSize]{};

		// Decompressing
		const auto decompressSize = ZSTD_decompress((void *) decompressBuffer, cBuffSize, objectData, dataSize);
		if (!decompressSize || decompressSize == dataSize)
		{
			delete[] decompressBuffer;
			return false;
		}

		// Swap pointers
		Clean();
		objectData = decompressBuffer;
		dataSize = decompressSize;
		return true;
	}

	void ZSTDObject::Clean()
	{
		if (objectData)
		{
			delete[] objectData;
			objectData = nullptr;
		}

		dataSize = 0;
	}

	bool CompressProto(SProtoVec & vProto)
	{
		// Compress object
		ZSTDObject cmprObject{ vProto };
		bool bRes = cmprObject.CompressData();

		// If compression is successfull, update reference object
		if (bRes)
		{
			vProto.resize(cmprObject.GetSize());
			std::memcpy(&vProto[0], cmprObject.GetData(), cmprObject.GetSize());
		}

		return bRes;
	}

	bool DecompressProto(SProtoVec& vProto)
	{
		// Decompression object
		ZSTDObject cmprObject{ vProto };
		bool bRes = cmprObject.DecompressData();

		// If decompression is successfull, update reference object
		if (bRes)
		{
			vProto.resize(cmprObject.GetSize());
			std::memcpy(&vProto[0], cmprObject.GetData(), cmprObject.GetSize());
		}

		return bRes;
	}

	void SendProtosToPlayer(LPDESC pDesc)
	{
		if (!pDesc)
			return;

		TPacketGCRemoteProto packet{};
		packet.bHeader = HEADER_GC_SEND_PROTO;

		for (const auto& [key, value] : m_proto_config)
		{
			// Get proto
			auto [vProto, stLength] = value.second();

			// Compress it
			if (!CompressProto(vProto))
			{
				sys_err("Compression is unsuccessfull!");
				continue;
			}

			// Send frame
			packet.eType = key;
			packet.eSubHeader = ERemoteProtosSubHeaders::HEADER;
			packet.wSize = sizeof(packet);
			packet.dwCount = stLength;
			packet.dwFourCC = value.first;
			packet.dwRealSize = vProto.size();
			pDesc->Packet(&packet, sizeof(packet));

			// Now send data, chunk by chunk
			auto pCurPos = vProto.data();
			packet.eSubHeader = ERemoteProtosSubHeaders::CHUNK;

			while (packet.dwRealSize > 0)
			{
				// Calculate how much bytes we need to send and how much is still left
				WORD wSize = std::min<size_t>(wChunkSize, packet.dwRealSize);
				packet.wSize = wSize + sizeof(packet);

				// Send buffered packet
				pDesc->BufferedPacket(&packet, sizeof(packet));
				pDesc->LargePacket(pCurPos, wSize);

				// Move further pointer and update size
				pCurPos += wSize;
				packet.dwRealSize -= wSize;
			}
		}
	}
}
#endif

