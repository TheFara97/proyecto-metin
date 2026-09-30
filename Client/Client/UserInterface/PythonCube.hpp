#pragma once

namespace cube {

namespace packets {

#pragma pack(1)
struct Header {
	uint8_t header;
	WORD size;
	uint8_t subheader;
};
#pragma pack()

namespace game {

enum {
	SUBHEADER_START,
	SUBHEADER_INFO,
	SUBHEADER_CLOSE
};

#pragma pack(1)
struct Item {
	uint32_t vnum;
	uint16_t count;
};

struct Recipe {
	uint16_t itemCount;
	Item reward;
	int64_t price;
	int64_t priceCheque;
	int32_t  priceAchievement;
	uint8_t chance;
	uint8_t category;
};

struct Start {
	uint32_t recipeCount;
};

struct Info {
	int64_t price;
    int64_t priceCheque;
	int32_t priceAchievement;
	uint8_t chance;
};
#pragma pack()

};

namespace client {

enum {
	SUBHEADER_MAKE,
	SUBHEADER_CLOSE
};

#pragma pack(1)
struct Make {
	uint8_t id;
	uint16_t count;
	TItemPos position;
};

#pragma pack()

};

};

};

class PythonCube
	: public singleton<PythonCube> {
public:
	bool ReceivePacket() const;

protected:
	bool ReceiveStartPacket() const;
	bool ReceiveInfoPacket() const;
	bool ReceiveClosePacket() const;

public:
	void SendMakePacket(uint8_t id, uint16_t count, TItemPos position) const;
	void SendClosePacket() const;
};
