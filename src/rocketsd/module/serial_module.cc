#include <rocketsd/module/serial_module.hh>

#include <log/log.hh>
#include <util/json.hh>

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
}

SerialModule::SerialModule(QObject* parent, protocol::ProtocolSP protocol)
    : Module(parent, protocol)
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

    std::string port;
    unsigned int baudrate;

    if (!util::json::validate("SerialModule", config,
        util::json::required(port, "port"),
        util::json::required(baudrate, "baudrate")
    )) {
        return false;
    }

    serialport_ = new QSerialPort(port.c_str());
    serialport_->setBaudRate(baudrate);

    connect(serialport_, &QSerialPort::readyRead, this, &SerialModule::onData);
    connect(serialport_, &QSerialPort::errorOccurred, this, &SerialModule::onError);
    if (!serialport_->open(QIODevice::ReadWrite)) {
        return false;
    }

    logging::info("SerialModule") << "Successfully init'd Serial client" << logging::tag{"id", id()} << logging::endl;
    return true;
}

void SerialModule::onPacket(radio_packet_t packet)
{
    //FIXME: This has not been tested whatsoever
    serialport_->write((char*)&packet, sizeof(radio_packet_t));
}

void SerialModule::onData()
{
    radio_packet_t packet;
    QByteArray buffer = serialport_->readAll();

    for (const auto& byte: buffer) {
        buffer_.push_back(static_cast<std::uint8_t>(byte));

        if (buffer_.size() > sizeof(radio_packet_t)) {
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
                emit packetReady(packet);
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