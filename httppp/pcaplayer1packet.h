#ifndef PCAPPACKET_H
#define PCAPPACKET_H

#include <QSharedData>
#include <QtDebug>
#include <pcap/pcap.h>
#include <QByteArray>
#include <QDateTime>

class PcapEngine;

class PcapLayer1PacketData : public QSharedData {
private:
  quint64 _timestamp; // in microseconds since 1970
  quint32 _wirelen;
  QByteArray _payload;

public:
  inline PcapLayer1PacketData() : _timestamp(0), _wirelen(0) { }
  inline PcapLayer1PacketData(const PcapLayer1PacketData &other)
    : QSharedData(), _timestamp(other._timestamp), _wirelen(other._wirelen),
      _payload(other._payload) {
  }
  inline PcapLayer1PacketData(const struct pcap_pkthdr* pkthdr, const u_char* packet)
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
    return QString("PcapPacket(%1, %2, %3, %4)")
        .arg(timestamp().time().toString("HH:mm:ss,zzz")).arg(_wirelen)
        .arg(_payload.size()).arg(_payload.toHex().constData());
  }
};

class PcapLayer1Packet {
  friend class PcapEngine;
private:
  QSharedDataPointer<PcapLayer1PacketData> d;

  inline PcapLayer1Packet(const struct pcap_pkthdr* pkthdr,
                    const u_char* packet) {
    d = new PcapLayer1PacketData(pkthdr, packet);
  }

public:
  inline PcapLayer1Packet() { d = new PcapLayer1PacketData(); }
  inline PcapLayer1Packet(const PcapLayer1Packet &other) : d(other.d) { }
  inline QDateTime timestamp() const { return d->timestamp(); }
  inline quint64 usecSince1970() const { return d->usecSince1970(); }
  inline quint32 wirelen() const { return d->wirelen(); }
  inline QByteArray payload() const { return d->payload(); }
  inline QString english() const { return d->english(); }
};

inline QDebug operator<<(QDebug dbg, const PcapLayer1Packet &pp) {
  dbg.nospace() << pp.english();
  return dbg.space();
}

#endif // PCAPPACKET_H
