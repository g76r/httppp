#include "httphitmodel.h"
#include <QDateTime>
#include "qpcaphttphit.h"
#include "httpdata.h"

#define COLUMNS 21

HttpHitModel::HttpHitModel(QObject *parent, HttpData *data)
  : QAbstractItemModel(parent), _data(data), _hitsCount(0) {
  connect(data, SIGNAL(dataReset()), this, SLOT(dataReset()));
  connect(data, SIGNAL(hitsCountTick(ulong)), this, SLOT(fetchMoreHits()));
}

QModelIndex HttpHitModel::index(int row, int column,
                                const QModelIndex &parent) const {
  Q_UNUSED(parent)
  if (row < 0 || row >= _hitsCount)
    return QModelIndex();
  return createIndex(row, column, nullptr);
}

QModelIndex HttpHitModel::parent(const QModelIndex &child) const {
  Q_UNUSED(child)
  return QModelIndex();
}

int HttpHitModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)
  return _hitsCount;
}

int HttpHitModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)
  return COLUMNS;
}

bool HttpHitModel::canFetchMore(const QModelIndex &parent) const {
  Q_UNUSED(parent)
  return _hitsCount < _data->hitsCount();
}

void HttpHitModel::fetchMore(const QModelIndex &parent) {
  Q_UNUSED(parent)
  int n = _data->hitsCount() - _hitsCount;
  if (n > 0) {
    beginInsertRows(QModelIndex(), _hitsCount, _hitsCount+n-1);
    _hitsCount += n;
    endInsertRows();
  }
}

QVariant HttpHitModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();
  QPcapHttpHit hit = _data->hitAt(index.row());
  if (!hit.isValid())
    return QVariant();
  switch (role) {
  case Qt::DisplayRole:
    switch(index.column()) {
    case 0:
      return hit.conversation().id();
    case 1:
      return hit.methodAsString();
    case 2:
      return hit.host();
    case 3:
      return hit.path();
    case 4:
      return hit.firstRequestPacket().src();
    case 5:
      return hit.firstRequestPacket().dst();
    case 6:
      return hit.firstRequestPacket().ip().src();
    case 7:
      return hit.firstRequestPacket().ip().dst();
    case 8:
      return hit.firstRequestPacket().srcPort();
    case 9:
      return hit.firstRequestPacket().dstPort();
    case 10:
      return hit.protocol();
    case 11:
      return QDateTime::fromMSecsSinceEpoch(hit.requestTimestamp()/1000)
          .toString("yyyy-MM-dd");
    case 12:
      return QDateTime::fromMSecsSinceEpoch(hit.requestTimestamp()/1000)
          .toString("hh:mm:ss");
    case 13:
      return hit.requestTimestamp()%1000000;
    case 14:
      return hit.requestTimestamp();
    case 15:
      return hit.usecToFirstByte();
    case 16:
      return hit.usecToLastByte();
    case 17:
      return hit.returnCode();
    case 18:
      return hit.customField(0);
    case 19:
      return hit.customField(1);
    case 20:
      return hit.customField(2);
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
      return tr("TTFB (usec)");
    case 16:
      return tr("TTLB (usec)");
    case 17:
      return tr("Return Code");
    case 18:
      return tr("Field 1");
    case 19:
      return tr("Field 2");
    case 20:
      return tr("Field 3");
    }
  }
  default:
    ;
  }
  return QVariant();
}

QPcapHttpHit HttpHitModel::hit(const QModelIndex &index) const {
  return index.isValid() ? _data->hitAt(index.row()) : QPcapHttpHit();
}

void HttpHitModel::dataReset() {
  if (_hitsCount) {
    emit beginRemoveRows(QModelIndex(), 0, _hitsCount-1);
    _hitsCount = 0;
    emit endRemoveRows();
  }
}

void HttpHitModel::fetchMoreHits() {
  fetchMore(QModelIndex());
}
