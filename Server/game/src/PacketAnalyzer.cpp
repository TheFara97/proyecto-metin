#include "stdafx.h"
#include "PacketAnalyzer.hpp"

namespace cheat {
	const uint32_t PacketAnalyzer::LAST_HEADER_BUFFER_SIZE = 3;

	void PacketAnalyzer::Push(header_t id) {
		buffer_.push_back(id);

		if (buffer_.size() > PacketAnalyzer::LAST_HEADER_BUFFER_SIZE)
			buffer_.erase(std::begin(buffer_));
	}

	PacketAnalyzer::header_t PacketAnalyzer::Pop() {
		if (buffer_.empty())
			return 0;

		auto ret = buffer_.front();
		buffer_.erase(std::begin(buffer_));

		return ret;
	}

	bool PacketAnalyzer::Test(std::initializer_list<header_t> entries) const {
		if (entries.size() > buffer_.size())
			return false;

		auto i = buffer_.size() - 1;
		for (auto&& e : entries) {
			if (e != buffer_.at(i--))
				return false;
		}

		return true;
	}
}  // namespace cheat
