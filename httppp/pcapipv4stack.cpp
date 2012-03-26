#include "pcapipv4stack.h"

PcapIPv4Stack::PcapIPv4Stack(QObject *parent) :
  QObject(parent)
{
}

void PcapIPv4Stack::fragmentedPacketReceived(PcapLayer2Packet packet) {
  Q_UNUSED(packet);
}
