#include <gtest/gtest.h>

#include <cute/proto/packet.hh>

TEST(Packet, makeData_works)
{
    cute::proto::Data data;
    cute::proto::makeData(data, {
        {"cute.proto.test.1", 1, 3.14},
        {"cute.proto.test.2", 2, false},
        {"cute.proto.test.3", 3, "cstr"},
        {"cute.proto.test.4", 4, std::string("cppstr")},
        {"cute.proto.test.5", 5, 42}
    });

    EXPECT_EQ(data.measurements_size(), 5);

    EXPECT_EQ(data.measurements(0).source(), "cute.proto.test.1");
    EXPECT_EQ(data.measurements(0).float_(), 3.14);
    EXPECT_EQ(data.measurements(0).timestamp(), 1);

    EXPECT_EQ(data.measurements(1).bool_(), false);
    EXPECT_EQ(data.measurements(2).string(), "cstr");
    EXPECT_EQ(data.measurements(3).string(), "cppstr");
    EXPECT_EQ(data.measurements(4).int_(), 42);
}

TEST(Packet, makeHandshake_works)
{
    cute::proto::Handshake handshake;
    cute::proto::makeHandshake(handshake, "cute.proto.test", {
        {"cute.proto.1", typeid(bool)},
        {"cute.proto.2", typeid(int)},
        {"cute.proto.3", typeid(double)},
        {"cute.proto.4", typeid(std::string)}
    });

    EXPECT_EQ(handshake.name(), "cute.proto.test");
    EXPECT_EQ(handshake.commands_size(), 4);
    EXPECT_EQ(handshake.commands(0).name(), "cute.proto.1");

    EXPECT_EQ(handshake.commands(0).type(), cute::proto::Handshake_Command_Type_BOOL);
    EXPECT_EQ(handshake.commands(1).type(), cute::proto::Handshake_Command_Type_INT);
    EXPECT_EQ(handshake.commands(2).type(), cute::proto::Handshake_Command_Type_FLOAT);
    EXPECT_EQ(handshake.commands(3).type(), cute::proto::Handshake_Command_Type_STRING);
}