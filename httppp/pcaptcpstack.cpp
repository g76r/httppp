#include "pcaptcpstack.h"

void PcapTcpStack::ipPacketReceived(PcapIPv4Packet packet) {
  PcapTcpPacket tcp(packet);
  // FIXME handle TCP conversation detection
  // FIXME handle TCP deduplication and reordering
  qDebug() << "received tcp packet" << tcp;
}
