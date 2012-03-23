#include "printer.h"
#include <QtDebug>

void Printer::packetReceived(PcapPacket packet) {
  qDebug() << packet;
}
