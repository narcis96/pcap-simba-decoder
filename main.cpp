#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <future>
#include "PcapParser.h"
#include "SimbaDecoder.h"
#include "ThreadPool.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <pcap file path> <output file path>" << std::endl;
        return EXIT_FAILURE;
    }
    
    const std::string pcapFileName = argv[1];
    const std::string outputFileName = argv[2];
    
    std::ofstream outFile(outputFileName);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open output file." << std::endl;
        return EXIT_FAILURE;
    }
    
    // Create a thread pool using the number of hardware threads available.
    ThreadPool pool(std::thread::hardware_concurrency());
    std::vector<std::future<std::string>> futures;

    auto start = std::chrono::steady_clock::now();

    try {
        parser::PcapParser parser(pcapFileName);
        if (!parser.readGlobalHeader()) {
            return EXIT_FAILURE;
        }
        // Enqueue a decoding task for each packet read.
        for (std::vector<uint8_t> packetData; parser.readNextPacket(packetData); ) {
            futures.emplace_back(
                pool.enqueue([packetData]() -> std::string {
                    simba::SimbaDecoder decoder(packetData);
                    if (!decoder.Decode()) {
                        // std::cerr << "Failed to decode one packet" << std::endl;
                        return std::string{};
                    }
                    return decoder.GetDecodedMessages().toJSON();
                })
            );
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        outFile.close();
        return EXIT_FAILURE;
    }
    
    // Batch write configuration.
    const size_t batchSize = 50000;
    size_t count = 0;
    std::string batchBuffer;
    
    // Wait for all tasks to complete and gather their output.
    for (auto &future : futures) {
        std::string jsonOutput = future.get();
        if (!jsonOutput.empty()) {
            batchBuffer.append(jsonOutput);
            batchBuffer.push_back('\n');
            count++;
            if (count % batchSize == 0) {
                outFile << batchBuffer;
                batchBuffer.clear();
            }
        }
    }
    // Write any remaining output.
    if (!batchBuffer.empty()) {
        outFile << batchBuffer;
    }
    
    auto end = std::chrono::steady_clock::now();
    std::cout << "Processing time: " 
              << std::chrono::duration<double, std::milli>(end - start).count() / 1000 
              << " seconds" << std::endl;
    
    outFile.close();
    return 0;
}
