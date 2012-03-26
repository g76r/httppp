#ifndef PCAPETHERNETSTACK_H
#define PCAPETHERNETSTACK_H

#include <QObject>
#include "pcapethernetpacket.h"
#include "pcaplayer1packet.h"

class PcapEthernetStack : public QObject {
  Q_OBJECT
public:
  explicit PcapEthernetStack(QObject *parent = 0) : QObject(parent) { }

signals:
  /** Emititng layer 2 packet.
    */
  void layer2PacketReceived(PcapLayer2Packet packet);
  //void ethernetPacketReceived(PcapEthernetPacket packet);

public slots:
  /** Receiving layer 1 packet, potentially not ethernet.
    */
  void layer1PacketReceived(PcapLayer1Packet packet);

private:
  Q_DISABLE_COPY(PcapEthernetStack)
 };

#endif // PCAPETHERNETSTACK_H
