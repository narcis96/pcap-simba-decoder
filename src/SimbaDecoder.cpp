#include "SimbaDecoder.h"

#include <sstream>

namespace simba {

SimbaDecoder::SimbaDecoder(const std::vector<uint8_t>& data)
    : data_(data), offset_(0)
{}

bool SimbaDecoder::Decode() {
    MarketDataPacketHeader marketDataPacketHeader;
    if (!readFromBuffer(marketDataPacketHeader)) {
        return false;
    }
    if (marketDataPacketHeader.IsIncremental()) {
        offset_ += INCREMENTAL_PACKET_HEADER_SIZE;
    }
    while (offset_ < data_.size()) {
        SBEHeader header;
        if (!readFromBuffer(header))  {
            return false;
        }
        // std::cout << header.schema_id << " " << header.template_id << std::endl;
        switch (header.template_id) {
            case OrderUpdate::TEMPLATE_ID: {        
                if (!readFromBuffer(value_.orderUpdates.emplace_back())) {
                    value_.orderUpdates.pop_back();
                    return false;
                }
                break;
            }
            case OrderExecution::TEMPLATE_ID: {
                if (!readFromBuffer(value_.orderExecutions.emplace_back())) {
                    value_.orderExecutions.pop_back();
                    return false;
                }
                break;
            }
            case OrderBookSnapshot::TEMPLATE_ID: {
                OrderBookSnapshotWithEntries orderBook;
                if (!readFromBuffer(orderBook.snapshot)) {
                    std::cerr << "failed to read OrderBookSnapshot" << std::endl;
                    return false;
                }
                orderBook.entries.resize(orderBook.snapshot.no_md_entries.num_in_group);
                for (auto& entry : orderBook.entries) {
                    if (!readFromBuffer(entry)) {
                        std::cerr << "failed to read one entry of OrderBookSnapshot" << std::endl;
                        return false;
                    }
                }
                value_.orderBookSnapshots.emplace_back(orderBook);
                break;
            }
            default: {
                // Skip unknown message body bytes.
                offset_ += header.block_length;
                break;
            }
        }
    }
    return true;
}

const DecodedMessages& SimbaDecoder::GetDecodedMessages() const {
    return value_;
}

std::string DecodedMessages::toJSON() const
{
    std::ostringstream json;
    json << "{";
    json << SerializeOrderUpdates(orderUpdates) << ",";
    json << SerializeOrderExecutions(orderExecutions) <<",";
    json << SerializeOrderBookSnapshots(orderBookSnapshots);
    json << "}";

    return json.str();
}

} // namespace simba
