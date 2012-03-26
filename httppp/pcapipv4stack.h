#ifndef PCAPIPV4STACK_H
#define PCAPIPV4STACK_H

#include <QObject>
#include "pcapipv4packet.h"
#include "pcaplayer2packet.h"

class PcapIPv4Stack : public QObject {
  Q_OBJECT
public:
  inline explicit PcapIPv4Stack(QObject *parent = 0) : QObject(parent) { }

signals:
  /** Emititng generic layer 3 defragmented packet.
    */
  void layer3PacketReceived(PcapLayer3Packet packet);
  /** Emititng IPv4 defragmented packet.
    */
  void ipv4PacketReceived(PcapIPv4Packet packet);

public slots:
  /** Receiving layer 2 packet, potentially fragmented.
    */
  void layer2PacketReceived(PcapLayer2Packet packet);

private:
  Q_DISABLE_COPY(PcapIPv4Stack)
};

#endif // PCAPIPV4STACK_H
