#ifndef PCAP_PARSER_H
#define PCAP_PARSER_H

#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <fstream>

namespace parser {

// PCAP Magic Numbers
constexpr uint32_t PCAP_MAGIC_NUMBER = 0xA1B2C3D4;
constexpr uint32_t PCAP_MAGIC_NUMBER_SWAPPED = 0xA1B23C4D;
constexpr size_t ETHERNET_HEADER_SIZE = 14;
constexpr size_t UDP_HEADER_SIZE = 8;
constexpr size_t IP_V4_BASE_HEADER_SIZE = 20;

#pragma pack(push, 1)
struct PcapGlobalHeader {
    uint32_t magic_number;
    uint16_t version_major;
    uint16_t version_minor;
    int32_t  thiszone;
    uint32_t sigfigs;
    uint32_t snaplen;
    uint32_t network;
};

struct PcapPacketHeader {
    uint32_t ts_sec;
    uint32_t ts_usec;
    uint32_t incl_len;
    uint32_t orig_len;
};
#pragma pack(pop)

class PcapParser {
public:
    explicit PcapParser(const std::string& filename);
    ~PcapParser();

    // Reads the global header from the PCAP file.
    // Returns true on success, false on failure.
    bool readGlobalHeader();

    // Reads the next packet's data into the provided vector.
    // Returns true if a packet was successfully read, false otherwise.
    bool readNextPacket(std::vector<uint8_t>& packetData);

private:
    std::ifstream file_;
    PcapGlobalHeader header_;
};

} // namespace parser

#endif // PCAP_PARSER_H
