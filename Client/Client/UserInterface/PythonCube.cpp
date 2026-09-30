#include "stdafx.h"
#include "PythonCube.hpp"
#include "PythonNetworkStream.h"

bool PythonCube::ReceivePacket() const {
	cube::packets::Header headerPacket;
	if (!CPythonNetworkStream::Instance().Recv(sizeof(headerPacket),
											   &headerPacket))
		return false;

	switch (headerPacket.subheader) {
		case cube::packets::game::SUBHEADER_START:
			return ReceiveStartPacket();
		case cube::packets::game::SUBHEADER_INFO:
			return ReceiveInfoPacket();
		case cube::packets::game::SUBHEADER_CLOSE:
			return ReceiveClosePacket();
		default: 
			break;
	}

	return true;
}

bool PythonCube::ReceiveStartPacket() const {
	auto& netStream = CPythonNetworkStream::Instance();

	cube::packets::game::Start startPacket;
	if (!netStream.Recv(sizeof(startPacket), &startPacket))
		return false;

	auto recipeList = PyList_New(startPacket.recipeCount);;

	for (uint32_t i = 0; i < startPacket.recipeCount; ++i) {
		cube::packets::game::Recipe recipePacket;
		if (!netStream.Recv(sizeof(recipePacket), &recipePacket))
			return false;

		auto itemList = PyList_New(recipePacket.itemCount);

		for (uint16_t j = 0; j < recipePacket.itemCount; ++j) {
			cube::packets::game::Item itemPacket;
			if (!netStream.Recv(sizeof(itemPacket), &itemPacket))
				return false;

			PyList_SET_ITEM(itemList, j, Py_BuildValue("(II)",
													   itemPacket.vnum,
													   itemPacket.count));
		}

		PyList_SET_ITEM(recipeList, i, Py_BuildValue("((IB)OLLIII)",
													 recipePacket.reward.vnum,
													 recipePacket.reward.count,
													 itemList,
													 recipePacket.price,
													 recipePacket.priceCheque,
													 recipePacket.priceAchievement,
													 recipePacket.chance,
													 recipePacket.category));
	}

	netStream.CallGamePyFunction("OnCubeOpen", Py_BuildValue("(O)", recipeList));
	return true;
}

bool PythonCube::ReceiveInfoPacket() const {
	auto& netStream = CPythonNetworkStream::Instance();

	cube::packets::game::Info infoPacket;
	if (!netStream.Recv(sizeof(infoPacket), &infoPacket))
		return false;

	netStream.CallGamePyFunction("OnCubeUpdate", Py_BuildValue("(LLIB)",
															   infoPacket.price,
															   infoPacket.priceCheque,
															   infoPacket.priceAchievement,
															   infoPacket.chance));
	return true;
}

bool PythonCube::ReceiveClosePacket() const {
	CPythonNetworkStream::Instance().CallGamePyFunction("OnCubeClose");
	return true;
}

void PythonCube::SendMakePacket(uint8_t id, uint16_t count, TItemPos position) const {
	cube::packets::Header headerPacket;
	headerPacket.header = HEADER_CG_CUBE;
	headerPacket.size = sizeof(cube::packets::Header) +
		sizeof(cube::packets::client::Make);
	headerPacket.subheader = cube::packets::client::SUBHEADER_MAKE;

	cube::packets::client::Make makePacket;
	makePacket.id = id;
	makePacket.count = count;
	makePacket.position = std::move(position);

	CPythonNetworkStream::Instance().Send(sizeof(headerPacket), &headerPacket);
	CPythonNetworkStream::Instance().Send(sizeof(makePacket), &makePacket);
}

void PythonCube::SendClosePacket() const {
	cube::packets::Header headerPacket;
	headerPacket.header = HEADER_CG_CUBE;
	headerPacket.size = sizeof(cube::packets::Header);
	headerPacket.subheader = cube::packets::client::SUBHEADER_CLOSE;

	CPythonNetworkStream::Instance().Send(sizeof(headerPacket), &headerPacket);
}
