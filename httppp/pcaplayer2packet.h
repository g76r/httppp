#ifndef PCAPLAYER2PACKET_H
#define PCAPLAYER2PACKET_H

#include <QSharedData>
#include <QtDebug>
#include <QByteArray>

class PcapLayer2PacketData : public QSharedData {
protected:
  quint16 _layer2Proto;
  QByteArray _payload;

public:
  inline explicit PcapLayer2PacketData(
      quint16 proto = 0, QByteArray payload = QByteArray())
    : _layer2Proto(proto), _payload(payload) { }
  inline PcapLayer2PacketData(const PcapLayer2PacketData &other)
    : QSharedData(), _layer2Proto(other._layer2Proto),
      _payload(other._payload) { }
  inline quint16 layer2Proto() const { return _layer2Proto; }
  inline QByteArray payload() const { return _payload; }
  virtual QString english() const;
};

class PcapLayer2Packet {
protected:
  QSharedDataPointer<PcapLayer2PacketData> d;
  explicit inline PcapLayer2Packet(PcapLayer2PacketData *data) {
    d = data ? data : new PcapLayer2PacketData(); }

public:
  enum Layer2Proto { Unknown = 0, EthernetII };

  explicit inline PcapLayer2Packet() : d(new PcapLayer2PacketData()) { }
  inline PcapLayer2Packet(const PcapLayer2Packet &other) : d(other.d) { }
  inline quint16 layer2Proto() const { return d->layer2Proto(); }
  inline QByteArray payload() const { return d->payload(); }
  inline bool isNull() const { return payload().isNull(); }
  inline QString english() const { return d->english(); }
};

inline QDebug operator<<(QDebug dbg, const PcapLayer2Packet &pp) {
  dbg.nospace() << pp.english();
  return dbg.space();
}

#endif // PCAPLAYER2PACKET_H