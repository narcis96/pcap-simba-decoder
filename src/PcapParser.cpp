#include "PcapParser.h"

#include <iostream>
#include <ctime>

namespace parser {

PcapParser::PcapParser(const std::string& filename) {
    file_.open(filename, std::ios::binary);
    if (!file_.is_open()) {
        throw std::runtime_error("Failed to open file");
    }
}

PcapParser::~PcapParser() {
    file_.close();
}

bool PcapParser::readGlobalHeader() {
    if (!file_.read(reinterpret_cast<char*>(&header_), sizeof(PcapGlobalHeader))) {
        return false;
    }
    
    // const uint32_t magic_number = header_.magic_number;
    // Check if the magic number is valid.
    // if (magic_number != PCAP_MAGIC_NUMBER && magic_number == PCAP_MAGIC_NUMBER_SWAPPED) {
    //     throw std::runtime_error("Invalid PCAP file format: Unknown magic number: " + std::to_string(magic_number));
    // }
    return true;
}

bool PcapParser::readNextPacket(std::vector<uint8_t>& packetData) {
    static char buff[IP_V4_BASE_HEADER_SIZE];
    PcapPacketHeader packetHeader;
    if (!file_.read(reinterpret_cast<char*>(&packetHeader), sizeof(PcapPacketHeader))) {
        return false;
    }
    if (!file_.seekg(ETHERNET_HEADER_SIZE, std::ios::cur)){ // skip Ethernet header
        return false;
    }
    if (!file_.read(buff, IP_V4_BASE_HEADER_SIZE)) {// read IPv4
        return false;
    }
    uint8_t versionAndHeaderLength = uint8_t(buff[0]);
    const uint8_t ihl = versionAndHeaderLength & 0x0F;    
    const size_t ipHeaderSize = ihl * 4;

    if (const int extendedHeaderSize = static_cast<int>(ipHeaderSize - IP_V4_BASE_HEADER_SIZE); extendedHeaderSize > 0) {
        if (!file_.seekg(ETHERNET_HEADER_SIZE, std::ios::cur)){ // skip extended IPv4 header
            std::cerr << "Error reading extended IPv4 header.";
            return false;
        }
    }
    if (!file_.seekg(UDP_HEADER_SIZE, std::ios::cur)){ // skip UDP header
        return false;
    }
    // Calculate the total length of headers
    const size_t headersLength = ETHERNET_HEADER_SIZE + ipHeaderSize + UDP_HEADER_SIZE;
    size_t payloadSize = packetHeader.incl_len - headersLength;
    packetData = std::vector<uint8_t>(payloadSize, 0);
    if (!file_.read(reinterpret_cast<char*>(packetData.data()), payloadSize)) {
        return false;
    }
    
    return true;
}

} // namespace parser