#ifndef PROTO_PACKET_H_
#define PROTO_PACKET_H_

#include <memory>

#include "packet.pb.h"
#include <QObject>

namespace cute::proto {
typedef std::shared_ptr<Packet> PacketSP;
}

Q_DECLARE_METATYPE(cute::proto::PacketSP)

#endif