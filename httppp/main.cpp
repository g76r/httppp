#include <QCoreApplication>
#include "pcapengine.h"
#include "printer.h"

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);
  PcapEngine pe(argc > 1 ? argv[1] : "sample.pcap");
  Printer p;
  QObject::connect(&pe, SIGNAL(layer1PacketReceived(PcapLayer1Packet)),
                   &p, SLOT(layer2PacketReceived(PcapLayer1Packet)));
  QObject::connect(&pe, SIGNAL(captureTerminated()), &a, SLOT(quit()));
  pe.start();
  return a.exec();
}
