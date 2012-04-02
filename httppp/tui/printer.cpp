#include "printer.h"
#include <QtDebug>
//#include "pcapethernetpacket.h"
//#include "pcapipv4packet.h"

/*void Printer::layer1PacketReceived(PcapLayer1Packet packet) {
  PcapEthernetPacket ether(packet);
  if (ether.layer3Proto() != 0x800) {
    //qDebug() << "skipping non IPv4 packet";
    return;
  }
  PcapIPv4Packet ip(ether);
  qDebug() << packet << ether << ip;
*/  /*qDebug() << packet.timestamp().time().toString("HH:mm:ss,zzz")
           << packet.wirelen() << ether.dst() << ether.src()
           //<< QString::number(ether.proto(), 16)
           << ip.english();
           //<< ip.src() << ip.dst() << ip.proto() << ip.headerSize()
           //<< ip.payloadSize()
           //<< ip.payloadToByteArray().toHex()
              ;*/
//}

void Printer::layer1PacketReceived(QPcapLayer1Packet packet) {
  qDebug() << packet;
}

void Printer::layer2PacketReceived(QPcapLayer2Packet packet) {
  qDebug() << packet;
}

void Printer::layer3PacketReceived(QPcapLayer3Packet packet) {
  qDebug() << packet;
}

void Printer::tcpUpstreamPacket(QPcapTcpPacket packet,
                                QPcapTcpConversation conversation) {
  qDebug() << conversation.id() << (packet.isEmpty() ? "-->" : ">>>") << packet;
}

void Printer::tcpDownstreamPacket(QPcapTcpPacket packet,
                                  QPcapTcpConversation conversation) {
  qDebug() << conversation.id() << (packet.isEmpty() ? "<--" : "<<<") << packet;
}

void Printer::httpHit(QPcapTcpConversation conversation, QPcapHttpHit hit) {
  qDebug() << conversation.id() << "HIT" << hit;
}

void Printer::httpHitToCsv(QPcapTcpConversation conversation, QPcapHttpHit hit) {
  Q_UNUSED(conversation);
  if (!_hitCsvHeaderPrinted) {
    hit.writeCsvHeader(&_stdout);
    _hitCsvHeaderPrinted = true;
  }
  hit.writeCsv(&_stdout);
}
