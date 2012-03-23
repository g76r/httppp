#ifndef PCAPIPV4PACKET_H
#define PCAPIPV4PACKET_H

#include <QObject>
#include <QtDebug>

class PcapIPv4Packet {
public:
  enum IpProtocol { ICMP = 1, IGMP = 2, IPv4 = 4, TCP = 6, UDP = 17, IPv6 = 41,
                    RSVP = 46, GRE = 47, IPIP = 94, IPX = 111, VRRP = 112,
                    L2TP = 115, SCTP = 132, FC = 133, UDPLite = 136,
                    MPLS = 137 };
private:
  quint32 _payloadSize;
  const quint8 *_payload;
  quint8/*4*/ _version;
  quint8/*4*/ _headerSize; // in 32 bits words
  quint8 _tos;
  quint16 _totalSize;
  quint16 _identification;
  bool _unusedFlag;
  bool _dontFragment;
  bool _moreFragments; // this packet is not the last fragment
  quint16/*13*/ _fragmentOffset;
  quint8 _ttl;
  quint8 _proto;
  quint16 _headerChecksum;
  quint8 _src[4];
  quint8 _dst[4];

  inline void reset() {
    _payloadSize = _version = _headerSize = _tos = _totalSize = _identification
        = _fragmentOffset = _ttl = _proto = _headerChecksum = 0;
    _unusedFlag = _dontFragment = _moreFragments = false;
    ::memset(_src, 0, 4);
    ::memset(_dst, 0, 4);
  }

public:
  inline PcapIPv4Packet(quint32 size, const quint8 *data) {
    if (size < 20) {
      reset();
      return;
    }
    _version = data[0] >> 4;
    _headerSize = data[0] & 0xf;
    _tos = data[1];
    _totalSize = (data[2] << 8) + data[3];
    _identification = (data[4] << 8) + data[5];
    // TODO flags and fragmentoffset
    _ttl = data[8];
    _proto = data[9];
    _headerChecksum = (data[10] << 8) + data[11];
    ::memccpy(_src, data+12, 4, 1);
    ::memccpy(_dst, data+16, 4, 1);
    //qDebug() << "PcapIPv4Packet" << size << _headerSize
    //         << _version;
    if (size < _headerSize*4 || _headerSize*4 < 20) {
      reset();
      return;
    }
    _payload = data+_headerSize*4;
    _payloadSize = size - _headerSize*4;
  }
  inline bool isNull() const { return !_payload; }
  inline quint32 payloadSize() const { return _payloadSize; }
  inline const quint8 *payload() const { return _payload; }
  inline QString src() const {
    return QString("%1.%2.%3.%4")
        .arg(_src[0]).arg(_src[1]).arg(_src[2]).arg(_src[3]);
  }
  inline QString dst() const {
    return QString("%1.%2.%3.%4")
        .arg(_dst[0]).arg(_dst[1]).arg(_dst[2]).arg(_dst[3]);
  }
  inline quint32 proto() const { return _proto; }
  /** @return in bytes (= actual header field * 4)
    */
  inline quint32 headerSize() const {
    return _headerSize*4;
  }
  inline QByteArray payloadToByteArray() const {
    // LATER avoid deep copy
    return QByteArray((const char*)_payload, _payloadSize);
  }
};

#endif // PCAPIPV4PACKET_H
