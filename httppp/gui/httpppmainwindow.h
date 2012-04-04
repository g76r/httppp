#ifndef HTTPPPMAINWINDOW_H
#define HTTPPPMAINWINDOW_H

#include <QMainWindow>
#include "qpcapengine.h"
#include "qpcapethernetstack.h"
#include "qpcapipv4stack.h"
#include "qpcaptcpstack.h"
#include "tcpconversationmodel.h"
#include "tcppacketmodel.h"

namespace Ui {
class HttpppMainWindow;
}

class HttpppMainWindow : public QMainWindow {
  Q_OBJECT
private:
  Ui::HttpppMainWindow *ui;
  QPcapEngine _pcapEngine;
  QPcapEthernetStack _etherStack;
  QPcapIPv4Stack _ipStack;
  QPcapTcpStack _tcpStack;
  //Printer p;
  TcpConversationModel _tcpConversationModel;
  TcpPacketModel _tcpPacketsModel;

public:
  explicit HttpppMainWindow(QWidget *parent = 0);
  void loadFile(QString filename);
  ~HttpppMainWindow();

private slots:
  void changePanelVisibility(bool visible);
  void forwardSelection(QModelIndex activated);

public slots:
  void loadFileDialog();
  void startCaptureDialog();

private:
  void selectConversationInConversations(QPcapTcpConversation conversation);
  void selectConversationInPackets(QPcapTcpConversation conversation);
  void showDetails(QPcapTcpConversation conversation);
  void showDetails(QPcapTcpPacket packet);
};

#endif // HTTPPPMAINWINDOW_H
