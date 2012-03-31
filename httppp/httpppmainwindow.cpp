#include "httpppmainwindow.h"
#include "ui_httpppmainwindow.h"

HttpppMainWindow::HttpppMainWindow(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::HttpppMainWindow) {
  ui->setupUi(this);
  ui->tcpConversationsView->setModel(&_tcpConversationModel);
  ui->tcpPacketsView->setModel(&_tcpPacketsModel);
  connect(&_pcapEngine, SIGNAL(layer1PacketReceived(PcapLayer1Packet)),
                   &_etherStack, SLOT(layer1PacketReceived(PcapLayer1Packet)));
  connect(&_etherStack, SIGNAL(layer2PacketReceived(PcapLayer2Packet)),
                   &_ipStack, SLOT(layer2PacketReceived(PcapLayer2Packet)));
  connect(&_ipStack, SIGNAL(ipv4PacketReceived(PcapIPv4Packet)),
                   &_tcpStack, SLOT(ipPacketReceived(PcapIPv4Packet)));
  connect(&_tcpStack, SIGNAL(conversationStarted(PcapTcpConversation)),
          &_tcpConversationModel, SLOT(addConversation(PcapTcpConversation)));
  connect(&_tcpStack, SIGNAL(tcpUpstreamPacket(PcapTcpPacket,PcapTcpConversation)),
          &_tcpPacketsModel, SLOT(addTcpUpstreamPacket(PcapTcpPacket,PcapTcpConversation)));
  connect(&_tcpStack, SIGNAL(tcpDownstreamPacket(PcapTcpPacket,PcapTcpConversation)),
          &_tcpPacketsModel, SLOT(addTcpDownstreamPacket(PcapTcpPacket,PcapTcpConversation)));
}

void HttpppMainWindow::loadFilename(QString filename) {
  _pcapEngine.loadFile(filename);
  _pcapEngine.start();
}

HttpppMainWindow::~HttpppMainWindow() {
  delete ui;
}

void HttpppMainWindow::changePanelVisibility(bool visible) {
  if (sender() == ui->pushTcpConversations) {
    ui->panelTcpConversations->setVisible(visible);
  } else if (sender() == ui->pushTcpPackets) {
    ui->panelTcpPackets->setVisible(visible);
  } else if (sender() == ui->pushHttpRequests) {
    ui->panelHttpRequests->setVisible(visible);
  } else if (sender() == ui->pushDetails) {
    ui->panelDetails->setVisible(visible);
  }
}

void HttpppMainWindow::forwardSelection(QModelIndex activated) {
  if (sender() == ui->tcpConversationsView) {
    //qDebug() << "forwardSelection from TCP conv";
    PcapTcpConversation c = _tcpConversationModel.conversation(activated);
    selectConversationInPackets(c);
    showDetails(c);
  } else if (sender() == ui->tcpPacketsView) {
    //qDebug() << "forwardSelection from TCP packets";
    PcapTcpPacket p = _tcpPacketsModel.packet(activated);
    PcapTcpConversation c = _tcpPacketsModel.conversation(activated);
    selectConversationInConversations(c);
    //qDebug() << "sender = tcpPacketView" << p.isNull() << p << c;
    if (p.isNull())
      showDetails(c);
    else
      showDetails(p);
  } else if (sender() == ui->httpRequestsView) {
    //qDebug() << "forwardSelection from HTTP requests";
    // TODO
  } else
    //qDebug() << "forwardSelection from nowhere";
    ;
}

void HttpppMainWindow::selectConversationInConversations(
    PcapTcpConversation conversation) {
  if (ui->tcpConversationsView->isVisible()) {
    int r = _tcpConversationModel.row(conversation);
    if (r >= 0)
      ui->tcpConversationsView->selectRow(r);
  }
}

void HttpppMainWindow::selectConversationInPackets(
    PcapTcpConversation conversation) {
  if (ui->tcpPacketsView->isVisible()) {    
    QModelIndex i = _tcpPacketsModel.index(conversation);
    QItemSelectionModel *sm = ui->tcpPacketsView->selectionModel();
    if (sm) {
      QModelIndexList mil = sm->selectedIndexes();
      if (!mil.contains(i)) {
        sm->select(i, QItemSelectionModel::Clear | QItemSelectionModel::Select
                   | QItemSelectionModel::Current);
      }
    }
  }
}

void HttpppMainWindow::showDetails(PcapTcpConversation conversation) {
  //qDebug() << "show details" << ui->detailsView->isVisible() << conversation
  //         << conversation.packets().size();
  if (ui->detailsView->isVisible()) {
    QString text;
    //ui->detailsView->clear();
    foreach (PcapTcpPacket p, conversation.packets()) {
      bool upstream = conversation.matchesSameStream(p);
      text.append(QString("<p><font color=%1>%2")
                  .arg(upstream ? "red" : "blue")
                  .arg(p.payload().constData())); // TODO escape
    }
    ui->detailsView->setHtml(text);
  }
}

void HttpppMainWindow::showDetails(PcapTcpPacket packet) {
  if (ui->detailsView->isVisible()) {
    ui->detailsView->setHtml(QString("<p>%1<p>%2").arg(packet.english())
                             .arg(packet.payload().constData())); // TODO escape
  }
}
