#include "tcpdata.h"
#include <QMetaObject>

TcpData::TcpData(QObject *parent) : QObject(parent) {
  qRegisterMetaType<QList<Conversation*> >("QList<Conversation*>");
}

TcpData::~TcpData() {
  qDeleteAll(_conversations);
}

void TcpData::connectToSource(QPcapTcpStack *stack) {
  connect(stack, SIGNAL(conversationStarted(QPcapTcpConversation)),
          this, SLOT(addConversation(QPcapTcpConversation)));
  connect(stack, SIGNAL(conversationFinished(QPcapTcpConversation)),
          this, SLOT(setConversationFinished(QPcapTcpConversation)));
  connect(stack, SIGNAL(tcpPacket(QPcapTcpPacket,QPcapTcpConversation)),
          this, SLOT(addPacket(QPcapTcpPacket,QPcapTcpConversation)));
  connect(stack, SIGNAL(captureFinished()), this, SLOT(captureFinished()));
}

int TcpData::conversationsCount() const {
  QMutexLocker locker(&_mutex);
  return _conversations.count();
}

int TcpData::packetCount(int conversationIndex) const {
  QMutexLocker locker(&_mutex);
  return (conversationIndex >= 0 && conversationIndex < _conversations.size())
      ? _conversations.at(conversationIndex)->_packets.size()
      : 0;
}

QPcapTcpConversation TcpData::conversationAt(int conversationIndex) const {
  QMutexLocker locker(&_mutex);
  return (conversationIndex >= 0 && conversationIndex < _conversations.size())
      ? _conversations.at(conversationIndex)->_conversation
      : QPcapTcpConversation();

}

int TcpData::conversationIndexById(quint64 conversationId) const {
  QMutexLocker locker(&_mutex);
  return _conversationsIndexById.value(conversationId, -1);
}

int TcpData::conversationIndexByPacketId(quint64 packetId) const {
  QMutexLocker locker(&_mutex);
  return _conversationsIndexByPacketId.value(packetId, -1);
}

int TcpData::packetIndexById(int conversationIndex, quint64 packetId) const {
  Conversation *c = _conversations.at(conversationIndex);
  if (c)
    for (int i = 0; i < c->_packets.size(); ++i) {
      const QPcapTcpPacket p = c->_packets.at(i);
      if (p.id() == packetId)
        return i;
    }
  return -1;
}

QPcapTcpPacket TcpData::packetAt(int conversationIndex, int packetIndex) const {
  QMutexLocker locker(&_mutex);
  if (conversationIndex >= 0 && conversationIndex < _conversations.size()) {
    Conversation *c = _conversations.at(conversationIndex);
    if (packetIndex >= 0 && packetIndex <= c->_packets.size())
      return c->_packets.at(packetIndex);
  }
  return QPcapTcpPacket();
}

void TcpData::addConversation(QPcapTcpConversation conversation) {
  QMutexLocker locker(&_mutex);
  //qDebug() << "addConversation" << conversation.id();
  int size = _conversations.size();
  _conversationsIndexById.insert(conversation.id(), size);
  _conversations.append(new Conversation(conversation));
  locker.unlock();
  if (size % 1000 == 10)
    emit hasMoreConversations();
}

void TcpData::setConversationFinished(QPcapTcpConversation conversation) {
  Q_UNUSED(conversation);
}

void TcpData::addPacket(QPcapTcpPacket packet,
                        QPcapTcpConversation conversation) {
  //qDebug() << "addPacket" << conversation.id() << packet.id();
  QMutexLocker locker(&_mutex);
  int index = _conversationsIndexById.value(conversation.id(), -1);
  if (index >= 0) {
    Conversation *c = _conversations.at(index);
    c->_packets.append(packet);
    _conversationsIndexByPacketId.insert(packet.id(), index);
  } else
    qWarning() << "TcpData::addPacket with unknown conversation"
               << conversation.id();
  ulong count = ++_packetsCount;
  locker.unlock();
  if (count % 1000 == 0)
    emit packetsCountTick(count);
}

void TcpData::captureFinished() {
  emit packetsCountTick(_packetsCount);
  emit hasMoreConversations();
}

void TcpData::clear() {
  QMutexLocker locker(&_mutex);
  _conversationsIndexById.clear();
  _conversationsIndexByPacketId.clear();
  // actual deletion of objects is performed asynchronously because clear()
  // can be called by any thread, especially the gui thread and deletion can
  // be cpu intensive
  QList<Conversation*> garbage = _conversations;
  _conversations = QList<Conversation*>();
  QMetaObject::invokeMethod(this, "collectGarbage", Qt::QueuedConnection,
                            Q_ARG(QList<Conversation*>, garbage));
  _packetsCount = 0;
  locker.unlock();
  emit dataReset();
}

void TcpData::collectGarbage(QList<Conversation*> garbage) {
  // LATER split into smaller pieces or better set to distinct thread ?
  qDeleteAll(garbage);
}
