#include "httphitmodel.h"
#include <QDateTime>

#define COLUMNS 17

HttpHitModel::HttpHitModel(QObject *parent) : QAbstractItemModel(parent) {
}

QModelIndex HttpHitModel::index(int row, int column,
                                const QModelIndex &parent) const {
  Q_UNUSED(parent);
  if (row < 0 || row >= _list.size())
    return QModelIndex();
  return createIndex(row, column, 0);
}

QModelIndex HttpHitModel::parent(const QModelIndex &child) const {
  Q_UNUSED(child);
  return QModelIndex();
}

int HttpHitModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return _list.size();
}

int HttpHitModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return COLUMNS;
}

QVariant HttpHitModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();
  if (index.row() < 0 || index.row() >= _list.size() || index.column() < 0
       || index.column() >= COLUMNS)
    return QVariant();
  switch (role) {
  case Qt::DisplayRole: {
    switch(index.column()) {
    case 0:
      return _list.at(index.row()).conversation().id();
    case 1:
      return _list.at(index.row()).methodAsString();
    case 2:
      return _list.at(index.row()).host();
    case 3:
      return _list.at(index.row()).path();
    case 4:
      return _list.at(index.row()).firstRequestPacket().src();
    case 5:
      return _list.at(index.row()).firstRequestPacket().dst();
    case 6:
      return _list.at(index.row()).firstRequestPacket().ip().src();
    case 7:
      return _list.at(index.row()).firstRequestPacket().ip().dst();
    case 8:
      return _list.at(index.row()).firstRequestPacket().srcPort();
    case 9:
      return _list.at(index.row()).firstRequestPacket().dstPort();
    case 10:
      return _list.at(index.row()).protocol();
    case 11:
      return QDateTime::fromMSecsSinceEpoch(
            _list.at(index.row()).requestTimestamp()/1000)
          .toString("yyyy-MM-dd");
    case 12:
      return QDateTime::fromMSecsSinceEpoch(
            _list.at(index.row()).requestTimestamp()/1000)
          .toString("hh:mm:ss");
    case 13:
      return _list.at(index.row()).requestTimestamp()%1000000;
    case 14:
      return _list.at(index.row()).requestTimestamp();
    case 15:
      return _list.at(index.row()).usecToFirstByte();
    case 16:
      return _list.at(index.row()).usecToLastByte();
    }
  }
  default:
    ;
  }
  return QVariant();
}

QVariant HttpHitModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const {
  if (orientation != Qt::Horizontal)
    return QVariant();
  switch (role) {
  case Qt::DisplayRole: {
    switch(section) {
    case 0:
      return tr("Conversation");
    case 1:
      return tr("Method");
    case 2:
      return tr("Host");
    case 3:
      return tr("Path");
    case 4:
      return tr("Source TCP");
    case 5:
      return tr("Destination TCP");
    case 6:
      return tr("Source IP");
    case 7:
      return tr("Destination IP");
    case 8:
      return tr("Source TCP Port");
    case 9:
      return tr("Destination TCP Port");
    case 10:
      return tr("Protocol");
    case 11:
      return tr("Day");
    case 12:
      return tr("Time");
    case 13:
      return tr("Microseconds");
    case 14:
      return tr("Timestamp");
    case 15:
      return tr("TTFB");
    case 16:
      return tr("TTLB");
    }
  }
  default:
    ;
  }
  return QVariant();
}

QPcapHttpHit HttpHitModel::hit(const QModelIndex &index) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= _list.size())
    return QPcapHttpHit();
  return _list.at(index.row());
}

void HttpHitModel::clear() {
  emit beginResetModel();
  _list.clear();
  emit endResetModel();
}

void HttpHitModel::addHit(QPcapHttpHit hit) {
  qDebug() << "addHit" << hit.conversation().id();
  emit beginInsertRows(QModelIndex(), _list.size(), _list.size());
  _list.append(hit);
  emit endInsertRows();
}
