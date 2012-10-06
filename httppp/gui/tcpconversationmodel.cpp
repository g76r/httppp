#include "tcpconversationmodel.h"
#include "tcpdata.h"

#define COLUMNS 3

TcpConversationModel::TcpConversationModel(QObject *parent, TcpData *data)
  : QAbstractItemModel(parent), _data(data), _conversationsCount(0) {
  connect(data, SIGNAL(dataReset()), this, SLOT(dataReset()));
  connect(data, SIGNAL(hasMoreConversations()),
          this, SLOT(fetchMoreConversations()));
}

QModelIndex TcpConversationModel::index(int row, int column,
                                        const QModelIndex &parent) const {
  if (parent.isValid())
    return QModelIndex();
  if (row < 0 || row >= _conversationsCount)
    return QModelIndex();
  return createIndex(row, column, (quint32)_data->conversationAt(row).id());
}

QModelIndex TcpConversationModel::index(QPcapTcpConversation
                                        conversation) const {
  int row = _data->conversationIndexById(conversation.id());
  return row >= 0 ? index(row, 0, QModelIndex()) : QModelIndex();
}

QPcapTcpConversation TcpConversationModel::conversation(
    const QModelIndex &index) const {
  return index.isValid() ? _data->conversationAt(index.row())
                         : QPcapTcpConversation();
}

QModelIndex TcpConversationModel::parent(const QModelIndex &child) const {
  Q_UNUSED(child);
  return QModelIndex();
}

int TcpConversationModel::rowCount(const QModelIndex &parent) const {
  return parent.isValid() ? 0 : _conversationsCount;
}

int TcpConversationModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return COLUMNS;
}

QVariant TcpConversationModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();
  QPcapTcpConversation c = _data->conversationAt(index.row());
  switch (role) {
  case Qt::DisplayRole: {
    switch(index.column()) {
    case 0:
      return c.id();
    case 1:
      return c.firstPacket().src();
    case 2:
      return c.firstPacket().dst();
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

bool TcpConversationModel::hasChildren(const QModelIndex &parent) const {
  return !parent.isValid();
}

bool TcpConversationModel::canFetchMore(const QModelIndex &parent) const {
  return !parent.isValid() && _conversationsCount < _data->conversationsCount();
}

void TcpConversationModel::fetchMore(const QModelIndex &parent) {
  Q_UNUSED(parent);
  int n = _data->conversationsCount() - _conversationsCount;
  if (n > 0) {
    beginInsertRows(QModelIndex(), _conversationsCount,
                    _conversationsCount+n-1);
    _conversationsCount += n;
    endInsertRows();
  }
}

void TcpConversationModel::dataReset() {
  if (_conversationsCount) {
    beginRemoveRows(QModelIndex(), 0, _conversationsCount-1);
    _conversationsCount = 0;
    endRemoveRows();
  }
}

void TcpConversationModel::fetchMoreConversations() {
  fetchMore(QModelIndex());
}
