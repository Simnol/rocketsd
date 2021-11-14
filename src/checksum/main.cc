#include <filesystem>
#include <iostream>

#include <shared/interfaces/radio/radio_packet.h>
#include <rocketsd/protocol/protocol_parser.hh>
#include <util/switch.hh>


int main(int argc, const char* argv[])
{
    rocketsd::protocol::ProtocolParser parser;
    auto* protocol = parser.parse(std::filesystem::path(argc == 2 ? argv[1] : "protocol.xml"));
    if (!protocol) {
        std::cerr << "Failed to parse protocol!" << std::endl;
        return -1;
    }

    radio_packet_t packet;
    int node_id, message_id;

    std::cout << "Node: ";
    std::cin >> node_id;

    auto* node = (*protocol)[node_id];
    if (!node) {
        std::cout << "Unknown node" << std::endl;
    }

    std::cout << "Message: ";
    std::cin >> message_id;

    auto* message = (*node)[message_id];
    if (!message) {
        std::cout << "Unknown message" << std::endl;
    }

    radio_packet_payload_t payload = { 0 };

    std::cout << "Payload [" << message->type() << "]: ";
    util::switcher::string(std::string(message->type()), {
        {"int", [&] { std::cin >> payload.INT; }},
        {"uint", [&] { std::cin >> payload.UINT; }},
        {"float", [&] { std::cin >> payload.FLOAT; }},
        {"bytes", [&] { std::cout << "not yet supported" << std::endl; }},
    });

    packet.node = node_id;
    packet.message_id = message_id;
    packet.payload = payload;

    std::cout << "CRC: " << static_cast<int>(radio_compute_crc(&packet)) << std::endl;

    delete protocol;
}