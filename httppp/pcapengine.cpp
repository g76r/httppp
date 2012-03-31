#include "pcapengine.h"
#include <QtDebug>
#include <QThread>
#include "pcapthread.h"
#include <QMetaType>
#include "pcaplayer2packet.h"

void PcapEngine::init() {
  _thread = new PcapThread(this);
  moveToThread(_thread);
  qRegisterMetaType<PcapLayer1Packet>("PcapLayer1Packet");
  qRegisterMetaType<PcapLayer2Packet>("PcapLayer2Packet");
  connect(_thread, SIGNAL(finished()), this, SIGNAL(captureTerminated()));
}


PcapEngine::PcapEngine() : _pcap(0) {
  init();
}

PcapEngine::PcapEngine(QString filename) : _pcap(0) {
  init();
  loadFile(filename);
}

void PcapEngine::start() {
  _thread->start();
}

void PcapEngine::loadFile(QString filename) {
  char errbuf[PCAP_ERRBUF_SIZE];
  if (_pcap) {
    pcap_close(_pcap);
    _pcap = 0;
  }
  _pcap = pcap_open_offline(filename.toUtf8(), errbuf);
  if (_pcap)
    _filename = filename;
  else {
    qDebug() << "pcap_open_offline" << filename << "failed:" << errbuf;
    _filename = QString();
  }
}

void PcapEngine::packetHandler(const struct pcap_pkthdr* pkthdr,
                               const u_char* packet) {
  //qDebug() << "PcapEngine::packetHandler" << (long)pkthdr << (long)packet;
  //QByteArray ba((const char *)packet, pkthdr->caplen);
  //qDebug() << "PcapEngine::packetHandler" //<< (long)pkthdr << (long)packet
  //         << ba.toHex();
  PcapLayer1Packet pp(pkthdr, packet);
  //qDebug() << pp;
  emit layer1PacketReceived(pp);
}

void PcapEngine::callback(u_char *user, const struct pcap_pkthdr* pkthdr,
                          const u_char* packet) {
  //qDebug() << "PcapEngine::callback";
  reinterpret_cast<PcapEngine*>(user)->packetHandler(pkthdr, packet);
}
