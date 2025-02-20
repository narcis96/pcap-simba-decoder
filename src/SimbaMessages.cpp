#include "SimbaMessages.h"

#include <string>
#include <sstream>

namespace simba {

double Decimal5NULLtoDouble(const Decimal5NULL&d) {
    return (d.mantissa != Decimal5NULL::NULL_VALUE? d.mantissa * Decimal5NULL::exponent: 0);
}

std::string SerializeOrderUpdates(const std::vector<OrderUpdate>& orderUpdates) {
    std::ostringstream data;
    data << "\"orderUpdates\":[";
    for (const auto& update : orderUpdates) {
        data << "{"
             << "\"md_entry_id\":" << update.md_entry_id << ","
             << "\"md_entry_px\":" << update.md_entry_px.mantissa * Decimal5::exponent << ","
             << "\"md_entry_size\":" << update.md_entry_size << ","
             << "\"md_flags\":" << static_cast<uint64_t>(update.md_flags) << ","
             << "\"md_flags2\":" << update.md_flags2 << ","
             << "\"security_id\":" << update.security_id << ","
             << "\"rpt_seq\":" << update.rpt_seq << ","
             << "\"md_update_action\":" << static_cast<int>(update.md_update_action) << ","
             << "\"md_entry_type\":\"" << static_cast<char>(update.md_entry_type) << "\""
             << "},";
    }
    if (!orderUpdates.empty()) data.seekp(-1, std::ios_base::end);
    data << "]";
    return data.str();
}

std::string SerializeOrderExecutions(const std::vector<OrderExecution>& orderExecutions) {
    std::ostringstream data;
    data << "\"orderExecutions\":[";
    for (const auto& execution : orderExecutions) {
        data << "{"
             << "\"md_entry_id\":" << execution.md_entry_id << ","
             << "\"md_entry_px\":"<< Decimal5NULLtoDouble(execution.md_entry_px) << ","
             << "\"md_entry_size\":" << execution.md_entry_size << ","
             << "\"last_px\":" << execution.last_px.mantissa * Decimal5::exponent << ","
             << "\"last_qty\":" << execution.last_qty << ","
             << "\"trade_id\":" << execution.trade_id << ","
             << "\"md_flags\":" << static_cast<uint64_t>(execution.md_flags) << ","
             << "\"md_flags2\":" << execution.md_flags2 << ","
             << "\"security_id\":" << execution.security_id << ","
             << "\"rpt_seq\":" << execution.rpt_seq << ","
             << "\"md_update_action\":"
             << static_cast<int>(execution.md_update_action) << ","
             << "\"md_entry_type\":\"" << static_cast<char>(execution.md_entry_type) << "\""
             << "},";
    }
    if (!orderExecutions.empty()) data.seekp(-1, std::ios_base::end);
    data << "]";
    return data.str();
}

std::string SerializeOrderBookSnapshots(const std::vector<OrderBookSnapshotWithEntries>& orderBookSnapshots) {
    std::ostringstream data;
    data << "\"orderBookSnapshots\":[";
    for (const auto& orderBook : orderBookSnapshots) {
        const auto snapshot = orderBook.snapshot;
        data << "{"
             << "\"security_id\":" << snapshot.security_id << ","
             << "\"last_msg_seq_num_processed\":" << snapshot.last_msg_seq_num_processed << ","
             << "\"rpt_seq\":" << snapshot.rpt_seq << ","
             << "\"exchange_trading_session_id\":" << snapshot.exchange_trading_session_id << ","
             << "\"no_md_entries\":{" << "\"block_length\":" << snapshot.no_md_entries.block_length << ","
             << "\"num_in_group\":" << static_cast<int>(snapshot.no_md_entries.num_in_group) << "},"
             << "\"entries\":[";
        for (const auto& entry : orderBook.entries) {
            data << "{"
                 << "\"md_entry_id\":" << entry.md_entry_id << ","
                 << "\"transact_time\":" << entry.transact_time << ","
                 << "\"md_entry_px\":" << Decimal5NULLtoDouble(entry.md_entry_px) << ","
                 << "\"md_entry_size\":" << entry.md_entry_size << ","
                 << "\"trade_id\":" << entry.trade_id << ","
                 << "\"md_flags\":" << static_cast<uint64_t>(entry.md_flags) << ","
                 << "\"md_flags2\":" << entry.md_flags2 << ","
                 << "\"md_entry_type\":\"" << static_cast<uint64_t>(entry.md_entry_type) << "\""
                 << "},";
        }
        if (!orderBook.entries.empty())
            data.seekp(-1, std::ios_base::end);
        data << "]},";
    }
    if (!orderBookSnapshots.empty())
        data.seekp(-1, std::ios_base::end);
    data << "]";
    return data.str();
}

} // namespace simba
