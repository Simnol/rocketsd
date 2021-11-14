#include <cute/proto/packet.hh>

namespace cute::proto {

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void makeData(proto::Data& data, std::initializer_list<_measurement_initializer> list)
{
    data.clear_measurements();

    for (const _measurement_initializer& init: list) {
        proto::Measurement& measurement = *(data.add_measurements());

        measurement.set_source(init.s);
        measurement.set_timestamp(init.t);

        std::visit(overloaded {
            [&measurement](bool v) { measurement.set_bool_(v); },
            [&measurement](int v) { measurement.set_state(v); },
            [&measurement](double v) { measurement.set_number(v); },
            [&measurement](const char* v) { measurement.set_string(v); },
            [&measurement](const std::string& v) { measurement.set_string(v); },
            [](auto v) { assert(false); }
        }, init.v);
    }
}

void makeHandshake(proto::Handshake& handshake, const std::string& name, std::initializer_list<_command_initializer> list)
{
    handshake.set_name(name);
    handshake.clear_commands();

    for (const _command_initializer& init: list) {
        proto::Handshake::Command& command = *(handshake.add_commands());

        command.set_name(init.n);

        if (init.t == typeid(bool)) {
            command.set_type(proto::Handshake_Command_Type_BOOL);
        } else if (init.t == typeid(int)) {
            command.set_type(proto::Handshake_Command_Type_STATE);
        } else if (init.t == typeid(double)) {
            command.set_type(proto::Handshake_Command_Type_NUMBER);
        } else if (init.t == typeid(std::string)) {
            command.set_type(proto::Handshake_Command_Type_STRING);
        } else {
            assert(false);
        }
    }
}

}