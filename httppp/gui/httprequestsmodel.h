#ifndef HTTPREQUESTSMODEL_H
#define HTTPREQUESTSMODEL_H

#include <QAbstractItemModel>

class HttpRequestsModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  explicit HttpRequestsModel(QObject *parent = 0);
  
signals:
  
public slots:
  
};

#endif // HTTPREQUESTSMODEL_H
