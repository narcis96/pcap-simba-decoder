#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <future>
#include <thread>
#include "PcapParser.h"
#include "SimbaDecoder.h"
#include "SafeVector.h"
#include "ThreadPool.h"

// More threads can lead to increased competition for the CPU cache. 
// Because the working set size is large, running more threads might cause cache thrashing, which slows down execution.
const unsigned int MAX_THREADS = 4;
const unsigned int EXPECTED_NUMBER_OF_PACKETS = 50000;

void writerThread(const std::string& outputFileName, SafeVector<std::future<std::string>> &futures) {
    std::ofstream outFile(outputFileName);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open output file." << std::endl;
        return;
    }
    std::future<std::string> future;
    while (true) {
        if (!futures.pop(future))  {
            break;
        }
        if (std::string jsonOutput = future.get(); !jsonOutput.empty()) {
            outFile << jsonOutput << "\n";
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <pcap file path> <output file path>" << std::endl;
        return EXIT_FAILURE;
    }
    
    const std::string pcapFileName = argv[1];
    const std::string outputFileName = argv[2];
    
    ThreadPool pool(std::min(MAX_THREADS, std::thread::hardware_concurrency()));
    SafeVector<std::future<std::string>> futures(EXPECTED_NUMBER_OF_PACKETS);
    std::thread writer(writerThread, std::cref(outputFileName), std::ref(futures));

    auto start = std::chrono::steady_clock::now();
    try {

        parser::PcapParser parser(pcapFileName);
        if (!parser.readGlobalHeader()) {
            return EXIT_FAILURE;
        }
        
        // Enqueue a decoding task for each packet read.
        for (std::vector<uint8_t> packetData; parser.readNextPacket(packetData); ) {
            futures.push(
                pool.enqueue([packetData]() -> std::string {
                    simba::SimbaDecoder decoder(std::move(packetData));
                    if (!decoder.Decode()) {
                        return std::string{};
                    }
                    return decoder.GetDecodedMessages().toJSON();
                })
            );
        }
        futures.setDone();

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    writer.join();

    auto end = std::chrono::steady_clock::now();
    std::cout << "Total processing time: " 
              << std::chrono::duration<double, std::milli>(end - start).count() / 1000 
              << " seconds" << std::endl;
    return EXIT_SUCCESS;
}