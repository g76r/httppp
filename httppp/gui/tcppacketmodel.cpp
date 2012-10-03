#include "tcppacketmodel.h"
#include <QBrush>

#define COLUMNS 1

TcpPacketModel::TcpPacketModel(QObject *parent)
  : QAbstractItemModel(parent),
    _root(new TreeItem(0, QPcapTcpPacket(), QPcapTcpConversation(), true, 0)),
    _upstreamIcon(":/icons/red_up_arrow.svg"),
    _downstreamIcon(":/icons/blue_down_arrow.svg") {
}

TcpPacketModel::~TcpPacketModel() {
  delete _root;
}

QModelIndex TcpPacketModel::index(int row, int column,
                                  const QModelIndex &parent) const {
  Q_UNUSED(column);
  if (row < 0)
    return QModelIndex();
  const QList<TreeItem*> *list = parent.isValid()
      ? &static_cast<TreeItem*>(parent.internalPointer())->_children
      : &_root->_children;
  if (row >= list->size())
    return QModelIndex();
  return createIndex(row, 0, list->at(row));
}

QModelIndex TcpPacketModel::parent(const QModelIndex &child) const {
  if (!child.isValid())
    return QModelIndex();
  TreeItem *childItem = static_cast<TreeItem*>(child.internalPointer());
  TreeItem *parentItem = childItem->_parent;
  if (parentItem == _root)
    return QModelIndex();
  return createIndex(parentItem->_row, 0, parentItem);
}

int TcpPacketModel::rowCount(const QModelIndex &parent) const {
  if (!parent.isValid())
    return _root->_children.size();
  return static_cast<TreeItem*>(parent.internalPointer())->_children.size();
}

int TcpPacketModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return COLUMNS;
}

QVariant TcpPacketModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();
  TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
  if (!item || !item->_parent )
    return QVariant();
  if (!item->_packet.isNull()) {
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::ToolTipRole:
      if (index.column() < 0 || index.column() >= COLUMNS)
        return QVariant();
      switch (index.column()) {
      case 0:
        return item->_packet.toShortText();
      }
      break;
    case Qt::DecorationRole:
      if (!item->_packet.isNull())
       return (item->_conversation.matchesSameStream(item->_packet))
           ? _upstreamIcon : _downstreamIcon;
      break;
    case Qt::ForegroundRole:
      return QBrush(item->_upstreamFinished ? QColor(255, 0, 0)
                                    : QColor(0, 0, 255));
    }
  } else  if (!item->_conversation.isNull()) {
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::ToolTipRole:
      return item->_conversation.toText();
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

void TcpPacketModel::addTcpUpstreamPacket(QPcapTcpPacket packet,
                                          QPcapTcpConversation conversation) {
  addPacket(conversation, packet, true);
}

void TcpPacketModel::addTcpDownstreamPacket(QPcapTcpPacket packet,
                                            QPcapTcpConversation conversation) {
  addPacket(conversation, packet, false);
}

void TcpPacketModel::addPacket(QPcapTcpConversation conversation,
                               QPcapTcpPacket packet, bool upstream) {
  //qDebug() << "addPacket" << packet;
  TreeItem *parent = _conversationItemsById.value(conversation.id());
  if (!parent) {
    beginInsertRows(QModelIndex(), _root->_children.size(),
                    _root->_children.size());
    parent = new TreeItem(_root, QPcapTcpPacket(), conversation, false,
                          _root->_children.size());
    _conversationItemsById.insert(conversation.id(), parent);
    _root->_children.append(parent);
    endInsertRows();
  }
  beginInsertRows(createIndex(parent->_row, 0, parent),
                  parent->_children.size(), parent->_children.size());
  parent->_children.append(new TreeItem(parent, packet, conversation,
                                        upstream, parent->_children.size()));
  endInsertRows();
}

QModelIndex TcpPacketModel::index(QPcapTcpConversation conversation) const {
  TreeItem *ti = _conversationItemsById.value(conversation.id());
  return ti ? createIndex(ti->_row, 0, ti) : QModelIndex();
}

QModelIndex TcpPacketModel::index(QPcapTcpPacket packet) const {
  // TODO optimize, this may need to set conversation reference in the tcp packet (rather than http), which is anyway more logical
  for (int i = 0; i < _root->_children.size(); ++i) {
    TreeItem *ti = _root->_children.at(i);
    for (int j = 0; j < ti->_children.size(); ++j) {
      TreeItem *tj = ti->_children.at(j);
      if (tj && tj->_packet == packet)
        return createIndex(j, 0, tj);
    }
  }
  return QModelIndex();
}

QPcapTcpPacket TcpPacketModel::packet(const QModelIndex &index) const {
  if (!index.isValid())
    return QPcapTcpPacket();
  TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
  if (!item || !item->_parent )
    return QPcapTcpPacket();
  return item->_packet;
}

QPcapTcpConversation TcpPacketModel::conversation(
    const QModelIndex &index) const {
  if (!index.isValid())
    return QPcapTcpConversation();
  TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
  if (!item || !item->_parent )
    return QPcapTcpConversation();
  return item->_conversation;
}

void TcpPacketModel::clear() {
  emit beginResetModel();
  //emit beginRemoveRows(QModelIndex(), 0, _root->_children.size()-1);
  //_root->_children.clear();
  _conversationItemsById.clear();
  delete _root;
  _root = new TreeItem(0, QPcapTcpPacket(), QPcapTcpConversation(), true, 0);
  //emit endRemoveRows();
  emit endResetModel();
}
