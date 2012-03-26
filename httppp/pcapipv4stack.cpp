#include "pcapipv4stack.h"

void PcapIPv4Stack::layer2PacketReceived(PcapLayer2Packet packet) {
  if (packet.layer3Proto() != 0x800) { // ignore non-IPv4 packets
    //qDebug() << "ignoring non-IPv4 packet with protocol #"
    //         << QString("0x%1")
    //            .arg(QString::number(packet.layer3Proto(), 16));
    return;
  }
  PcapIPv4Packet ip(packet);
  /*if (!ip.dontFragment() && ip.layer4Proto() == PcapIPv4Packet::TCP)
    qDebug() << "found TCP packet without don't fragment' flag"
             << ip.src() << ip.dst() << ip.payload().size(); */
  if (ip.moreFragments() || ip.fragmentOffset()) {
    // TODO handle fragmented packets
    qDebug() << "ignoring fragmented IPv4 packet fragment"
             << (ip.fragmentOffset() == 0
                 ? "first" : (ip.moreFragments() ? "middle" : "last" ))
             << ip.fragmentOffset() << ip.moreFragments()
             << ip.src() << ip.dst() << ip.identification() << ip.layer4Proto();
  } else {
    emit layer3PacketReceived(ip);
    emit ipv4PacketReceived(ip);
  }
}
