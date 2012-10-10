#ifndef TCPDATA_H
#define TCPDATA_H

#include <QList>
#include <QMutex>
#include "qpcaptcpconversation.h"
#include "qpcaptcppacket.h"
#include "qpcaptcpstack.h"

class TcpData : public QObject {
  Q_OBJECT

  class Conversation {
    friend class TcpData;
    QPcapTcpConversation _conversation;
    QList<QPcapTcpPacket> _packets;
  public:
    Conversation(QPcapTcpConversation conversation = QPcapTcpConversation())
      : _conversation(conversation) { }
    Conversation(const Conversation &other)
      : _conversation(other._conversation), _packets(other._packets) { }
  };

  mutable QMutex _mutex;
  QList<Conversation*> _conversations;
  QHash<quint64,int> _conversationsIndexById;
  QHash<quint64,int> _conversationsIndexByPacketId;

public:
  explicit TcpData(QObject *parent = 0);
  ~TcpData();
  void connectToSource(QPcapTcpStack *stack);
  int conversationsCount() const;
  int packetCount(int conversationIndex) const;
  QPcapTcpConversation conversationAt(int conversationIndex) const;
  /** @return -1 if not found */
  int conversationIndexById(quint64 conversationId) const;
  /** @return -1 if not found */
  int conversationIndexByPacketId(quint64 packetId) const;
  int packetIndexById(int conversationIndex, quint64 packetId) const;
  QPcapTcpPacket packetAt(int conversationIndex, int packetIndex) const;
  
signals:
  void dataReset();
  void hasMoreConversations();
  void packetsCountTick(unsigned long count);

public slots:
  void addConversation(QPcapTcpConversation conversation);
  void setConversationFinished(QPcapTcpConversation conversation);
  void addPacket(QPcapTcpPacket packet, QPcapTcpConversation conversation);
  void captureFinished();
  void clear();

private:
  Q_INVOKABLE void collectGarbage(QList<Conversation*> garbage);
};

#endif // TCPDATA_H
