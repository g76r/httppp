#ifndef PCAPTCPCONVERSATION_H
#define PCAPTCPCONVERSATION_H

#include <QSharedData>
#include <QtDebug>
#include "pcaptcppacket.h"

class PcapTcpConversation;

class PcapTcpConversationData : public QSharedData {
  friend class PcapTcpConversation;
private:
  quint64 _id;
  PcapTcpPacket _firstPacket;
  quint32 _nextUpstreamNumber;
  quint32 _nextDownstreamNumber;
  bool _numbersInitialized;

public:
  PcapTcpConversationData(PcapTcpPacket firstPacket = PcapTcpPacket());
  inline PcapTcpConversationData(const PcapTcpConversationData &other)
    : QSharedData(), _id(other._id), _firstPacket(other._firstPacket),
      _nextUpstreamNumber(other._nextUpstreamNumber),
      _nextDownstreamNumber(other._nextDownstreamNumber),
      _numbersInitialized(other._numbersInitialized) { }
};

class PcapTcpConversation {
private:
  QExplicitlySharedDataPointer<PcapTcpConversationData> d;

public:
  inline PcapTcpConversation(PcapTcpPacket firstPacket = PcapTcpPacket()) {
    d = new PcapTcpConversationData(firstPacket); }
  inline PcapTcpConversation(const PcapTcpConversation &other) : d(other.d) { }
  inline quint32 id() const { return d->_id; }
  inline PcapTcpPacket firstPacket() const { return d->_firstPacket; }
  inline QString english() const {
    return QString("PcapTcpConversation(%1, %2, %3)").arg(d->_id)
        .arg(d->_firstPacket.src()).arg(d->_firstPacket.dst()); }
  inline bool matchesEitherStream(PcapTcpPacket packet) {
    return (d->_firstPacket.srcPort() == packet.srcPort()
            && d->_firstPacket.dstPort() == packet.dstPort()
            && d->_firstPacket.ip().srcAsInt() == packet.ip().srcAsInt()
            && d->_firstPacket.ip().dstAsInt() == packet.ip().dstAsInt())
        || (d->_firstPacket.dstPort() == packet.srcPort()
            && d->_firstPacket.srcPort() == packet.dstPort()
            && d->_firstPacket.ip().srcAsInt() == packet.ip().dstAsInt()
            && d->_firstPacket.ip().dstAsInt() == packet.ip().srcAsInt()); }
  inline bool matchesSameStream(PcapTcpPacket packet) {
    return d->_firstPacket.srcPort() == packet.srcPort()
        && d->_firstPacket.dstPort() == packet.dstPort()
        && d->_firstPacket.ip().srcAsInt() == packet.ip().srcAsInt()
        && d->_firstPacket.ip().dstAsInt() == packet.ip().dstAsInt(); }
  inline quint32 &nextUpstreamNumber() { return d->_nextUpstreamNumber; }
  inline quint32 &nextDownstreamNumber() { return d->_nextDownstreamNumber; }
  inline bool &numbersInitialized() { return d->_numbersInitialized; }
  inline bool operator ==(const PcapTcpConversation &other) const {
    return d == other.d; }
  inline bool operator <(const PcapTcpConversation &other) const {
    return id() < other.id(); }
};

inline QDebug operator<<(QDebug dbg, const PcapTcpConversation &o) {
  dbg.nospace() << o.english();
  return dbg.space();
}

inline uint qHash(const PcapTcpConversation &o) {
  return o.id();
}

#endif // PCAPTCPCONVERSATION_H
