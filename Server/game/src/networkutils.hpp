#ifndef NETWORKUTILS_HPP
#define NETWORKUTILS_HPP

#include "MemoryBuffer.h"
#include "char.h"
#include "desc.h"
#include "packet.h"
#include <glaze/glaze.hpp>
#include <type_traits>
#include <span>
#include <string>
#include <string_view>
#include <optional>
#include <vector>
#include <memory>
#include <utility>

namespace networkutils
{
/**
 * @brief Concept for packet headers requiring header and sub_header members
 */
template <typename T>
concept HeaderPacketConcept = requires(T t) {
    { t.header } -> std::convertible_to<uint8_t>;
    { t.sub_header } -> std::convertible_to<uint8_t>;
};

/**
 * @brief Concept for types that can be serialized by glaze
 */
template <typename T>
concept GlazeSerializable = requires(T t, std::string& str) {
    { glz::write_json(t, str) } -> std::convertible_to<bool>;
};

/**
 * @brief Default dynamic packet header structure
 */
struct DynamicPacketInfo {
    uint8_t header;
    uint8_t sub_header;

    struct glaze {
        using T = DynamicPacketInfo;
        static constexpr auto value = glz::object(
            "header", &T::header,
            "sub_header", &T::sub_header
        );
    };
};

/**
 * @brief Dynamic packet builder with fluent interface
 * 
 * @tparam HeaderPacket The packet header type (must satisfy HeaderPacketConcept)
 */
template <typename HeaderPacket = DynamicPacketInfo>
    requires HeaderPacketConcept<HeaderPacket>
class DynamicPacketBuilder
{
public:
    /**
     * @brief Construct a new Dynamic Packet Builder
     * 
     * @param initial_buffer_size Initial buffer size in bytes (default: 1024)
     */
    explicit DynamicPacketBuilder(size_t initial_buffer_size = 1024) 
        : m_buffer(initial_buffer_size)
    {
        m_buffer.SetFlags(net::MemoryBuffer::BUFFER_GROW);
    }

    /**
     * @brief Add packet header
     * 
     * @tparam T Type of sub_header (will be converted to uint8_t)
     * @param header Main packet header
     * @param sub_header Sub header for packet type
     * @return Reference to this builder for method chaining
     */
    template <typename T>
    DynamicPacketBuilder& add_header(const uint8_t header, const T sub_header)
    {
        m_header_packet.header = header;
        m_header_packet.sub_header = static_cast<uint8_t>(sub_header);

        m_buffer.Write(&m_header_packet, sizeof(m_header_packet));

        return *this;
    }

    /**
     * @brief Add raw payload data
     * 
     * @tparam T Type of payload
     * @param payload Data to add
     * @return Reference to this builder for method chaining
     */
    template <typename T>
    DynamicPacketBuilder& add_payload(const T& payload)
    {
        m_buffer.Write(&payload, sizeof(T));
        return *this;
    }

    /**
     * @brief Add serialized payload using glaze
     * 
     * @tparam T Type of payload (must be glaze serializable)
     * @param payload Data to serialize and add
     * @return Reference to this builder for method chaining
     */
    template <typename T>
    requires GlazeSerializable<T>
    DynamicPacketBuilder& add_serialized_payload(const T& payload)
    {
        // Serialize data to JSON
        std::string json_data;
        if (!glz::write_json(payload, json_data)) {
            // Log serialization error
            sys_err("Failed to serialize payload data");
            return *this;
        }
        
        // Write data size
        uint32_t size = static_cast<uint32_t>(json_data.size());
        m_buffer.Write(&size, sizeof(size));
        
        // Write JSON data
        m_buffer.Write(json_data.data(), json_data.size());
        
        return *this;
    }

    /**
     * @brief Add vector payload data
     * 
     * @tparam T Element type
     * @param vec Vector of elements to add
     * @return Reference to this builder for method chaining
     */
    template <typename T>
    DynamicPacketBuilder& add_vector_payload(const std::vector<T>& vec)
    {
        // Write vector size
        uint32_t size = static_cast<uint32_t>(vec.size());
        m_buffer.Write(&size, sizeof(size));
        
        // Write vector elements
        if (!vec.empty()) {
            m_buffer.Write(vec.data(), vec.size() * sizeof(T));
        }
        
        return *this;
    }

    /**
     * @brief Add string payload
     * 
     * @param str String to add
     * @return Reference to this builder for method chaining
     */
    DynamicPacketBuilder& add_string_payload(std::string_view str)
    {
        // Write string size
        uint32_t size = static_cast<uint32_t>(str.size());
        m_buffer.Write(&size, sizeof(size));
        
        // Write string data
        if (!str.empty()) {
            m_buffer.Write(str.data(), str.size());
        }
        
        return *this;
    }

