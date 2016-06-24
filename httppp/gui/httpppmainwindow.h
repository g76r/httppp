#ifndef HTTPPPMAINWINDOW_H
#define HTTPPPMAINWINDOW_H

#include <QMainWindow>
#include "qpcapengine.h"
#include "qpcapethernetstack.h"
#include "qpcapipv4stack.h"
#include "qpcaptcpstack.h"
#include "qpcaphttpstack.h"
#include "tcpconversationmodel.h"
#include "tcppacketmodel.h"
#include "httphitmodel.h"
#include <QSortFilterProxyModel>
#include <QThread>
#include "common/httpcustomfieldanalyzer.h"
#include "tcpdata.h"
#include "httpdata.h"

namespace Ui {
class HttpppMainWindow;
}

class HttpppMainWindow : public QMainWindow {
  Q_OBJECT
private:
  Ui::HttpppMainWindow *ui;
  QThread *_pcapThread, *_tcpHttpThread;
  QPcapEngine *_pcapEngine;
  QPcapEthernetStack *_etherStack;
  QPcapIPv4Stack *_ipStack;
  QPcapTcpStack *_tcpStack;
  QPcapHttpStack *_httpStack;
  HttpCustomFieldAnalyzer *_customFieldAnalyzer;
  TcpData *_tcpData;
  TcpConversationModel *_tcpConversationModel;
  TcpPacketModel *_tcpPacketModel;
  HttpData *_httpData;
  HttpHitModel *_httpHitModel;
  QSortFilterProxyModel *_tcpConversationProxyModel, *_httpHitProxyModel;
  QString _lastLoadedFilename;
  bool _firstShow, _currentlyLoading;

public:
  explicit HttpppMainWindow(QWidget *parent = 0);
  ~HttpppMainWindow();
  void loadFile(QString filename);
  void showEvent(QShowEvent *e);

public slots:
  void loadFileDialog();
  void reload();
  void startCaptureDialog();
  void httpHitsToCsvDialog();

private slots:
  void toggleMessagesPanel();
  void forwardSelection(QModelIndex currentIndex);
  void updatePacketsCount(unsigned long count);
  void updateHitsCount(unsigned long count);
  void captureFinished();
  void releaseThread();

private:
  void selectConversationInConversations(QPcapTcpConversation conversation);
  void selectConversationInPackets(QPcapTcpConversation conversation);
  void selectPacketInPackets(QPcapTcpPacket packet);
  void showDetails(QPcapTcpConversation conversation);
  void showDetails(QPcapTcpConversation conversation, QPcapTcpPacket packet);
  static void staticMessageHandler(
      QtMsgType type, const QMessageLogContext &context, const QString &msg);
  Q_INVOKABLE void messageHandler(QString text);
};

#endif // HTTPPPMAINWINDOW_H
