#ifndef SIMBADECODER_H
#define SIMBADECODER_H

#include <cstdint>
#include <vector>
#include <utility>
#include <cstring>
#include <iostream>

#include "SimbaMessages.h"

namespace simba {

struct DecodedMessages {
    std::vector<OrderUpdate> orderUpdates;
    std::vector<OrderExecution> orderExecutions;
    std::vector<OrderBookSnapshotWithEntries> orderBookSnapshots;
    std::string toJSON() const;
};

class SimbaDecoder {
public:
    SimbaDecoder(const std::vector<uint8_t>& data);

    bool Decode();
    const DecodedMessages& GetDecodedMessages() const;

private:
    // Template helper to read data from our buffer.
    template <typename T>
    bool readFromBuffer(T &value) {
        if (offset_ + sizeof(T) > data_.size())
            return false;
        std::memcpy(&value, &data_[offset_], sizeof(T));
        offset_ += sizeof(T);
        return true;
    }
    const std::vector<uint8_t>& data_;
    size_t offset_;
    DecodedMessages value_;
};

} // namespace simba

#endif // SIMBADECODER_H
