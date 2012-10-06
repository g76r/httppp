#include "tcppacketmodel.h"
#include <QBrush>

#define COLUMNS 1
#define PACKET_FLAG 0x80000000

TcpPacketModel::TcpPacketModel(QObject *parent, TcpData *data)
  : QAbstractItemModel(parent), _data(data),
    _upstreamIcon(":/icons/red_up_arrow.svg"),
    _downstreamIcon(":/icons/blue_down_arrow.svg"), _conversationsCount(0) {
  connect(data, SIGNAL(dataReset()), this, SLOT(dataReset()));
  connect(data, SIGNAL(hasMoreConversations()),
          this, SLOT(fetchMoreConversations()));
}

QModelIndex TcpPacketModel::index(int row, int column,
                                  const QModelIndex &parent) const {
  Q_UNUSED(column);
  if (parent.isValid()) {
    quint32 id = parent.internalId();
    if (id & PACKET_FLAG)
      return QModelIndex(); // packets have no children
    QModelIndex i = createIndex(row, 0, id | PACKET_FLAG);
    //qDebug() << "index packet" << id << i.internalId() << row;
    return i;
  }
  QPcapTcpConversation c = _data->conversationAt(row);
  //if (c.isNull())
  //  return QModelIndex();
  QModelIndex i = createIndex(row, 0, (quint32)c.id());
  //qDebug() << "index conv" << i.internalId() << row;
  return i;
}

QModelIndex TcpPacketModel::parent(const QModelIndex &child) const {
  if (!child.isValid())
    return QModelIndex();
  quint32 id = child.internalId();
  if (id & PACKET_FLAG) {
    id &= ~PACKET_FLAG;
    int index = _data->conversationIndexById(id);
    if (index >= 0)
      return createIndex(index, 0, id);
  }
  return QModelIndex();
}

int TcpPacketModel::rowCount(const QModelIndex &parent) const {
  if (!parent.isValid())
    return _conversationsCount;
  quint32 id = parent.internalId();
  return (id & PACKET_FLAG) ? 0 : _packetsCount.value(id);
}

int TcpPacketModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return COLUMNS;
}

QVariant TcpPacketModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();
  //qDebug() << "data" << index.internalId();
  quint32 id = index.internalId();
  int conversationIndex = _data->conversationIndexById(id & ~PACKET_FLAG);
  if (conversationIndex < 0)
    return QVariant();
  if (id & PACKET_FLAG) {
    QPcapTcpPacket packet = _data->packetAt(conversationIndex, index.row());
    //qDebug() << "data for packet" << (id & ~PACKET_FLAG) << index.row()
    //         << packet.id() << "role" << role << index.column();
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::ToolTipRole:
      switch (index.column()) {
      case 0:
        return packet.toShortText();
      }
      break;
    case Qt::DecorationRole:
      return packet.upstream() ? _upstreamIcon : _downstreamIcon;
      break;
    case Qt::ForegroundRole:
      return QBrush(packet.upstream() ? QColor(255, 0, 0) : QColor(0, 0, 255));
    }
  } else {
    QPcapTcpConversation conversation
        = _data->conversationAt(conversationIndex);
    //qDebug() << "data for conversation" << (id&~PACKET_FLAG) << index.row()
    //<< conversation.id();
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::ToolTipRole:
      return conversation.toText();
    case Qt::DecorationRole:
      break;
    case Qt::ForegroundRole:
      break;
    }
  }
  return QVariant();
}

QVariant TcpPacketModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const {
  if (section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return "TCP Sequence Number";
  return QVariant();
}

bool TcpPacketModel::hasChildren(const QModelIndex &parent) const {
  if (!parent.isValid())
    return _conversationsCount || canFetchMore(parent);
  if (parent.internalId() & PACKET_FLAG)
    return false;
  return _packetsCount.value(parent.internalId(), false)
      || canFetchMore(parent);
}

bool TcpPacketModel::canFetchMore(const QModelIndex &parent) const {
  //qDebug() << "canFetchMore" << parent.isValid() << parent.internalId();
  if (!parent.isValid())
    return _conversationsCount < _data->conversationsCount();
  quint32 id = parent.internalId();
  if (id & PACKET_FLAG)
    return false;
  return _packetsCount.value(id, 0) < _data->packetCount(parent.row());
}

void TcpPacketModel::fetchMore(const QModelIndex &parent) {
  quint32 id = parent.internalId();
  //qDebug() << "fetchMore" << parent.isValid() << id;
  if (!parent.isValid()) {
    int n = _data->conversationsCount() - _conversationsCount;
    if (n > 0) {
      n = qMin(n, 10);
      beginInsertRows(parent, _conversationsCount, _conversationsCount+n-1);
      //qDebug() << "  conversations:" << n;
      _conversationsCount += n;
      endInsertRows();
    }
  } else if (!(id & PACKET_FLAG)) {
    int before = _packetsCount.value(id);
    int after = _data->packetCount(parent.row());
    if (after > before) {
      after = qMin(after, before+10);
      beginInsertRows(parent, before, after-1);
      //qDebug() << "  packets:" << parent.row() << after-before;
      _packetsCount.insert(id, after);
      endInsertRows();
    }
  }
}

QModelIndex TcpPacketModel::index(QPcapTcpConversation conversation) const {
  int index = _data->conversationIndexById(conversation.id());
  return index >= 0 ? createIndex(index, 0, (quint32)conversation.id())
                    : QModelIndex();
}

QModelIndex TcpPacketModel::index(QPcapTcpPacket packet) const {
  int conversationIndex = _data->conversationIndexByPacketId(packet.id());
  if (conversationIndex >= 0) {
    QPcapTcpConversation conversation
        = _data->conversationAt(conversationIndex);
    int packetIndex = _data->packetIndexById(conversationIndex, packet.id());
    if (packetIndex >= 0)
      return createIndex(packetIndex, 0,
                         (quint32)conversation.id() | PACKET_FLAG);
  }
  return QModelIndex();
}

QPcapTcpPacket TcpPacketModel::packet(const QModelIndex &index) const {
  if (!index.isValid())
    return QPcapTcpPacket();
  quint32 id = index.internalId();
  if (id & PACKET_FLAG)
    return _data->packetAt(_data->conversationIndexById(id & ~PACKET_FLAG),
                           index.row());
  return QPcapTcpPacket();
}

QPcapTcpConversation TcpPacketModel::conversation(
    const QModelIndex &index) const {
  if (!index.isValid())
    return QPcapTcpConversation();
  int conversationIndex
      = _data->conversationIndexById(index.internalId() & ~PACKET_FLAG);
  return conversationIndex >= 0 ? _data->conversationAt(conversationIndex)
                                : QPcapTcpConversation();
}

void TcpPacketModel::dataReset() {
  //qDebug() << "dataReset" << _conversationsCount;
  if (_conversationsCount) {
    beginRemoveRows(QModelIndex(), 0, _conversationsCount-1);
    _conversationsCount = 0;
    _packetsCount.clear();
    endRemoveRows();
  }
}

//#include <QMetaObject>
void TcpPacketModel::fetchMoreConversations() {
  //qDebug() << "fetchMoreConversations";
  QModelIndex root;
  fetchMore(root);
  //if (canFetchMore(root))
  //  QMetaObject::invokeMethod(this, "fetchMoreConversations",
  //                            Qt::QueuedConnection);
}
