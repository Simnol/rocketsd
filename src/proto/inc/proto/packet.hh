#ifndef CUTE_PROTO_PACKET_HH_
#define CUTE_PROTO_PACKET_HH_

#include <variant>

#include <packet.pb.h>

#include "delimited_protobuf_stream.hh"

namespace cute::proto {

struct _measurement_initializer {
    const std::string& s;
    const uint64_t t;
    std::variant<bool, int, double, const char*, std::string> v;

    _measurement_initializer(const std::string& s, const uint64_t t, const std::variant<bool, int, double, const char*, std::string>  v)
        : s(s)
        , t(t)
        , v(v)
    {
    }
};

struct _command_initializer {
    const std::string& n;
    const std::type_info& t;

    _command_initializer(const std::string& n, const std::type_info& t)
        : n(n)
        , t(t)
    {
    }
};

void makeData(proto::Data& data, std::initializer_list<_measurement_initializer> list);
void makeHandshake(proto::Handshake& handshake, const std::string& name, std::initializer_list<_command_initializer> list);

typedef DelimitedProtobufStream<Packet> DelimitedPacketStream;

}

#endif