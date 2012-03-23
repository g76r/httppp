#ifndef PCAPPACKET_H
#define PCAPPACKET_H

#include <QSharedData>
#include <QtDebug>
#include <pcap/pcap.h>
#include <QByteArray>
#include <QDateTime>

class PcapEngine;

class PcapPacketData : public QSharedData {
private:
  quint64 _timestamp; // in microseconds since 1970
  quint32 _wirelen;
  QByteArray _payload;

public:
  inline PcapPacketData() : _timestamp(0), _wirelen(0) { }
  inline PcapPacketData(const PcapPacketData &other)
    : QSharedData(), _timestamp(other._timestamp), _wirelen(other._wirelen),
      _payload(other._payload) {
  }
  inline PcapPacketData(const struct pcap_pkthdr* pkthdr, const u_char* packet)
    : _timestamp(pkthdr->ts.tv_usec + pkthdr->ts.tv_sec*(1000000LL)),
      _wirelen(pkthdr->len), _payload((const char*)packet, pkthdr->caplen) {
  }
  inline quint64 usecSince1970() const { return _timestamp; }
  inline QDateTime timestamp() const {
    return QDateTime::fromMSecsSinceEpoch(_timestamp/1000);
  }
  inline quint32 wirelen() const { return _wirelen; }
  inline QByteArray payload() const { return _payload; }
  inline QString english() const {
    return QString("PcapPacket(%1, %2, %3, %4)").arg(_timestamp).arg(_wirelen)
        .arg(_payload.size()).arg(_payload.toHex().constData());
  }
};

class PcapPacket {
  friend class PcapEngine;
private:
  QSharedDataPointer<PcapPacketData> d;

  inline PcapPacket(const struct pcap_pkthdr* pkthdr,
                    const u_char* packet) {
    d = new PcapPacketData(pkthdr, packet);
  }

public:
  inline PcapPacket() { d = new PcapPacketData(); }
  inline PcapPacket(const PcapPacket &other) : d(other.d) { }
  inline QDateTime timestamp() const { return d->timestamp(); }
  inline quint64 usecSince1970() const { return d->usecSince1970(); }
  inline quint32 wirelen() const { return d->wirelen(); }
  inline QByteArray payload() const { return d->payload(); }
  inline QString english() const { return d->english(); }
};

inline QDebug operator<<(QDebug dbg, const PcapPacket &pp) {
  dbg.nospace() << pp.english();
  return dbg.space();
}

#endif // PCAPPACKET_H

