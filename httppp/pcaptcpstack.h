#ifndef PCAPTCPSTACK_H
#define PCAPTCPSTACK_H

#include <QObject>
#include <QByteArray>
#include "pcaptcpconversation.h"
#include "pcaptcppacket.h"

class PcapTcpStack : public QObject {
  Q_OBJECT
public:
  explicit PcapTcpStack(QObject *parent = 0) : QObject(parent) { }

signals:
  /** Emitting ordered data stream chunk, client to server.
    * Beware: if SYN/SYN-ACK exchange was previous capture start, client and
    * server are randomly chosen (actually: first seen packet source is client).
    */
  void tcpUpstreamChunk(PcapTcpPacket packet,
                        PcapTcpConversation conversation);
  /** Emitting ordered data stream chunk, server to client.
    * Beware: if SYN/SYN-ACK exchange was previous capture start, client and
    * server are randomly chosen (actually: first seen packet source is client).
    */
  void tcpDownstreamChunk(PcapTcpPacket packet,
                          PcapTcpConversation conversation);

public slots:
  /** Receiving IP packet, potentially retransmitted or in wrong order.
    */
  void ipPacketReceived(PcapIPv4Packet packet);

private:
  Q_DISABLE_COPY(PcapTcpStack)
};

#endif // PCAPTCPSTACK_H
