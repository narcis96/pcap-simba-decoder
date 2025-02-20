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
    return true;
}

bool PcapParser::readNextPacket(std::vector<uint8_t>&packetData) {
    // Read the packet header.
    PcapPacketHeader packetHeader;
    if (!file_.read(reinterpret_cast<char*>(&packetHeader), sizeof(packetHeader))) {
        return false;
    }
    const size_t packetSize = packetHeader.incl_len;

    // Ensure our internal buffer is large enough.
    if (buffer_.size() < packetSize) {
        buffer_.resize(packetSize);
    }

    // Read the whole packet at once.
    if (!file_.read(reinterpret_cast<char*>(buffer_.data()), packetSize)) {
        return false;
    }

    // Calculate the offset to the payload.
    size_t offset = 0;
    offset += ETHERNET_HEADER_SIZE;

    // Read IPv4 header information.
    const uint8_t versionAndHeaderLength = buffer_[offset];
    const uint8_t ihl = versionAndHeaderLength & 0x0F;
    const size_t ipHeaderSize = ihl * 4;
    offset += IP_V4_BASE_HEADER_SIZE;
    if (ipHeaderSize > IP_V4_BASE_HEADER_SIZE) {
        offset += (ipHeaderSize - IP_V4_BASE_HEADER_SIZE);
    }
    // offset += (ipHeaderSize > IP_V4_BASE_HEADER_SIZE) * (ipHeaderSize - IP_V4_BASE_HEADER_SIZE);


    // Skip UDP header.
    offset += UDP_HEADER_SIZE;

    if (offset >= packetSize) {
        std::cerr << "Invalid Packet: not enough data" << std::endl;
        return false;
    }

    // Compute payload size.
    const size_t payloadSize = packetSize - offset;
    packetData.resize(payloadSize);
    std::memcpy(packetData.data(), &buffer_[offset], payloadSize);
    return true;
}


} // namespace parser