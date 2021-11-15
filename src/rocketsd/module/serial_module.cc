#include <rocketsd/module/serial_module.hh>

#include <log/log.hh>
#include <util/json.hh>
#include <util/time.hh>
#include <util/switch.hh>
#include <rocketsd/protocol/name.hh>
#include <rocketsd/protocol/protocol_parser.hh>

namespace rocketsd::modules {

namespace {
    const std::string qtSocketErrorToString(QSerialPort::SerialPortError error)
    {
        const char* errorStrings[] = {
            "No error",
            "Device not found",
            "Permission denied",
            "Could not open",
            "Parity error",
            "Frame error",
            "Break condition error",
            "Could not write",
            "Could not read",
            "Resource error",
            "Unsupported operation",
            "Unknown error",
            "Timeout",
            "Not open",
        };

        return (error >= 0 && error < sizeof(errorStrings)) ? errorStrings[error] : "Unknown error";
    }

    bool buildMeasurement(protocol::Protocol& protocol, radio_packet_t& packet, cute::proto::Measurement& measurement)
    {
        auto* node = protocol[packet.node];
        if (!node) {
            return false;
        }

        auto* message = (*node)[packet.message_id];
        if (!message) {
            return false;
        }

        measurement.set_source(protocol::to_cute_name(&protocol, node, message));
        measurement.set_timestamp(util::time::now<std::milli>());

        util::switcher::string(std::string(message->type()), {
            {"int",   [&packet, &measurement]() { measurement.set_state(packet.payload.INT); }},
            {"float", [&packet, &measurement]() { measurement.set_number(packet.payload.FLOAT); }},
        }, []() { logging::err("SerialModule") << "Unsupported payload type" << logging::endl; });
        return measurement.value_case() != cute::proto::Measurement::VALUE_NOT_SET;
    }
}

SerialModule::SerialModule(QObject* parent)
    : Module(parent)
{
}

SerialModule::~SerialModule()
{
}

bool SerialModule::init(json& config)
{
    if (!Module::init(config)) {
        return false;
    }

    std::string port, protocol_path;
    unsigned int baudrate;

    if (!util::json::validate("SerialModule", config,
        util::json::required(port, "port"),
        util::json::required(protocol_path, "protocol"),
        util::json::required(baudrate, "baudrate")
    )) {
        return false;
    }

    if (!std::filesystem::is_regular_file(protocol_path)) {
        logging::err("SerialModule") << "Invalid protocol path '" << protocol_path << "'" << logging::endl;
        return false;
    }

    protocol::ProtocolParser parser;
    protocol_ = protocol::ProtocolSP(parser.parse(std::filesystem::path(protocol_path)));

    if (!protocol_) {
        logging::err("SerialModule") << "Could not parse protocol definition" << logging::endl;
        return false;
    }

    serialport_ = new QSerialPort(port.c_str(), this);
    serialport_->setBaudRate(baudrate);

    connect(serialport_, &QSerialPort::readyRead, this, &SerialModule::onData);
    connect(serialport_, &QSerialPort::errorOccurred, this, &SerialModule::onError);
    if (!serialport_->open(QIODevice::ReadWrite)) {
        logging::err("SerialModule") << "Could not open serial port" << logging::endl;
        return false;
    }

    logging::info("SerialModule") << "Successfully init'd Serial client" << logging::tag{"id", id()} << logging::endl;
    return true;
}

void SerialModule::onMessage(Message msg)
{
    protocol::Node* node;
    protocol::Message* message;

    protocol::from_cute_name(protocol_.get(), msg.measurement.source(), &node, &message);

    if (node && message) {
        radio_packet_t packet;
        memset(&packet, 0, sizeof(radio_packet_t));

        packet.node = node->id();
        packet.message_id = message->id();
        
        switch (msg.measurement.value_case()) {
            case cute::proto::Measurement::kNumber:
                packet.payload.FLOAT = msg.measurement.number();
                break;
            case cute::proto::Measurement::kState:
                packet.payload.INT = msg.measurement.state();
                break;
            default:
                logging::err("SerialModule") << "Unsupported payload type" << logging::endl;
                return;
        }

        packet.checksum = radio_compute_crc(&packet);
        serialport_->write((char*)&packet, sizeof(radio_packet_t));
    } else {
        logging::warn("SerialModule") << "Received measurement not covered in protocol: ignoring" << logging::tag("name", msg.measurement.source()) << logging::endl;
    }
}

void SerialModule::onData()
{
    radio_packet_t packet;
    QByteArray buffer = serialport_->readAll();

    for (const auto& byte: buffer) {
        buffer_.push_back(static_cast<std::uint8_t>(byte));

        if (buffer_.size() >= sizeof(radio_packet_t)) {
            // Yes, this is very explicit/ugly, but:
            // - Can't memcpy/reinterpret_cast because dequeue is non-continuous
            // - No branch is not pretty but it is efficient, predictable
            packet.node = buffer_[0];
            packet.message_id = buffer_[1];

            packet.payload.BYTES[0] = buffer_[2];
            packet.payload.BYTES[1] = buffer_[3];
            packet.payload.BYTES[2] = buffer_[4];
            packet.payload.BYTES[3] = buffer_[5];

            packet.payload.BYTES[4] = buffer_[6];
            packet.payload.BYTES[5] = buffer_[7];
            packet.payload.BYTES[6] = buffer_[8];
            packet.payload.BYTES[7] = buffer_[9];

            packet.checksum = buffer_[10];

            if (packet.checksum == radio_compute_crc(&packet)) {
                cute::proto::Measurement measurement;
                if (buildMeasurement(*protocol_, packet, measurement)) {
                    emit messageReady({this, measurement});
                } else {
                    logging::err("SerialModule") << "Failed to build measurement" << logging::endl;
                }

                buffer_.erase(buffer_.begin(), buffer_.begin() + sizeof(radio_packet_t));
            } else {
                logging::debug("SerialModule") << "Invalid CRC" 
                    << logging::tag{"buffer_size", (int)buffer_.size()}
                    << logging::tag{"got", (int)packet.checksum}
                    << logging::tag{"expected", (int)radio_compute_crc(&packet)}
                    << logging::endl;
                buffer_.erase(buffer_.begin());
            }
        }
    }
}

void SerialModule::onError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError) {
        logging::err("SerialModule") << "Serial port error: " << qtSocketErrorToString(error) << logging::endl;
    }
}

} // namespace