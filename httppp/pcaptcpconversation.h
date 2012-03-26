#ifndef PCAPTCPCONVERSATION_H
#define PCAPTCPCONVERSATION_H

#include <QSharedData>
#include <QtDebug>
#include "pcaptcppacket.h"

class PcapTcpConversationData : public QSharedData {
private:
  quint64 _id;
  PcapTcpPacket _firstPacket;

public:
  PcapTcpConversationData(PcapTcpPacket firstPacket = PcapTcpPacket());
  inline PcapTcpConversationData(const PcapTcpConversationData &other)
    : QSharedData(), _id(other._id), _firstPacket(other._firstPacket) { }
  inline quint32 id() const { return _id; }
  inline PcapTcpPacket firstPacket() const { return _firstPacket; }
  inline QString english() const {
    return QString("PcapTcpConversation(%1, %2, %3)").arg(_id)
        .arg(_firstPacket.src()).arg(_firstPacket.dst());
  }
};

class PcapTcpConversation {
private:
  QSharedDataPointer<PcapTcpConversationData> d;

public:
  inline PcapTcpConversation() { d = new PcapTcpConversationData(); }
  inline PcapTcpConversation(const PcapTcpConversation &other) : d(other.d) { }
  inline quint32 id() const { return d->id(); }
  inline PcapTcpPacket firstPacket() const { return d->firstPacket(); }
  inline QString english() const { return d->english(); }
};

inline QDebug operator<<(QDebug dbg, const PcapTcpConversation &pp) {
  dbg.nospace() << pp.english();
  return dbg.space();
}
#endif // PCAPTCPCONVERSATION_H
