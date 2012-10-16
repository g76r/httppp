#include "httpcustomfieldanalyzer.h"
#include <QtConcurrentRun>
#include <QTimer>

HttpCustomFieldAnalyzer::HttpCustomFieldAnalyzer(QObject *parent)
  : QObject(parent) {
}

void HttpCustomFieldAnalyzer::connectToSource(QPcapHttpStack *stack) {
  connect(stack, SIGNAL(httpHit(QPcapHttpHit,QByteArray,QByteArray)),
          this, SLOT(rawHttpHit(QPcapHttpHit,QByteArray,QByteArray)));
  connect(stack, SIGNAL(captureFinished()),
          this, SLOT(waitForThreadedComputation()));
  connect(stack, SIGNAL(captureStarted()), this, SIGNAL(captureStarted()));
}

void HttpCustomFieldAnalyzer::rawHttpHit(QPcapHttpHit hit,
                                         QByteArray upstreamData,
                                         QByteArray downstreamData) {
#ifndef MONOTHREAD
  QtConcurrent::run(this, &HttpCustomFieldAnalyzer::compute, hit, upstreamData,
                    downstreamData);
#else
  compute(hit, upstreamData, downstreamData);
#endif
}

void HttpCustomFieldAnalyzer::compute(QPcapHttpHit hit,
                                      QByteArray upstreamData,
                                      QByteArray downstreamData) {
  QList<QPcapHttpFilter> filters = _filters; // copy for staying thread safe
  if (filters.size()) {
    char cookedUpstream[upstreamData.size()+1];
    char cookedDownstream[downstreamData.size()+1];
    int i;
    for (i = 0; i < upstreamData.size(); ++i) {
      char ch = upstreamData.at(i);
      if (ch < 32)
        ch = ' ';
      cookedUpstream[i] = ch;
    }
    cookedUpstream[i] = 0;
    for (i = 0; i < downstreamData.size(); ++i) {
      char ch = downstreamData.at(i);
      if (ch < 32)
        ch = ' ';
      cookedDownstream[i] = ch;
    }
    cookedDownstream[i] = 0;
    for (int i = 0; i < filters.size(); ++i) {
      QPcapHttpFilter f = filters[i];
      QRegExp re = f.re();
      //qDebug() << "testing upstream custom field" << i << f.direction()
      //         << re.pattern() << cookedUpstream.size()
      //         << cookedDownstream.size();
      if ((f.direction() == QPcapHttpStack::Upstream
           || f.direction() == QPcapHttpStack::Anystream)) {
        if (re.indexIn(cookedUpstream) >= 0) {
          //qDebug() << "found upstream custom field" << i
          //         << re.cap(f.captureRank());
          hit.setCustomField(i, re.cap(f.captureRank()));
        }
      }
      if ((f.direction() == QPcapHttpStack::Downstream
           || f.direction() == QPcapHttpStack::Anystream)
          && hit.customField(i).isNull()) {
        if (re.indexIn(cookedDownstream) >= 0) {
          //qDebug() << "found downstream custom field" << i
          //         << re.cap(f.captureRank());
          hit.setCustomField(i, re.cap(f.captureRank()));
        }
      }
    }
  }
  emit httpHit(hit);
}

void HttpCustomFieldAnalyzer::waitForThreadedComputation() {
#ifndef MONOTHREAD
  if (QThreadPool::globalInstance()->activeThreadCount() > 0)
    QTimer::singleShot(500, this, SLOT(waitForThreadedComputation()));
  else
#endif
    emit captureFinished();
}
