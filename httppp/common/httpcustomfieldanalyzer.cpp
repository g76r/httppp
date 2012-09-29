#include "httpcustomfieldanalyzer.h"

HttpCustomFieldAnalyzer::HttpCustomFieldAnalyzer(QObject *parent)
  : QObject(parent) {
}

void HttpCustomFieldAnalyzer::connectToLowerStack(QPcapHttpStack *stack) {
  connect(stack, SIGNAL(httpHit(QPcapHttpHit,QByteArray,QByteArray)),
          this, SLOT(rawHttpHit(QPcapHttpHit,QByteArray,QByteArray)));
}

void HttpCustomFieldAnalyzer::rawHttpHit(QPcapHttpHit hit,
                                         QByteArray upstreamData,
                                         QByteArray donwstreamData) {
  //qDebug() << "looking for custom fields" << _filters.size();
  QByteArray cookedUpstream, cookedDownstream;
  for (int i = 0; i < upstreamData.size(); ++i) {
    char ch = upstreamData.at(i);
    if (ch < 32)
      ch = ' ';
    cookedUpstream.append(&ch, 1);
  }
  for (int i = 0; i < donwstreamData.size(); ++i) {
    char ch = donwstreamData.at(i);
    if (ch < 32)
      ch = ' ';
    cookedDownstream.append(&ch, 1);
  }
  for (int i = 0; i < _filters.size(); ++i) {
    QPcapHttpFilter f = _filters[i];
    QRegExp re = f.re();
    //qDebug() << "testing upstream custom field" << i << f.direction()
    //         << re.pattern() << cookedUpstream.size()
    //         << cookedDownstream.size();
    if ((f.direction() == QPcapHttpStack::Upstream
         || f.direction() == QPcapHttpStack::Anystream)) {
      if (re.indexIn(cookedUpstream.constData()) >= 0) {
        //qDebug() << "found upstream custom field" << i
        //         << re.cap(f.captureRank());
        hit.setCustomField(i, re.cap(f.captureRank()));
      }
    }
    if ((f.direction() == QPcapHttpStack::Downstream
         || f.direction() == QPcapHttpStack::Anystream)
        && hit.customField(i).isNull()) {
      if (re.indexIn(cookedDownstream.constData()) >= 0) {
        //qDebug() << "found downstream custom field" << i
        //         << re.cap(f.captureRank());
        hit.setCustomField(i, re.cap(f.captureRank()));
      }
    }
  }
  emit httpHit(hit);
}
