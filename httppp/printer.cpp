#include "printer.h"
#include <QtDebug>
#include "pcapethernetpacket.h"
#include "pcapipv4packet.h"

void Printer::packetReceived(PcapPacket packet) {
  PcapEthernetPacket ether(packet.payload().size(),
                        (const quint8*)packet.payload().constData());
  if (ether.proto() != 0x800) {
    //qDebug() << "skipping non IPv4 packet";
    return;
  }
  PcapIPv4Packet ip(ether.payloadSize(), ether.payload());
  qDebug() << packet.timestamp().time().toString("HH:mm:ss,zzz")
           << packet.wirelen() << ether.dst() << ether.src()
           //<< QString::number(ether.proto(), 16)
           << ip.src() << ip.dst() << ip.proto() << ip.headerSize()
           << ip.payloadSize()
           //<< ip.payloadToByteArray().toHex()
              ;
}
