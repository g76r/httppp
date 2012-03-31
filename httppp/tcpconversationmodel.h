#ifndef TCPCONVERSATIONMODEL_H
#define TCPCONVERSATIONMODEL_H

#include <QAbstractItemModel>
#include "pcaptcpconversation.h"

class TcpConversationModel : public QAbstractItemModel {
  Q_OBJECT
private:
  QList<PcapTcpConversation> _list;

public:
  explicit TcpConversationModel(QObject *parent = 0);
  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  QModelIndex index(PcapTcpConversation conversation) const;
  QModelIndex parent(const QModelIndex &child) const;
  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  PcapTcpConversation conversation(const QModelIndex &index) const;
  //QModelIndex index(PcapTcpConversation conversation) const;
  int row(PcapTcpConversation conversation) const;

signals:
  
public slots:
  void addConversation(PcapTcpConversation conversation);
  void setConversationFinished(PcapTcpConversation conversation);
};

#endif // TCPCONVERSATIONMODEL_H
