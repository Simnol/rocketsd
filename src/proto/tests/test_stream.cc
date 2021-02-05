#include <gtest/gtest.h>

#include <sstream>

#include <proto/packet.hh>

TEST(DelimitedMessageStream, read_write_works)
{
    std::stringstream ios;
    cute::proto::Packet outPacket, inPacket;
    cute::proto::DelimitedPacketStream outStream(outPacket), inStream(inPacket);


    cute::proto::makeHandshake(*outPacket.mutable_handshake(), "cute.proto.test", {{"cute.proto.test.command", typeid(double)}});
    ios << outStream;
    ios >> inStream;

    EXPECT_FALSE(outStream);
    EXPECT_TRUE(inStream);
    EXPECT_EQ(inPacket.handshake().name(), "cute.proto.test");
    EXPECT_EQ(inPacket.handshake().commands_size(), 1);
}

TEST(DelimitedMessageStream, returns_invalid_on_broken_input)
{
    std::stringstream ios;
    cute::proto::Packet outPacket, inPacket;
    cute::proto::DelimitedPacketStream outStream(outPacket), inStream(inPacket);

    uint8_t buffer[5];

    cute::proto::makeHandshake(*outPacket.mutable_handshake(), "cute.proto.test", {{"cute.proto.test.command", typeid(double)}});
    ios << outStream;
    ios.read((char*) buffer, sizeof(buffer));
    ios >> inStream;
    EXPECT_FALSE(inStream);
}