#ifndef HTTPPPMAINWINDOW_H
#define HTTPPPMAINWINDOW_H

#include <QMainWindow>
#include "pcapengine.h"
#include "pcapethernetstack.h"
#include "pcapipv4stack.h"
#include "pcaptcpstack.h"
#include "tcpconversationmodel.h"
#include "tcppacketmodel.h"

namespace Ui {
class HttpppMainWindow;
}

class HttpppMainWindow : public QMainWindow {
  Q_OBJECT
private:
  Ui::HttpppMainWindow *ui;
  PcapEngine _pcapEngine;
  PcapEthernetStack _etherStack;
  PcapIPv4Stack _ipStack;
  PcapTcpStack _tcpStack;
  //Printer p;
  TcpConversationModel _tcpConversationModel;
  TcpPacketModel _tcpPacketsModel;

public:
  explicit HttpppMainWindow(QWidget *parent = 0);
  void loadFilename(QString filename);
  ~HttpppMainWindow();

private slots:
  void changePanelVisibility(bool visible);
  void forwardSelection(QModelIndex activated);

private:
  void selectConversationInConversations(PcapTcpConversation conversation);
  void selectConversationInPackets(PcapTcpConversation conversation);
  void showDetails(PcapTcpConversation conversation);
  void showDetails(PcapTcpPacket packet);
};

#endif // HTTPPPMAINWINDOW_H
