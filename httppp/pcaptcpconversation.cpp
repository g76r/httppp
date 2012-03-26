#include "pcaptcpconversation.h"
#include <QAtomicInt>

static QAtomicInt _conversationCounter = 1;

PcapTcpConversationData::PcapTcpConversationData(PcapTcpPacket firstPacket)
  : QSharedData(), _id(_conversationCounter.fetchAndAddRelaxed(1)),
    _firstPacket(firstPacket), _nextUpstreamNumber(firstPacket.seqNumber()),
    _nextDownstreamNumber(0), _numbersInitialized(false) {
}
