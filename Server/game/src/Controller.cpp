#include "stdafx.h"
#include "Controller.hpp"
#include "char.h"
#include "desc.h"
#include "log.h"

#include <algorithm>

namespace cheat {
	bool Controller::isDebug_ = false;

	Controller::Controller(CHARACTER& ch)
		: ch_(ch), packetPunisher_(0), rangeWaitDamageDetectionCount_(0) {}

	void Controller::PacketPush(PacketAnalyzer::header_t id) {
		packetAnalyzer_.Push(id);
	}

	void Controller::AnalyzePackets() {
		if (packetAnalyzer_.Test({HEADER_CG_ITEM_PICKUP,
								  HEADER_CG_MOVE,
								  HEADER_CG_MOVE}))
			++packetPunisher_;

		if (packetPunisher_ > 5) {
			LogManager::Instance().HackLog("M2BOB", &ch_);
			packetPunisher_ = 0;
		}
	}

	void Controller::ReceiveAttackPacket() {
		auto count = moveTimes_.size();

		if (count >= 25) {
			std::sort(std::begin(moveTimes_), std::end(moveTimes_));

			decltype(moveTimes_)::value_type sum = 0;

			auto low = moveTimes_.front();
			std::for_each(std::begin(moveTimes_), std::end(moveTimes_),
						  [&](const uint32_t& value) { sum += (value - low); });

			auto median = sum / count;

			if (IsDebug())
				ch_.ChatPacket(CHAT_TYPE_INFO,
							   "<DEBUG:cheat::Controller> median = %u", median);

			if (median < 750) {
				++rangeWaitDamageDetectionCount_;

				if (rangeWaitDamageDetectionCount_ > 3) {
					LogManager::Instance().HackLog("RANGE_WAIT_DAMAGE", &ch_);

					if (ch_.GetDesc())
						ch_.GetDesc()->DelayedDisconnect(0);
				}
			} else {
				rangeWaitDamageDetectionCount_ = 0;
			}

			moveTimes_.clear();
		}
	}

	void Controller::ReceiveMovePacket(uint32_t clientTime) {
		moveTimes_.emplace_back(clientTime);
	}
}  // namespace cheat

