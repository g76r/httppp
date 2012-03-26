#ifndef PCAPIPV4PACKET_H
#define PCAPIPV4PACKET_H

#include <QtDebug>
#include "pcaplayer2packet.h"
#include "pcaplayer3packet.h"

class PcapIPv4PacketData : public PcapLayer3PacketData {
protected:
  quint8/*4*/ _version;
  quint8/*4*/ _headerSize; // in 32 bits words
  quint8 _tos;
  quint16 _totalSize;
  quint16 _identification;
  bool _unusedFlag;
  bool _dontFragment;
  bool _moreFragments; // this packet is not the last fragment
  quint16/*13*/ _fragmentOffset; // 64 bits words
  quint8 _ttl;
  quint8 _layer4Proto;
  quint16 _headerChecksum;
  quint8 _src[4];
  quint8 _dst[4];

  inline void reset() {
    _version = _headerSize = _tos = _totalSize = _identification
        = _fragmentOffset = _ttl = _layer4Proto = _headerChecksum = 0;
    _unusedFlag = _dontFragment = _moreFragments = false;
    ::memset(_src, 0, 4);
    ::memset(_dst, 0, 4);
    _payload.clear();
  }

public:
  /** Construct an IP packet fragment from a layer 2 frame.
    */
  PcapIPv4PacketData(const PcapLayer2Packet &packet);
  inline PcapIPv4PacketData(const PcapIPv4PacketData &other)
    : PcapLayer3PacketData(other.layer3Proto(), other._payload),
      _version(other._version), _headerSize(other._headerSize),
      _tos(other._tos), _totalSize(other._totalSize),
      _identification(other._identification), _unusedFlag(other._unusedFlag),
      _dontFragment(other._dontFragment), _moreFragments(other._moreFragments),
      _fragmentOffset(other._fragmentOffset), _ttl(other._ttl),
      _layer4Proto(other._layer4Proto), _headerChecksum(other._headerChecksum) {
    ::memcpy(_src, other._src, 4);
    ::memcpy(_dst, other._dst, 4);
  }
  inline quint16 layer3Proto() const { return _layer3Proto; }
  QString english() const;
  inline quint16 totalSize() const { return _totalSize; }
  inline quint16 identification() const { return _identification; }
  inline bool dontFragment() const { return _dontFragment; }
  inline bool moreFragments() const { return _moreFragments; }
  /** @return in 64 bits words as in packet header
    */
  inline quint16 fragmentOffset() const { return _fragmentOffset; }
  inline QString src() const {
    return QString("%1.%2.%3.%4")
        .arg(_src[0]).arg(_src[1]).arg(_src[2]).arg(_src[3]);
  }
  inline QString dst() const {
    return QString("%1.%2.%3.%4")
        .arg(_dst[0]).arg(_dst[1]).arg(_dst[2]).arg(_dst[3]);
  }
  inline quint32 layer4Proto() const { return _layer4Proto; }
  /** @return in bytes (= actual header field * 4)
    */
  inline quint32 headerSizeInBytes() const {
    return _headerSize*4;
  }
  /** @return in 4 bytes words count (= bytes / 4)
    */
  inline quint32 headerSize() const {
    return _headerSize;
  }
};

class PcapIPv4Packet : public PcapLayer3Packet {
public:
  enum Layer4Protocol { ICMP = 1, IGMP = 2, IPv4 = 4, TCP = 6, UDP = 17, IPv6 = 41,
                        RSVP = 46, GRE = 47, IPIP = 94, IPX = 111, VRRP = 112,
                        L2TP = 115, SCTP = 132, FC = 133, UDPLite = 136,
                        MPLS = 137 };

  inline explicit PcapIPv4Packet(const PcapLayer2Packet &packet
                                 = PcapLayer2Packet())
    : PcapLayer3Packet(new PcapIPv4PacketData(packet)) { }
  inline PcapIPv4Packet(const PcapIPv4Packet &other)
    : PcapLayer3Packet(other) { }
  inline quint16 totalSize() const { return static_cast<const PcapIPv4PacketData*>(d.constData())->totalSize(); }
  inline quint16 identification() const { return static_cast<const PcapIPv4PacketData*>(d.constData())->identification(); }
  inline bool dontFragment() const { return static_cast<const PcapIPv4PacketData*>(d.constData())->dontFragment(); }
  inline bool moreFragments() const { return static_cast<const PcapIPv4PacketData*>(d.constData())->moreFragments(); }
  /** @return in 64 bits words as in packet header
    */
  inline quint16 fragmentOffset() const { return static_cast<const PcapIPv4PacketData*>(d.constData())->fragmentOffset(); }
  inline quint16 layer4Proto() const { return static_cast<const PcapIPv4PacketData*>(d.constData())->layer4Proto(); }
  inline QString src() const { return static_cast<const PcapIPv4PacketData*>(d.constData())->src(); }
  inline QString dst() const { return static_cast<const PcapIPv4PacketData*>(d.constData())->dst(); }
};

#endif // PCAPIPV4PACKET_H
