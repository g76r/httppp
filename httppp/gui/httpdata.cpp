#include "httpdata.h"
#include <QMetaObject>

HttpData::HttpData(QObject *parent) : QObject(parent) {
  qRegisterMetaType<QList<QPcapHttpHit> >("QList<QPcapHttpHit>");
}

void HttpData::connectToSource(QPcapHttpStack *source) {
  connect(source, SIGNAL(httpHit(QPcapHttpHit,QByteArray,QByteArray)),
          this, SLOT(addHit(QPcapHttpHit)));
  connect(source, SIGNAL(captureStarted()), this, SLOT(captureStarting()));
  connect(source, SIGNAL(captureFinished()), this, SLOT(captureFinishing()));
}

void HttpData::connectToSource(HttpCustomFieldAnalyzer *source) {
  connect(source, SIGNAL(httpHit(QPcapHttpHit)),
          this, SLOT(addHit(QPcapHttpHit)));
  connect(source, SIGNAL(captureStarted()), this, SLOT(captureStarting()));
  connect(source, SIGNAL(captureFinished()), this, SLOT(captureFinishing()));
}

int HttpData::hitsCount() const {
  QMutexLocker locker(&_mutex);
  return _hits.size();
}

QPcapHttpHit HttpData::hitAt(int index) const {
  QMutexLocker locker(&_mutex);
  return index >= 0 && index < _hits.size() ? _hits.at(index) : QPcapHttpHit();
}

void HttpData::addHit(QPcapHttpHit hit) {
  QMutexLocker locker(&_mutex);
  _hits.append(hit);
  ulong count = _hits.size();
  locker.unlock();
  if (count % 100 == 0)
    emit hitsCountTick(count);
}

void HttpData::captureStarting() {
  emit captureStarted();
  emit hitsCountTick(0);
}

void HttpData::captureFinishing() {
  QMutexLocker locker(&_mutex);
  ulong count = _hits.size();
  locker.unlock();
  emit hitsCountTick(count);
  emit captureFinished();
}

void HttpData::clear() {
  QMutexLocker locker(&_mutex);
  // actual deletion of objects is performed asynchronously because clear()
  // can be called by any thread, especially the gui thread and deletion can
  // be cpu intensive
  QList<QPcapHttpHit> garbage = _hits;
  _hits = QList<QPcapHttpHit>();
  QMetaObject::invokeMethod(this, "collectGarbage", Qt::QueuedConnection,
                            Q_ARG(QList<QPcapHttpHit>, garbage));
  locker.unlock();
  emit dataReset();
}

void HttpData::collectGarbage(QList<QPcapHttpHit> garbage) {
  garbage.clear();
}
