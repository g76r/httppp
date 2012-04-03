#include <QApplication>
#include "qpcapengine.h"
#include "qpcapethernetstack.h"
#include "qpcapipv4stack.h"
#include "qpcaptcpstack.h"
#include "qpcaphttpstack.h"
#include "printer.h"

static inline void connect(const QObject * sender, const char * signal,
                           const QObject * receiver, const char * method,
                           Qt::ConnectionType type = Qt::AutoConnection ) {
  QObject::connect(sender, signal, receiver, method, type);
}

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);
  QPcapEngine pe(argc > 1 ? argv[1] : "sample.pcap");
  QPcapEthernetStack ether;
  QPcapIPv4Stack ip;
  QPcapTcpStack tcp;
  QPcapHttpStack http;
  Printer p;
  //QObject::connect(&pe, SIGNAL(layer1PacketReceived(QPcapLayer1Packet)),
  //                 &p, SLOT(layer2PacketReceived(QPcapLayer1Packet)));
  connect(&pe, SIGNAL(layer1PacketReceived(QPcapLayer1Packet)),
          &ether, SLOT(layer1PacketReceived(QPcapLayer1Packet)));
  connect(&ether, SIGNAL(layer2PacketReceived(QPcapLayer2Packet)),
          &ip, SLOT(layer2PacketReceived(QPcapLayer2Packet)));
  //QObject::connect(&ip, SIGNAL(layer3PacketReceived(QPcapLayer3Packet)),
  //                 &p, SLOT(layer3PacketReceived(QPcapLayer3Packet)));
  connect(&ip, SIGNAL(ipv4PacketReceived(QPcapIPv4Packet)),
          &tcp, SLOT(ipPacketReceived(QPcapIPv4Packet)));
  connect(&tcp, SIGNAL(tcpUpstreamPacket(QPcapTcpPacket,QPcapTcpConversation)),
          &p, SLOT(tcpUpstreamPacket(QPcapTcpPacket,QPcapTcpConversation)));
  connect(&tcp, SIGNAL(tcpDownstreamPacket(QPcapTcpPacket,QPcapTcpConversation)),
          &p, SLOT(tcpDownstreamPacket(QPcapTcpPacket,QPcapTcpConversation)));
  connect(&tcp, SIGNAL(conversationStarted(QPcapTcpConversation)),
          &http, SLOT(conversationStarted(QPcapTcpConversation)));
  connect(&tcp, SIGNAL(conversationFinished(QPcapTcpConversation)),
          &http, SLOT(conversationFinished(QPcapTcpConversation)));
  connect(&tcp, SIGNAL(tcpUpstreamPacket(QPcapTcpPacket,QPcapTcpConversation)),
          &http, SLOT(tcpUpstreamPacket(QPcapTcpPacket,QPcapTcpConversation)));
  connect(&tcp, SIGNAL(tcpDownstreamPacket(QPcapTcpPacket,QPcapTcpConversation)),
          &http, SLOT(tcpDownstreamPacket(QPcapTcpPacket,QPcapTcpConversation)));
  connect(&http, SIGNAL(discardDownstreamBuffer(QPcapTcpConversation)),
          &tcp, SLOT(discardDownstreamBuffer(QPcapTcpConversation)));
  connect(&http, SIGNAL(discardUpstreamBuffer(QPcapTcpConversation)),
          &tcp, SLOT(discardUpstreamBuffer(QPcapTcpConversation)));
  connect(&http, SIGNAL(httpHit(QPcapTcpConversation,QPcapHttpHit)),
          &p, SLOT(httpHit(QPcapTcpConversation,QPcapHttpHit)));
  connect(&http, SIGNAL(httpHit(QPcapTcpConversation,QPcapHttpHit)),
          &p, SLOT(httpHitToCsv(QPcapTcpConversation,QPcapHttpHit)));
  // the following quit condition would not work if stacks become multithreaded
  connect(&pe, SIGNAL(captureTerminated()), &a, SLOT(quit()));
  pe.start();
  return a.exec();
}
