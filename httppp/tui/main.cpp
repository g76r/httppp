#include <QApplication>
#include "qpcapengine.h"
#include "qpcapethernetstack.h"
#include "qpcapipv4stack.h"
#include "qpcaptcpstack.h"
#include "printer.h"

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);
  QPcapEngine pe(argc > 1 ? argv[1] : "sample.pcap");
  QPcapEthernetStack ether;
  QPcapIPv4Stack ip;
  QPcapTcpStack tcp;
  Printer p;
  //QObject::connect(&pe, SIGNAL(layer1PacketReceived(QPcapLayer1Packet)),
  //                 &p, SLOT(layer2PacketReceived(QPcapLayer1Packet)));
  QObject::connect(&pe, SIGNAL(layer1PacketReceived(QPcapLayer1Packet)),
                   &ether, SLOT(layer1PacketReceived(QPcapLayer1Packet)));
  QObject::connect(&ether, SIGNAL(layer2PacketReceived(QPcapLayer2Packet)),
                   &ip, SLOT(layer2PacketReceived(QPcapLayer2Packet)));
  //QObject::connect(&ip, SIGNAL(layer3PacketReceived(QPcapLayer3Packet)),
  //                 &p, SLOT(layer3PacketReceived(QPcapLayer3Packet)));
  QObject::connect(&ip, SIGNAL(ipv4PacketReceived(QPcapIPv4Packet)),
                   &tcp, SLOT(ipPacketReceived(QPcapIPv4Packet)));
  QObject::connect(&tcp, SIGNAL(tcpUpstreamPacket(QPcapTcpPacket,QPcapTcpConversation)),
                   &p, SLOT(tcpUpstreamPacket(QPcapTcpPacket,QPcapTcpConversation)));
  QObject::connect(&tcp, SIGNAL(tcpDownstreamPacket(QPcapTcpPacket,QPcapTcpConversation)),
                   &p, SLOT(tcpDownstreamPacket(QPcapTcpPacket,QPcapTcpConversation)));
  // the following quit condition would not work if stacks become multithreaded
  QObject::connect(&pe, SIGNAL(captureTerminated()), &a, SLOT(quit()));
  pe.start();
  return a.exec();
}
