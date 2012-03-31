#ifndef PCAPTCPSTACK_H
#define PCAPTCPSTACK_H

#include <QObject>
#include <QByteArray>
#include "pcaptcpconversation.h"
#include "pcaptcppacket.h"
#include <QMultiMap>

class PcapTcpStack : public QObject {
  Q_OBJECT
private:
  QSet<PcapTcpConversation> _conversations;
  QMultiMap<PcapTcpConversation, PcapTcpPacket> _upstreamBuffer;
  QMultiMap<PcapTcpConversation, PcapTcpPacket> _downstreamBuffer;

public:
  explicit PcapTcpStack(QObject *parent = 0) : QObject(parent) { }

signals:
  /** Emitting ordered data stream chunk, client to server.
    * Beware: if SYN/SYN-ACK exchange was previous capture start, client and
    * server are randomly chosen (actually: first seen packet source is client).
    */
  void tcpUpstreamPacket(PcapTcpPacket packet,
                         PcapTcpConversation conversation);
  /** Emitting ordered data stream chunk, server to client.
    * Beware: if SYN/SYN-ACK exchange was previous capture start, client and
    * server are randomly chosen (actually: first seen packet source is client).
    */
  void tcpDownstreamPacket(PcapTcpPacket packet,
                           PcapTcpConversation conversation);

  void conversationStarted(PcapTcpConversation conversation);
  void conversationFinished(PcapTcpConversation conversation);

public slots:
  /** Receiving IP packet, potentially retransmitted or in wrong order.
    */
  void ipPacketReceived(PcapIPv4Packet packet);

private:
  void dispatchPacket(PcapTcpPacket packet, PcapTcpConversation conversation);

  Q_DISABLE_COPY(PcapTcpStack)
};

#endif // PCAPTCPSTACK_H
