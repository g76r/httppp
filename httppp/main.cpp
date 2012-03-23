#include <QCoreApplication>
#include "pcapengine.h"
#include "printer.h"

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);
  PcapEngine pe(argc > 1 ? argv[1] : "sample.pcap");
  Printer p;
  QObject::connect(&pe, SIGNAL(packetReceived(PcapPacket)),
                   &p, SLOT(packetReceived(PcapPacket)));
  QObject::connect(&pe, SIGNAL(captureTerminated()), &a, SLOT(quit()));
  pe.start();
  return a.exec();
}
