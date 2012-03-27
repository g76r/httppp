#include "pcaptcpstack.h"

void PcapTcpStack::ipPacketReceived(PcapIPv4Packet packet) {
  if (packet.layer4Proto() != PcapIPv4Packet::TCP) {
    return; // ignoring non-tcp packets
  }
  PcapTcpPacket tcp(packet);
  if (tcp.isNull()) {
    qDebug() << "   ignoring malformed tcp packet";
    return;
  }
  foreach (PcapTcpConversation c, _conversations) {
    if (c.matchesEitherStream(tcp)) {
      //qDebug() << c.id() << "   received established tcp packet" << tcp << c.nextUpstreamNumber() << c.nextDownstreamNumber();
      dispatchPacket(tcp, c);
      return;
    }
  }
  if (tcp.fin() || tcp.rst()) {
    qDebug() << "   ignoring fin or rst new tcp packet" << tcp << tcp.fin() << tcp.rst();
    return;
  }
  PcapTcpConversation c(tcp);
  //qDebug() << c.id() << "   received new tcp packet" << tcp;
  _conversations.insert(c);
  dispatchPacket(tcp, c);
}

void PcapTcpStack::dispatchPacket(PcapTcpPacket packet,
                                  PcapTcpConversation conversation) {
  if (conversation.matchesSameStream(packet)) {
    // upstream packet (client to server)
    //qDebug() << "  upstream" << conversation.id() << packet.seqNumber() << conversation.nextUpstreamNumber() << packet.payload().size();
    if (packet.seqNumber() == conversation.nextUpstreamNumber()) {
      emit tcpUpstreamPacket(packet, conversation);
      conversation.nextUpstreamNumber() +=
          conversation.numbersInitialized() || !packet.syn() || packet.ack()
          ? packet.payload().size() : 1;
      // FIXME scan upstream packets if any
    } else {
      if ((qint32)(packet.seqNumber()-conversation.nextUpstreamNumber()) < 0) {
        // retransmission of already treated packet: nothing to do
        if (conversation.nextUpstreamNumber()-(qint32)(packet.seqNumber() == 1))
            qDebug() << conversation.id() << "KK>" << packet; // probable keepalive
        else
            qDebug() << conversation.id() << "RR>" << packet;
      } else {
        qDebug() << conversation.id() << "-->" << packet;
        // FIXME
        //_upstreamBuffer.insertMulti(conversation, packet);
      }
    }
  } else {
    // downstream packet (server to client)
    if (!conversation.numbersInitialized()) {
      conversation.nextDownstreamNumber() = packet.seqNumber();
    }
    //qDebug() << "  downstream" << conversation.id() << packet.seqNumber() << conversation.nextDownstreamNumber() << packet.payload().size();
    if (packet.seqNumber() == conversation.nextDownstreamNumber()) {
      emit tcpDownstreamPacket(packet, conversation);
      conversation.nextDownstreamNumber() +=
          conversation.numbersInitialized() || !packet.syn() || !packet.ack()
          ? packet.payload().size() : 1;
      // FIXME scan downstream packets if any
    } else {
      if ((qint32)(packet.seqNumber()-conversation.nextDownstreamNumber()) < 0){
        // retransmission of already treated packet: nothing to do
        if (conversation.nextDownstreamNumber()-(qint32)(packet.seqNumber()==1))
          qDebug() << conversation.id() << "KK>" << packet; // probable keepalive
        else
          qDebug() << conversation.id() << "<RR" << packet;
      } else {
        qDebug() << conversation.id() << "<--" << packet;
        // FIXME
        //_downstreamBuffer.insertMulti(conversation, packet);
      }
    }
    if (!conversation.numbersInitialized()) {
      conversation.numbersInitialized() = true;
    }
  }
  // FIXME handle TCP deduplication and reordering
  // FIXME remove TCP conversation after fin-ack
  if (packet.rst() || packet.fin()) {
    qDebug() << conversation.id() << "XXX";
    _conversations.remove(conversation);
    // FIXME
    //_upstreamBuffer.remove(conversation); // should be useless
    //_downstreamBuffer.remove(conversation); // should be useless
  }
}
