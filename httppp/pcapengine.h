#ifndef PCAPFILE_H
#define PCAPFILE_H

#include <QString>
#include <QObject>
#include <pcap/pcap.h>
#include "pcaplayer1packet.h"

class PcapThread;

class PcapEngine : public QObject {
  Q_OBJECT
  friend class PcapThread;
private:
  pcap_t *_pcap;
  QString _filename;
  PcapThread *_thread;

public:
  PcapEngine();
  PcapEngine(QString filename);
  void loadFile(QString filename);
  void start();

signals:
  void captureTerminated();
  void layer1PacketReceived(PcapLayer1Packet packet);

private:
  void init();
  void packetHandler(const struct pcap_pkthdr* pkthdr,
                     const u_char* packet);
  static void callback(u_char *user, const struct pcap_pkthdr* pkthdr,
                       const u_char* packet);

  Q_DISABLE_COPY(PcapEngine)
};

#endif // PCAPFILE_H
