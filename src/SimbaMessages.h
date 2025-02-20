#ifndef SIMBA_MESSAGES_H
#define SIMBA_MESSAGES_H

#include <cstdint>
#include <vector>
#include <cstddef>
#include <type_traits>


namespace simba {

static constexpr size_t INCREMENTAL_PACKET_HEADER_SIZE = 12;

using PacketData = std::vector<uint8_t>;

#pragma pack(push, 1) // Ensures structures are packed with no padding

// Structure for decimal values with a fixed exponent
struct Decimal5 {
    int64_t mantissa;
    static constexpr double exponent = 1e-5;
};

// Structure for nullable decimal values
struct Decimal5NULL {
    static constexpr int64_t MAX_VALUE = 9223372036854775806;
    static constexpr int64_t NULL_VALUE = 9223372036854775807;

    int64_t mantissa;
    static constexpr double exponent = 1e-5;
};

// Market Data Packet Header structure
struct MarketDataPacketHeader {
    bool IsIncremental() const noexcept { return msg_flags & 0x8; }

    uint32_t msg_seq_num;
    uint16_t msg_size;
    uint16_t msg_flags;
    uint64_t sending_time;
};

static_assert(sizeof(MarketDataPacketHeader) == 16,
            "MarketDataPacketHeader size is incorrect");

// Structure for SBE Header
struct SBEHeader {
    uint16_t block_length;
    uint16_t template_id;
    uint16_t schema_id;
    uint16_t version;

};
static_assert(sizeof(SBEHeader) == 8, "SBEHeader size is incorrect");

// Enumeration for MDEntryType
enum class MDEntryType : char
{
    Bid = '0',
    Offer = '1',
    EmptyBook = 'J',
};

// GroupSize structure
struct GroupSize {
    uint16_t block_length;
    uint8_t num_in_group;
};
static_assert(sizeof(GroupSize) == 3, "GroupSize size is incorrect");

// Structure for OrderUpdate message
struct OrderUpdate
{
    static constexpr uint16_t TEMPLATE_ID = 15;

    int64_t md_entry_id;
    Decimal5 md_entry_px;
    int64_t md_entry_size;
    uint64_t md_flags;
    uint64_t md_flags2;
    int32_t security_id;
    uint32_t rpt_seq;
    uint8_t md_update_action;
    MDEntryType md_entry_type;
};
static_assert(sizeof(OrderUpdate) == 50, "OrderUpdate size is incorrect");

// Structure for OrderExecution message
struct OrderExecution
{
    static constexpr uint16_t TEMPLATE_ID = 16;

    int64_t md_entry_id;
    Decimal5NULL md_entry_px;
    int64_t md_entry_size;
    Decimal5 last_px;
    int64_t last_qty;
    int64_t trade_id;
    uint64_t md_flags;
    uint64_t md_flags2;
    int32_t security_id;
    uint32_t rpt_seq;
    uint8_t md_update_action;
    MDEntryType md_entry_type;
};
static_assert(sizeof(OrderExecution) == 74, "OrderExecution size is incorrect");

struct OrderBookSnapshot {
    static constexpr uint16_t TEMPLATE_ID = 17;

    int32_t security_id;
    uint32_t last_msg_seq_num_processed;
    uint32_t rpt_seq;
    uint32_t exchange_trading_session_id;
    GroupSize no_md_entries;
};

static_assert(sizeof(OrderBookSnapshot) == 19, "OrderBookSnapshot size is incorrect");


struct OrderBookEntry {
    int64_t md_entry_id;
    uint64_t transact_time;
    Decimal5NULL md_entry_px;
    int64_t md_entry_size;
    int64_t trade_id;
    uint64_t md_flags;
    uint64_t md_flags2;
    MDEntryType md_entry_type;
};

static_assert(sizeof(OrderBookEntry) == 57, "OrderBookSnapshot size is incorrect");

struct OrderBookSnapshotWithEntries {
    OrderBookSnapshot snapshot;
    std::vector<OrderBookEntry> entries;
};



#pragma pack(pop) // Restore original packing

std::string SerializeOrderUpdates(const std::vector<OrderUpdate>& orderUpdates);

std::string SerializeOrderExecutions(const std::vector<OrderExecution>& orderExecutions);

std::string SerializeOrderBookSnapshots(const std::vector<OrderBookSnapshotWithEntries>& orderBookSnapshots);

} // namespace simba

#endif // SIMBA_MESSAGES_H
