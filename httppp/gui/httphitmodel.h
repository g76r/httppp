#ifndef HTTPHITMODEL_H
#define HTTPHITMODEL_H

#include <QAbstractItemModel>
#include "qpcaphttphit.h"
#include "qpcaptcpconversation.h"

class HttpHitModel : public QAbstractItemModel {
  Q_OBJECT
private:
  QList<QPcapHttpHit> _list;

public:
  explicit HttpHitModel(QObject *parent = 0);
  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  QModelIndex parent(const QModelIndex &child) const;
  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  QPcapHttpHit hit(const QModelIndex &index) const;
  void clear();

public slots:
  void addHit(QPcapHttpHit hit);
};

#endif // HTTPHITMODEL_H
