#include <QApplication>
/*#include "pcapengine.h"
#include "pcapethernetstack.h"
#include "pcapipv4stack.h"
#include "pcaptcpstack.h"
#include "printer.h"
*/
#include "httpppmainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  /*PcapEngine pe(argc > 1 ? argv[1] : "sample.pcap");
  PcapEthernetStack ether;
  PcapIPv4Stack ip;
  PcapTcpStack tcp;
  Printer p;
  //QObject::connect(&pe, SIGNAL(layer1PacketReceived(PcapLayer1Packet)),
  //                 &p, SLOT(layer2PacketReceived(PcapLayer1Packet)));
  QObject::connect(&pe, SIGNAL(layer1PacketReceived(PcapLayer1Packet)),
                   &ether, SLOT(layer1PacketReceived(PcapLayer1Packet)));
  QObject::connect(&ether, SIGNAL(layer2PacketReceived(PcapLayer2Packet)),
                   &ip, SLOT(layer2PacketReceived(PcapLayer2Packet)));
  //QObject::connect(&ip, SIGNAL(layer3PacketReceived(PcapLayer3Packet)),
  //                 &p, SLOT(layer3PacketReceived(PcapLayer3Packet)));
  QObject::connect(&ip, SIGNAL(ipv4PacketReceived(PcapIPv4Packet)),
                   &tcp, SLOT(ipPacketReceived(PcapIPv4Packet)));
  //QObject::connect(&tcp, SIGNAL(tcpUpstreamPacket(PcapTcpPacket,PcapTcpConversation)),
  //                 &p, SLOT(tcpUpstreamPacket(PcapTcpPacket,PcapTcpConversation)));
  //QObject::connect(&tcp, SIGNAL(tcpDownstreamPacket(PcapTcpPacket,PcapTcpConversation)),
  //                 &p, SLOT(tcpDownstreamPacket(PcapTcpPacket,PcapTcpConversation)));
  // the following quit condition would not work if stacks become multithreaded
  QObject::connect(&pe, SIGNAL(captureTerminated()), &a, SLOT(quit()));
  pe.start();*/
  HttpppMainWindow w;
  w.show();
  w.loadFilename(argc > 1 ? argv[1] : "sample.pcap");
  return a.exec();
}
