#ifndef PRINTER_H
#define PRINTER_H

#include <QObject>
#include "qpcaplayer1packet.h"
#include "qpcaplayer2packet.h"
#include "qpcaplayer3packet.h"
#include "qpcaptcppacket.h"
#include "qpcaptcpconversation.h"
#include "qpcaphttphit.h"

class Printer : public QObject {
  Q_OBJECT
public:
  inline explicit Printer(QObject *parent = 0) : QObject(parent) { }
  
public slots:
  void layer1PacketReceived(QPcapLayer1Packet packet);
  void layer2PacketReceived(QPcapLayer2Packet packet);
  void layer3PacketReceived(QPcapLayer3Packet packet);
  void tcpUpstreamPacket(QPcapTcpPacket packet,
                         QPcapTcpConversation conversation);
  void tcpDownstreamPacket(QPcapTcpPacket packet,
                           QPcapTcpConversation conversation);
  void httpHit(QPcapTcpConversation conversation, QPcapHttpHit hit);

private:
  Q_DISABLE_COPY(Printer)
};

#endif // PRINTER_H
