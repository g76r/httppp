#ifndef PRINTER_H
#define PRINTER_H

#include <QObject>
#include "pcappacket.h"

class Printer : public QObject {
  Q_OBJECT
public:
  inline explicit Printer(QObject *parent = 0) : QObject(parent) { }
  
public slots:
  void packetReceived(PcapPacket packet);

private:
  Q_DISABLE_COPY(Printer)
};

#endif // PRINTER_H
