#include "pcapipv4packet.h"

QString PcapIPv4PacketData::english() const {
  return QString("PcapIPv4Packet(%1, %2, %3, %4, %5)")
      .arg(src()).arg(dst()).arg(layer4Proto())
      .arg(_payload.size()).arg(_payload.toHex().constData());
}