    /**
     * @brief Send the packet to a client
     * 
     * @param p_character Character to send the packet to
     */
    void send_to_client(const LPCHARACTER p_character)
    {
        if (!p_character) {
            sys_err("Cannot send packet to null character");
            return;
        }
        
        const auto p_desc = p_character->GetDesc();
        if (!p_desc) {
            sys_err("Character %s has no descriptor", p_character->GetName());
            return;
        }

        p_desc->Packet(m_buffer.GetReadPointer(), m_buffer.GetSpace());

        // Clear buffer after sending
        clear_buffer();
    }

    /**
     * @brief Clear the packet buffer
     */
    void clear_buffer()
    {
        m_buffer.Truncate();
    }

    /**
     * @brief Get current buffer size
     * 
     * @return Size in bytes
     */
    [[nodiscard]] std::size_t size() 
    {
        return m_buffer.GetSpace();
    }

    /**
     * @brief Get buffer data pointer and size
     * 
     * @return Pair of (data pointer, size)
     */
    [[nodiscard]] std::pair<const char*, size_t> get_data()
    {
        return {m_buffer.GetReadPointer(), m_buffer.GetSpace()};
    }

private:
    HeaderPacket m_header_packet{};
    net::MemoryBuffer m_buffer;
};

/**
 * @brief Dynamic packet reader for parsing received packets
 */
class DynamicPacketReader
{
public:
    /**
     * @brief Construct a new reader from raw data
     * 
     * @param data Pointer to packet data
     * @param size Size of packet data
     */
    DynamicPacketReader(const void* data, size_t size) 
        : m_buffer(size)
    {
        m_buffer.Write(data, size);
    }

    /**
     * @brief Read packet header
     * 
     * @tparam T Header type
     * @param header Output header
     * @return true if successful
     */
    template <typename T>
    [[nodiscard]] bool read_header(T& header)
    {
        return m_buffer.Read(reinterpret_cast<char*>(&header), sizeof(T));
    }

    /**
     * @brief Read raw payload data
     * 
     * @tparam T Payload type
     * @param payload Output payload
     * @return true if successful
     */
    template <typename T>
    [[nodiscard]] bool read_payload(T& payload)
    {
        return m_buffer.Read(reinterpret_cast<char*>(&payload), sizeof(T));
    }

    /**
     * @brief Read serialized payload
     * 
     * @tparam T Payload type (must be glaze serializable)
     * @param payload Output payload
     * @return true if successful
     */
    template <typename T>
    requires GlazeSerializable<T>
    [[nodiscard]] bool read_serialized_payload(T& payload)
    {
        // Read data size
        uint32_t size = 0;
        if (!m_buffer.Read(reinterpret_cast<char*>(&size), sizeof(size))) {
            return false;
        }
        
        // Validate size
        if (size == 0 || size > 10 * 1024 * 1024) { // Max 10MB for safety
            sys_err("Invalid serialized data size: %u", size);
            return false;
        }
        
        // Check if we have enough data
        if (m_buffer.GetSpace() < size) {
            sys_err("Not enough data to read serialized payload: need %u, have %zu", 
                    size, m_buffer.GetSpace());
            return false;
        }
        
        // Read JSON data
        std::string json_data(size, '\0');
        if (!m_buffer.Read(&json_data[0], size)) {
            sys_err("Failed to read serialized data");
            return false;
        }
        
        // Deserialize JSON to payload
        auto result = glz::read_json(payload, json_data);
        if (result.has_error()) {
            sys_err("Failed to deserialize data: %s", result.error().c_str());
            return false;
        }
        
        return true;
    }

    /**
     * @brief Read vector data
     * 
     * @tparam T Element type
     * @param vec Output vector
     * @return true if successful
     */
    template <typename T>
    [[nodiscard]] bool read_vector(std::vector<T>& vec)
    {
        // Read vector size
        uint32_t size = 0;
        if (!m_buffer.Read(reinterpret_cast<char*>(&size), sizeof(size))) {
            return false;
        }
        
        // Validate size
        if (size > 1000000) { // Sanity check
            sys_err("Vector size too large: %u", size);
            return false;
        }
        
        // Prepare vector
        vec.resize(size);
        
        // Read vector data
        if (size > 0) {
            if (!m_buffer.Read(reinterpret_cast<char*>(vec.data()), size * sizeof(T))) {
                return false;
            }
        }
        
        return true;
    }

