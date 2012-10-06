#ifndef TCPPACKETMODEL_H
#define TCPPACKETMODEL_H

#include <QAbstractItemModel>
#include "qpcaptcppacket.h"
#include "qpcaptcpconversation.h"
#include <QIcon>
#include "tcpdata.h"

class TcpPacketModel : public QAbstractItemModel {
  Q_OBJECT
  TcpData *_data;
  QIcon _upstreamIcon, _downstreamIcon;
  int _conversationsCount;
  QHash<quint64,int> _packetsCount;

public:
  explicit TcpPacketModel(QObject *parent, TcpData *data);
  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  QModelIndex parent(const QModelIndex &child) const;
  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  QModelIndex index(QPcapTcpConversation) const;
  QModelIndex index(QPcapTcpPacket) const;
  /** @return packet pointed by index or null packet (isNull()) if index points
    * to a conversation
    */
  QPcapTcpPacket packet(const QModelIndex &index) const;
  /** @return conversation pointed by index, or conversation to which packet
    * pointed by index belong to.
    */
  QPcapTcpConversation conversation(const QModelIndex &index) const;
  bool hasChildren(const QModelIndex &parent) const;
  bool canFetchMore(const QModelIndex &parent) const;
  void fetchMore(const QModelIndex &parent);

private slots:
  void dataReset();
  void fetchMoreConversations();
};

#endif // TCPPACKETMODEL_H
