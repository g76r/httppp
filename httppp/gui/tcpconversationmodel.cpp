#include "tcpconversationmodel.h"

#define COLUMNS 3

TcpConversationModel::TcpConversationModel(QObject *parent)
  : QAbstractItemModel(parent) {
}

void TcpConversationModel::addConversation(QPcapTcpConversation conversation) {
  emit beginInsertRows(QModelIndex(), _list.size(), _list.size());
  _list.append(conversation);
  emit endInsertRows();
}

void TcpConversationModel::setConversationFinished(
    QPcapTcpConversation conversation) {
  Q_UNUSED(conversation);
  // LATER may do something such as changing conversation color
}

QModelIndex TcpConversationModel::index(int row, int column,
                                        const QModelIndex &parent) const {
  Q_UNUSED(parent);
  if (row < 0 || row >= _list.size())
    return QModelIndex();
  return createIndex(row, column, _list.at(row).id());
}

QModelIndex TcpConversationModel::index(QPcapTcpConversation
                                        conversation) const {
  for (int i = 0; i < _list.size(); ++i) {
    const QPcapTcpConversation &c = _list.at(i);
    if (c == conversation)
      return createIndex(i, 0, c.id());
  }
  return QModelIndex();
}

QPcapTcpConversation TcpConversationModel::conversation(
    const QModelIndex &index) const {
  foreach (QPcapTcpConversation c, _list) {
    if (c.id() == index.internalId())
      return c;
  }
  return QPcapTcpConversation();
}

QModelIndex TcpConversationModel::parent(const QModelIndex &child) const {
  Q_UNUSED(child);
  return QModelIndex();
}

int TcpConversationModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return _list.size();
}

int TcpConversationModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return COLUMNS;
}

QVariant TcpConversationModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();
  if (index.row() < 0 || index.row() >= _list.size() || index.column() < 0
       || index.column() >= COLUMNS)
    return QVariant();
  switch (role) {
  case Qt::DisplayRole: {
    switch(index.column()) {
    case 0:
      return _list.at(index.row()).id();
    case 1:
      return _list.at(index.row()).firstPacket().src();
    case 2:
      return _list.at(index.row()).firstPacket().dst();
    }
  }
  default:
    ;
  }
  return QVariant();
}

QVariant TcpConversationModel::headerData(
    int section, Qt::Orientation orientation, int role) const {
  if (orientation == Qt::Vertical)
    return QVariant();
  if (section >= COLUMNS)
    return QVariant();
  switch (role) {
  case Qt::DisplayRole: {
    switch(section) {
    case 0:
      return tr("Id");
    case 1:
      return tr("Source");
    case 2:
      return tr("Destination");
    }
  }
  default:
    ;
  }
  return QVariant();
}

void TcpConversationModel::clear() {
  emit beginRemoveRows(QModelIndex(), 0, _list.size()-1);
  _list.clear();
  emit endRemoveRows();
}
