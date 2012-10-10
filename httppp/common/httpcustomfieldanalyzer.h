#ifndef HTTPCUSTOMFIELDANALYZER_H
#define HTTPCUSTOMFIELDANALYZER_H

#include <QObject>
#include "qpcaphttpstack.h"
#include <QAtomicInt>

class HttpCustomFieldAnalyzer : public QObject {
  Q_OBJECT

  class QPcapHttpFilter;

  class QPcapHttpFilterData : public QSharedData {
    friend class HttpCustomFieldAnalyzer::QPcapHttpFilter;
    QRegExp _re;
    QPcapHttpStack::QPcapHttpDirection _direction;
    int _captureRank;
  public:
    inline QPcapHttpFilterData(
        QRegExp regex = QRegExp(),
        QPcapHttpStack::QPcapHttpDirection direction
        = QPcapHttpStack::Anystream,
        int captureRank = 1) : _re(regex), _direction(direction),
      _captureRank(captureRank) { }
    inline QPcapHttpFilterData(const QPcapHttpFilterData &other)
      : QSharedData(), _re(other._re), _direction(other._direction),
        _captureRank(other._captureRank) { }
  };

  class QPcapHttpFilter {
    QSharedDataPointer<QPcapHttpFilterData> d;
  public:
    inline QPcapHttpFilter(
        QRegExp regex = QRegExp(),
        QPcapHttpStack::QPcapHttpDirection direction
        = QPcapHttpStack::Anystream,
        int captureRank = 1)
      : d(new QPcapHttpFilterData(regex, direction, captureRank)) { }
    inline QPcapHttpFilter(const QPcapHttpFilter &other) : d(other.d) { }
    const QRegExp re() const { return d->_re; }
    QPcapHttpStack::QPcapHttpDirection direction() const {
      return d->_direction; }
    int captureRank() const { return d->_captureRank; }
  };

  QList<QPcapHttpFilter> _filters;

public:
  explicit HttpCustomFieldAnalyzer(QObject *parent = 0);
  void connectToSource(QPcapHttpStack *stack);
  inline void addFilter(
      QString regex, QPcapHttpStack::QPcapHttpDirection direction
      = QPcapHttpStack::Anystream,
      int captureRank = 1) {
    _filters.append(QPcapHttpFilter(QRegExp(regex), direction, captureRank));
  }
  inline void clearFilters() { _filters.clear(); }

signals:
  void httpHit(QPcapHttpHit hit);
  void captureStarted();
  void captureFinished();

public slots:
  void rawHttpHit(QPcapHttpHit hit, QByteArray upstreamData,
                  QByteArray downstreamData);

private slots:
  void waitForThreadedComputation();

private:
  void compute(QPcapHttpHit hit, QByteArray upstreamData,
               QByteArray downstreamData);
};

#endif // HTTPCUSTOMFIELDANALYZER_H
