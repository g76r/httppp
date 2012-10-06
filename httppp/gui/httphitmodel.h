#ifndef HTTPHITMODEL_H
#define HTTPHITMODEL_H

#include <QAbstractItemModel>

class QPcapHttpHit;
class HttpData;

class HttpHitModel : public QAbstractItemModel {
  Q_OBJECT
private:
  HttpData *_data;
  int _hitsCount;

public:
  HttpHitModel(QObject *parent, HttpData *data);
  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  QModelIndex parent(const QModelIndex &child) const;
  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  bool canFetchMore(const QModelIndex &parent) const;
  void fetchMore(const QModelIndex &parent);
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  QPcapHttpHit hit(const QModelIndex &index) const;

private slots:
  void dataReset();
  void fetchMoreHits();
};

#endif // HTTPHITMODEL_H
