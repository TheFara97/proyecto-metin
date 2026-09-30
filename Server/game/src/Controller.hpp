#pragma once

#include "PacketAnalyzer.hpp"

class CHARACTER;

namespace cheat {
	class Controller {
		public:
			Controller(CHARACTER& ch);
			virtual ~Controller() = default;

			void PacketPush(PacketAnalyzer::header_t id);

			void AnalyzePackets();

			void ReceiveAttackPacket();
			void ReceiveMovePacket(uint32_t clientTime);

		protected:
			CHARACTER& ch_;

			PacketAnalyzer packetAnalyzer_;
			uint32_t packetPunisher_;

			uint32_t rangeWaitDamageDetectionCount_;
			std::vector<uint32_t> moveTimes_;

		public:
			static void SetDebug(bool isDebug) { isDebug_ = isDebug; }
			static bool IsDebug() { return isDebug_; }

		protected:
			static bool isDebug_;
	};

	template <typename T>
	const T getMedian(std::vector<T>& values, size_t vectorSize) {
		T median = 0;

		if (vectorSize > 0) {
			//Sort the vector
			std::sort(values.begin(), values.end());

			//Extract the median
			if (vectorSize % 2 == 0) {
				median = (values[vectorSize / 2 - 1] + values[vectorSize / 2]) / 2;
			} else {
				median = values[(size_t)std::floor(vectorSize / 2)];
			}
		}

		return median;
	}
};  // namespace cheat

