#include "pcapethernetstack.h"
#include <QtDebug>

void PcapEthernetStack::layer1PacketReceived(PcapLayer1Packet packet) {
  PcapEthernetPacket ether(packet);
  if (ether.isNull())
    qDebug() << "ignoring non-ethernet packet" << packet;
  else
    emit layer2PacketReceived(ether);
}