    /**
     * @brief Read string data
     * 
     * @param str Output string
     * @return true if successful
     */
    [[nodiscard]] bool read_string(std::string& str)
    {
        // Read string size
        uint32_t size = 0;
        if (!m_buffer.Read(reinterpret_cast<char*>(&size), sizeof(size))) {
            return false;
        }
        
        // Validate size
        if (size > 1024 * 1024) { // Max 1MB for safety
            sys_err("String size too large: %u", size);
            return false;
        }
        
        // Prepare string
        str.resize(size);
        
        // Read string data
        if (size > 0) {
            if (!m_buffer.Read(&str[0], size)) {
                return false;
            }
        }
        
        return true;
    }

    /**
     * @brief Check if more data is available
     * 
     * @return true if there's more data to read
     */
    [[nodiscard]] bool has_more_data()
    {
        return m_buffer.GetSpace() > 0;
    }

    /**
     * @brief Get remaining data size
     * 
     * @return Size in bytes
     */
    [[nodiscard]] size_t remaining_size()
    {
        return m_buffer.GetSpace();
    }

private:
    net::MemoryBuffer m_buffer;
};

/**
 * @brief Packet processor for handling incoming packets
 */
class PacketProcessor
{
public:
    using HandlerFunction = std::function<bool(LPCHARACTER, DynamicPacketReader&)>;
    
    /**
     * @brief Register a handler for a specific packet type
     * 
     * @param header Main header
     * @param sub_header Sub header
     * @param handler Handler function
     */
    void register_handler(uint8_t header, uint8_t sub_header, HandlerFunction handler)
    {
        uint16_t key = make_key(header, sub_header);
        m_handlers[key] = std::move(handler);
    }
    
    /**
     * @brief Process an incoming packet
     * 
     * @param ch Character that received the packet
     * @param data Packet data
     * @param size Packet size
     * @return true if packet was handled
     */
    bool process_packet(LPCHARACTER ch, const void* data, size_t size)
    {
        if (!ch || !data || size < sizeof(DynamicPacketInfo)) {
            return false;
        }
        
        // Create packet reader
        DynamicPacketReader reader(data, size);
        
        // Read header
        DynamicPacketInfo header;
        if (!reader.read_header(header)) {
            sys_err("Failed to read packet header");
            return false;
        }
        
        // Find handler
        uint16_t key = make_key(header.header, header.sub_header);
        auto it = m_handlers.find(key);
        if (it == m_handlers.end()) {
            // No handler registered for this packet type
            return false;
        }
        
        // Call handler
        try {
            return it->second(ch, reader);
        } catch (const std::exception& e) {
            sys_err("Exception in packet handler: %s", e.what());
            return false;
        }
    }
    
private:
    /**
     * @brief Create a key from header and sub_header
     */
    static uint16_t make_key(uint8_t header, uint8_t sub_header)
    {
        return (static_cast<uint16_t>(header) << 8) | sub_header;
    }
    
    std::unordered_map<uint16_t, HandlerFunction> m_handlers;
};

// Global packet processor instance
inline PacketProcessor& GetPacketProcessor()
{
    static PacketProcessor processor;
    return processor;
}

/**
 * @brief Helper functions for common packet operations
 */
namespace utils {
    /**
     * @brief Send notification to a character
     * 
     * @tparam MessageType Type of message
     * @param character Recipient character
     * @param header Main header
     * @param sub_header Sub header
     * @param message Message to send
     */
    template <typename MessageType>
    void send_notification(LPCHARACTER character, uint8_t header, uint8_t sub_header, const MessageType& message)
    {
        DynamicPacketBuilder<> builder;
        builder.add_header(header, sub_header)
               .add_payload(message)
               .send_to_client(character);
    }

    /**
     * @brief Send serialized notification to a character
     * 
     * @tparam MessageType Type of message (must be glaze serializable)
     * @param character Recipient character
     * @param header Main header
     * @param sub_header Sub header
     * @param message Message to send
     */
    template <typename MessageType>
    requires GlazeSerializable<MessageType>
    void send_serialized_notification(LPCHARACTER character, uint8_t header, uint8_t sub_header, const MessageType& message)
    {
        DynamicPacketBuilder<> builder;
        builder.add_header(header, sub_header)
               .add_serialized_payload(message)
               .send_to_client(character);
    }
    
    /**
     * @brief Register a packet handler
     * 
     * @tparam Func Handler function type
     * @param header Main header
     * @param sub_header Sub header
     * @param handler Handler function
     */
    template <typename Func>
    void register_packet_handler(uint8_t header, uint8_t sub_header, Func&& handler)
    {
        GetPacketProcessor().register_handler(header, sub_header, std::forward<Func>(handler));
    }
}

} // namespace networkutils

#endif // NETWORKUTILS_HPP