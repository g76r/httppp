#ifndef TCPCONVERSATIONMODEL_H
#define TCPCONVERSATIONMODEL_H

#include <QAbstractItemModel>
#include "qpcaptcpconversation.h"

class TcpData;

class TcpConversationModel : public QAbstractItemModel {
  Q_OBJECT
private:
  TcpData *_data;
  int _conversationsCount;

public:
  explicit TcpConversationModel(QObject *parent, TcpData *data);
  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  QModelIndex index(QPcapTcpConversation conversation) const;
  QModelIndex parent(const QModelIndex &child) const;
  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  QPcapTcpConversation conversation(const QModelIndex &index) const;
  bool hasChildren(const QModelIndex &parent) const;
  bool canFetchMore(const QModelIndex &parent) const;
  void fetchMore(const QModelIndex &parent);

private slots:
  void dataReset();
  void fetchMoreConversations();
};

#endif // TCPCONVERSATIONMODEL_H
