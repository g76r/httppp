#ifndef PCAPETHERNETPACKET_H
#define PCAPETHERNETPACKET_H

#include "pcappacket.h"

#define ETHERTYPE_PUP           0x0200          /* Xerox PUP */
#define ETHERTYPE_SPRITE        0x0500          /* Sprite */
#define ETHERTYPE_IP            0x0800          /* IP */
#define ETHERTYPE_ARP           0x0806          /* Address resolution */
#define ETHERTYPE_REVARP        0x8035          /* Reverse ARP */
#define ETHERTYPE_AT            0x809B          /* AppleTalk protocol */
#define ETHERTYPE_AARP          0x80F3          /* AppleTalk ARP */
#define ETHERTYPE_VLAN          0x8100          /* IEEE 802.1Q VLAN tagging */
#define ETHERTYPE_IPX           0x8137          /* IPX */
#define ETHERTYPE_IPV6          0x86dd          /* IP protocol version 6 */
#define ETHERTYPE_LOOPBACK      0x9000          /* used to test interfaces */

/** Ethernet II packet dissector.
  */
class PcapEthernetPacket {
private:
  quint32 _payloadSize;
  const quint8 *_payload;
  quint8 _dst[6];
  quint8 _src[6];
  quint16 _proto;

public:
  inline PcapEthernetPacket(quint32 size, const quint8 *data) {
    if (size < 18) {
      ::memset(_dst, 0 , 6);
      ::memset(_src, 0 , 6);
      _proto = _payloadSize = 0;
      _payload = 0;
    } else {
      ::memcpy(_dst, data, 6);
      ::memcpy(_src, data+6, 6);
      _proto = (data[12] << 8) + data[13];
      _payload = data+14;
      _payloadSize = size-18;
      // LATER do something with 4 last bytes CRC
      // LATER handle LLC frames
    }
  }
  inline bool isNull() const { return !_payload; }
  inline quint32 payloadSize() const { return _payloadSize; }
  inline const quint8 *payload() const { return _payload; }
  inline QString dst() const {
    return QString("%1:%2:%3:%4:%5:%6").arg(_dst[0], 2, 16, QLatin1Char('0'))
        .arg(_dst[1], 2, 16, QLatin1Char('0'))
        .arg(_dst[2], 2, 16, QLatin1Char('0'))
        .arg(_dst[3], 2, 16, QLatin1Char('0'))
        .arg(_dst[4], 2, 16, QLatin1Char('0'))
        .arg(_dst[5], 2, 16, QLatin1Char('0'));
  }
  inline QString src() const {
    return QString("%1:%2:%3:%4:%5:%6").arg(_src[0], 2, 16, QLatin1Char('0'))
        .arg(_src[1], 2, 16, QLatin1Char('0'))
        .arg(_src[2], 2, 16, QLatin1Char('0'))
        .arg(_src[3], 2, 16, QLatin1Char('0'))
        .arg(_src[4], 2, 16, QLatin1Char('0'))
        .arg(_src[5], 2, 16, QLatin1Char('0'));
  }
  inline quint16 proto() const { return _proto; }
  inline QByteArray payloadToByteArray() const {
    // LATER avoid deep copy
    return QByteArray((const char*)_payload, _payloadSize);
  }

private:
  Q_DISABLE_COPY(PcapEthernetPacket)
};

#endif // PCAPETHERNETPACKET_H
