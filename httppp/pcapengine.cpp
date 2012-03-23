#include "pcapengine.h"
#include <QtDebug>
#include <QThread>
#include "pcapthread.h"
#include <QMetaType>

PcapEngine::PcapEngine(QString filename) : _pcap(0), _filename(filename) {
  _thread = new PcapThread(this);
  moveToThread(_thread);
  qRegisterMetaType<PcapPacket>("PcapPacket");
  connect(_thread, SIGNAL(finished()), this, SIGNAL(captureTerminated()));
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
  }
}

void PcapEngine::packetHandler(const struct pcap_pkthdr* pkthdr,
                               const u_char* packet) {
  //qDebug() << "PcapEngine::packetHandler" << (long)pkthdr << (long)packet;
  //QByteArray ba((const char *)packet, pkthdr->caplen);
  //qDebug() << "PcapEngine::packetHandler" //<< (long)pkthdr << (long)packet
  //         << ba.toHex();
  PcapPacket pp(pkthdr, packet);
  //qDebug() << pp;
  emit packetReceived(pp);
}

void PcapEngine::callback(u_char *user, const struct pcap_pkthdr* pkthdr,
                          const u_char* packet) {
  //qDebug() << "PcapEngine::callback";
  reinterpret_cast<PcapEngine*>(user)->packetHandler(pkthdr, packet);
}
