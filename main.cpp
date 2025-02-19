#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>

#include "PcapParser.h"
#include "SimbaDecoder.h"


int main(int argc, char* argv[]) {
     // Ensure correct number of arguments
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <pcap file path> <output file path>" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Capture the file paths from the command line arguments
    const std::string pcapFileName = argv[1];
    const std::string outputFileName = argv[2];
    
    std::ofstream outFile(outputFileName);
    if (!outFile.is_open()) {
        std::cerr <<  "Error: Could not open output file.";
        return EXIT_FAILURE;
    }
    auto start = std::chrono::steady_clock::now();
    try {
        // Initialize the parser and start parsing
        parser::PcapParser parser(pcapFileName);
        if (!parser.readGlobalHeader()) {
            return EXIT_FAILURE;
        }
        for (std::vector<uint8_t> packetData; parser.readNextPacket(packetData); ) {
            simba::SimbaDecoder decoder(packetData);
            if (!decoder.Decode()) {
                std::cerr << "Failed to decode one packet" << std::endl;
                continue;
            }
            outFile << decoder.GetDecodedMessages().toJSON() << std::endl;
        }

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        outFile.close(); // defer
        return EXIT_FAILURE;
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration<double, std::milli>(end-start).count()/ 1000 << " seconds" << std::endl;
    outFile.close(); // defer
    return 0;
}
