#ifndef TCPCONVERSATIONMODEL_H
#define TCPCONVERSATIONMODEL_H

#include <QAbstractItemModel>
#include "qpcaptcpconversation.h"

class TcpConversationModel : public QAbstractItemModel {
  Q_OBJECT
private:
  QList<QPcapTcpConversation> _list;

public:
  explicit TcpConversationModel(QObject *parent = 0);
  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  QModelIndex index(QPcapTcpConversation conversation) const;
  QModelIndex parent(const QModelIndex &child) const;
  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  QPcapTcpConversation conversation(const QModelIndex &index) const;
  void clear();

public slots:
  void addConversation(QPcapTcpConversation conversation);
  void setConversationFinished(QPcapTcpConversation conversation);
};

#endif // TCPCONVERSATIONMODEL_H
