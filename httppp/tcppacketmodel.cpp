#include "tcppacketmodel.h"
#include <QBrush>

#define COLUMNS 1

TcpPacketModel::TcpPacketModel(QObject *parent)
  : QAbstractItemModel(parent),
    _root(new TreeItem(0, PcapTcpPacket(), PcapTcpConversation(), true)) {
}

TcpPacketModel::~TcpPacketModel() {
  delete _root;
}

QModelIndex TcpPacketModel::index(int row, int column,
                                  const QModelIndex &parent) const {
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
  return createIndex(parentItem->row(), 0, parentItem);
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
        return item->_packet.seqNumber();
      }
      break;
    case Qt::DecorationRole:
      // TODO icon for up and downstream
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
      return item->_conversation.english();
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
  // TODO
  return QVariant();
}

//Qt::ItemFlags flags(const QModelIndex & index) const {
//}

void TcpPacketModel::addTcpUpstreamPacket(PcapTcpPacket packet,
                                          PcapTcpConversation conversation) {
  addPacket(conversation, packet, true);
}

void TcpPacketModel::addTcpDownstreamPacket(PcapTcpPacket packet,
                                            PcapTcpConversation conversation) {
  addPacket(conversation, packet, false);
}

//#include <QtDebug>
void TcpPacketModel::addPacket(PcapTcpConversation conversation,
                               PcapTcpPacket packet, bool upstream) {
  //qDebug() << "addPacket" << packet;
  emit beginResetModel(); // TODO optimize
  TreeItem *parent;
  foreach(TreeItem *i, _root->_children) {
    if (i->_conversation == conversation) {
      parent = i;
      goto found;
    }
  }
  parent = new TreeItem(_root, PcapTcpPacket(), conversation, false);
  _root->_children.append(parent);
found:
  parent->_children.append(new TreeItem(parent, packet, conversation, upstream));
  conversation.packets().append(packet);
  emit endResetModel();
}

QModelIndex TcpPacketModel::index(PcapTcpConversation conversation) const {
  for (int i = 0; i < _root->_children.size(); ++ i) {
    TreeItem *ti = _root->_children.at(i);
    if (ti && ti->_conversation == conversation)
      return createIndex(i, 0, ti);
  }
  return QModelIndex();
}

PcapTcpPacket TcpPacketModel::packet(const QModelIndex &index) const {
  if (!index.isValid())
    return PcapTcpPacket();
  TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
  if (!item || !item->_parent )
    return PcapTcpPacket();
  return item->_packet;
}

PcapTcpConversation TcpPacketModel::conversation(
    const QModelIndex &index) const {
  if (!index.isValid())
    return PcapTcpConversation();
  TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
  if (!item || !item->_parent )
    return PcapTcpConversation();
  return item->_conversation;
}