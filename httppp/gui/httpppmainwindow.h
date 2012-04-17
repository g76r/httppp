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

namespace Ui {
class HttpppMainWindow;
}

class HttpppMainWindow : public QMainWindow {
  Q_OBJECT
private:
  Ui::HttpppMainWindow *ui;
  QThread _thread1, _thread2, _thread3;
  QPcapEngine _pcapEngine;
  QPcapEthernetStack _etherStack;
  QPcapIPv4Stack _ipStack;
  QPcapTcpStack _tcpStack;
  QPcapHttpStack _httpStack;
  TcpConversationModel _tcpConversationModel;
  TcpPacketModel _tcpPacketModel;
  HttpHitModel _httpHitModel;
  QSortFilterProxyModel _tcpConversationProxyModel, _httpHitProxyModel;
  QString _lastLoadedFilename;

public:
  explicit HttpppMainWindow(QWidget *parent = 0);
  void loadFile(QString filename);
  ~HttpppMainWindow();

private slots:
  void changePanelVisibility(bool visible);
  void forwardSelection(QModelIndex currentIndex);

public slots:
  void loadFileDialog();
  void reload();
  void startCaptureDialog();
  void httpHitsToCsvDialog();

private:
  void selectConversationInConversations(QPcapTcpConversation conversation);
  void selectConversationInPackets(QPcapTcpConversation conversation);
  void selectPacketInPackets(QPcapTcpPacket packet);
  void showDetails(QPcapTcpConversation conversation);
  void showDetails(QPcapTcpConversation conversation, QPcapTcpPacket packet);
  static void staticMessageHandler(QtMsgType type, const char *msg);
  void messageHandler(QtMsgType type, const char *msg);
};

#endif // HTTPPPMAINWINDOW_H
