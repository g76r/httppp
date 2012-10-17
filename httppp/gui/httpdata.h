#ifndef HTTPDATA_H
#define HTTPDATA_H

#include <QObject>
#include <QMutex>
#include <QList>
#include "qpcaphttphit.h"
#include "qpcaphttpstack.h"
#include "common/httpcustomfieldanalyzer.h"

class HttpData : public QObject {
  Q_OBJECT
  mutable QMutex _mutex;
  QList<QPcapHttpHit> _hits;

public:
  explicit HttpData(QObject *parent = 0);
  void connectToSource(QPcapHttpStack *source);
  void connectToSource(HttpCustomFieldAnalyzer *souce);
  int hitsCount() const;
  QPcapHttpHit hitAt(int index) const;

signals:
  void dataReset();
  void hitsCountTick(unsigned long count);
  void captureStarted();
  void captureFinished();

public slots:
  void addHit(QPcapHttpHit hit);
  void clear();

private slots:
  void captureStarting();
  void captureFinishing();

private:
  Q_INVOKABLE void collectGarbage(QList<QPcapHttpHit> garbage);
};

#endif // HTTPDATA_H
