#pragma once

namespace cheat {
	class PacketAnalyzer {
		public:
			static const uint32_t LAST_HEADER_BUFFER_SIZE;

			using header_t = uint8_t;

		public:
			PacketAnalyzer() = default;
			virtual ~PacketAnalyzer() = default;

			void Push(header_t id);
			header_t Pop();

			bool Test(std::initializer_list<header_t> entries) const;

		protected:
			std::vector<header_t> buffer_;
	};
}  // namespace cheat

