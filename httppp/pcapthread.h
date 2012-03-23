#ifndef PCAPTHREAD_H
#define PCAPTHREAD_H

#include <QThread>

class PcapEngine;

class PcapThread : public QThread {
  Q_OBJECT
private:
  PcapEngine *_engine;

public:
  explicit PcapThread(PcapEngine *engine, QObject *parent = 0);
  void run();

private:
  Q_DISABLE_COPY(PcapThread)
};

#endif // PCAPTHREAD_H
