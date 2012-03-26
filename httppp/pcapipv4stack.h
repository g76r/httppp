#ifndef PCAPIPV4STACK_H
#define PCAPIPV4STACK_H

#include <QObject>
#include "pcapipv4packet.h"
#include "pcaplayer2packet.h"

class PcapIPv4Stack : public QObject {
  Q_OBJECT
public:
  explicit PcapIPv4Stack(QObject *parent = 0);
  
signals:
  void wholePacketReceived(PcapIPv4Packet packet);

public slots:
  void fragmentedPacketReceived(PcapLayer2Packet packet);

private:
  Q_DISABLE_COPY(PcapIPv4Stack)
};

#endif // PCAPIPV4STACK_H
