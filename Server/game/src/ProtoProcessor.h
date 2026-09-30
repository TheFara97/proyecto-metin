#pragma once
#ifdef __REMOTE_PROTOS__
#include "packet.h"

namespace ProtoProcessor
{
	using SProtoVec = std::vector<BYTE>;
	using PProtoStruct = std::pair<SProtoVec, SProtoVec::size_type>;
	extern std::map<EProtos, std::pair<DWORD, std::function<PProtoStruct()>>> m_proto_config;

	// Compression class
	class ZSTDObject
	{
		public:
			ZSTDObject(SProtoVec& vProto);
			~ZSTDObject();

		public:
			bool CompressData(int iCompressionLevel = 1);
			bool DecompressData();
			const void* GetData() { return (void*) objectData; }
			const size_t GetSize() { return dataSize; }

		private:
			void Clean();

		private:
			BYTE * objectData;
			size_t dataSize;
	};

	bool CompressProto(SProtoVec & vProto);
	bool DecompressProto(SProtoVec & vProto);
	void SendProtosToPlayer(LPDESC pDesc);
}
#endif

