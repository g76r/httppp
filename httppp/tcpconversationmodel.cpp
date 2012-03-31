#include "tcpconversationmodel.h"

#define COLUMNS 5

TcpConversationModel::TcpConversationModel(QObject *parent)
  : QAbstractItemModel(parent) {
}

void TcpConversationModel::addConversation(PcapTcpConversation conversation) {
  emit beginInsertRows(QModelIndex(), _list.size(), _list.size());
  _list.append(conversation);
  emit endInsertRows();
}

void TcpConversationModel::setConversationFinished(
    PcapTcpConversation conversation) {
  Q_UNUSED(conversation);
  // FIXME
}

QModelIndex TcpConversationModel::index(int row, int column,
                                        const QModelIndex &parent) const {
  if (row < 0 || row >= _list.size())
    return QModelIndex();
  return createIndex(row, column, _list.at(row).id());
}

int TcpConversationModel::row(PcapTcpConversation conversation) const {
  for (int i = 0; i < _list.size(); ++i) {
    PcapTcpConversation c = _list.at(i);
    if (c == conversation)
      return i;
  }
  return -1;
}

/*QModelIndex TcpConversationModel::index(PcapTcpConversation
                                        conversation) const {
  for (int i = 0; i < _list.size(); ++i) {
    PcapTcpConversation c = _list.at(i);
    if (c == conversation)
      return createIndex(i, 0, c.id());
  }
}*/

PcapTcpConversation TcpConversationModel::conversation(
    const QModelIndex &index) const {
  foreach (PcapTcpConversation c, _list) {
    if (c.id() == index.internalId())
      return c;
  }
  return PcapTcpConversation();
}

QModelIndex TcpConversationModel::parent(const QModelIndex &child) const {
  return QModelIndex();
}

int TcpConversationModel::rowCount(const QModelIndex &parent) const {
  return _list.size();
}

int TcpConversationModel::columnCount(const QModelIndex &parent) const {
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
      return _list.at(index.row()).firstPacket().srcPort();
    case 3:
      return _list.at(index.row()).firstPacket().dst();
    case 4:
      return _list.at(index.row()).firstPacket().srcPort();
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
      return tr("Source Host");
    case 2:
      return tr("Source Port");
    case 3:
      return tr("Destination Host");
    case 4:
      return tr("Destination Port");
    }
  }
  default:
    ;
  }
  return QVariant();
}
