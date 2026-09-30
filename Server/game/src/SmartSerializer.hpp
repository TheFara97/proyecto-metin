#pragma once
#include "desc.h"
#include <rapidjson/document.h>
#include <rapidxml/rapidxml_ext.hpp>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <nlohmann/json.hpp>

#include <boost/algorithm/string/join.hpp>
#include <type_traits>
#include <span>

namespace SmartSerializer
{
    inline constexpr char sSeparator = '|';
    inline constexpr char sDelimiter = '/';

    class PacketData {
    public:
        enum class DataType { RAW, JSON, XML, TOKEN };

        struct DataItem {
            std::vector<std::byte> data;
        };

        template<typename T>
        void AddRaw(const T& data) {
            AddRawImpl(&data, sizeof(T));
        }

        void AddJson(const nlohmann::json& json) {
            AddStringData(json.dump());
        }

        void AddXml(const rapidxml::xml_document<>& xml) {
            std::string xmlStr;
            rapidxml::print(std::back_inserter(xmlStr), xml, 0);
            AddStringData(xmlStr);
        }

        template<typename Container>
        void AddTokens(const Container& tokens) {
            AddStringData(boost::algorithm::join(tokens, std::string(1, sSeparator)));
        }

        const std::vector<DataItem>& GetItems() const { return items; }

    private:
        std::vector<DataItem> items;

        void AddRawImpl(const void* data, size_t size) {
            DataItem item;
            item.data.assign(static_cast<const std::byte*>(data), static_cast<const std::byte*>(data) + size);
            items.push_back(std::move(item));
        }

        void AddStringData(const std::string& str) {
            AddRawImpl(str.data(), str.size());
        }
    };

    class PacketBuilder
    {
    public:
        PacketBuilder(uint8_t header, uint8_t subheader)
        {
	        m_packet.SetPacket(header, subheader);
        }

        template<typename T>
        PacketBuilder& AddRaw(const T& data) {
            m_packetData.AddRaw(data);
            return *this;
        }

        PacketBuilder& AddJson(const nlohmann::json& json) {
            m_packetData.AddJson(json);
            return *this;
        }

        PacketBuilder& AddXml(const rapidxml::xml_document<>& xml) {
            m_packetData.AddXml(xml);
            return *this;
        }

        template<typename Container>
        PacketBuilder& AddTokens(const Container& tokens) {
            m_packetData.AddTokens(tokens);
            return *this;
        }

        std::pair<TDefaultPacket, PacketData> Build() &&
        {
            return {std::move(m_packet), std::move(m_packetData)};
        }

        void Send(LPDESC pDesc)
        {
            auto [packet, data] = std::move(*this).Build();

            packet.IncreaseSize(packet.GetFixedSize());

            TEMP_BUFFER buffer(1092);

            for (const auto& item : data.GetItems())
            {
			    packet.IncreaseSize(sizeof(uint16_t) + item.data.size());
                uint16_t size = static_cast<uint16_t>(item.data.size());
                buffer.write(&size, sizeof(size));
                buffer.write(item.data.data(), item.data.size());
            }

            pDesc->BufferedPacket(&packet, sizeof(packet));
            pDesc->Packet(buffer.read_peek(), buffer.size());
        }

    private:
        TDefaultPacket m_packet;
        PacketData m_packetData;
    };
}