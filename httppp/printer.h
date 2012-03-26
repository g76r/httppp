#ifndef PRINTER_H
#define PRINTER_H

#include <QObject>
#include "pcaplayer1packet.h"
#include "pcaplayer2packet.h"
#include "pcaplayer3packet.h"
#include "pcaptcppacket.h"
#include "pcaptcpconversation.h"

class Printer : public QObject {
  Q_OBJECT
public:
  inline explicit Printer(QObject *parent = 0) : QObject(parent) { }
  
public slots:
  void layer1PacketReceived(PcapLayer1Packet packet);
  void layer2PacketReceived(PcapLayer2Packet packet);
  void layer3PacketReceived(PcapLayer3Packet packet);
  void tcpUpstreamPacket(PcapTcpPacket packet,
                         PcapTcpConversation conversation);
  void tcpDownstreamPacket(PcapTcpPacket packet,
                           PcapTcpConversation conversation);

private:
  Q_DISABLE_COPY(Printer)
};

#endif // PRINTER_H
