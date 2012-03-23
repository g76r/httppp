#include "pcapthread.h"
#include <QtDebug>
#include <pcap/pcap.h>
#include "pcapengine.h"
#include <QCoreApplication>

PcapThread::PcapThread(PcapEngine *engine, QObject *parent)
  : QThread(parent), _engine(engine) {
}

void PcapThread::run() {
  forever {
    int rc = pcap_dispatch(_engine->_pcap, 32, _engine->callback,
                           reinterpret_cast<u_char*>(_engine));
    QCoreApplication::processEvents();
    if (rc <= 0)
      break;
  }
  _engine->moveToThread(QCoreApplication::instance()->thread());
}
