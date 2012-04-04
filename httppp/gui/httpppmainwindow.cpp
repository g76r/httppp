#include "httpppmainwindow.h"
#include "ui_httpppmainwindow.h"
#include <QFileDialog>
#include "util/csvwriter.h"

HttpppMainWindow::HttpppMainWindow(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::HttpppMainWindow) {
  ui->setupUi(this);
  ui->tcpConversationsView->setModel(&_tcpConversationModel);
  ui->tcpPacketsView->setModel(&_tcpPacketModel);
  _httpHitProxyModel.setSourceModel(&_httpHitModel);
  ui->httpHitsView->setModel(&_httpHitProxyModel);
  connect(&_pcapEngine, SIGNAL(layer1PacketReceived(QPcapLayer1Packet)),
                   &_etherStack, SLOT(layer1PacketReceived(QPcapLayer1Packet)));
  connect(&_etherStack, SIGNAL(layer2PacketReceived(QPcapLayer2Packet)),
                   &_ipStack, SLOT(layer2PacketReceived(QPcapLayer2Packet)));
  connect(&_ipStack, SIGNAL(ipv4PacketReceived(QPcapIPv4Packet)),
                   &_tcpStack, SLOT(ipPacketReceived(QPcapIPv4Packet)));
  connect(&_tcpStack, SIGNAL(conversationStarted(QPcapTcpConversation)),
          &_tcpConversationModel, SLOT(addConversation(QPcapTcpConversation)));
  connect(&_tcpStack, SIGNAL(tcpUpstreamPacket(QPcapTcpPacket,QPcapTcpConversation)),
          &_tcpPacketModel, SLOT(addTcpUpstreamPacket(QPcapTcpPacket,QPcapTcpConversation)));
  connect(&_tcpStack, SIGNAL(tcpDownstreamPacket(QPcapTcpPacket,QPcapTcpConversation)),
          &_tcpPacketModel, SLOT(addTcpDownstreamPacket(QPcapTcpPacket,QPcapTcpConversation)));
  _httpStack.connectToLowerStack(_tcpStack);
  connect(&_httpStack, SIGNAL(httpHit(QPcapHttpHit)),
          &_httpHitModel, SLOT(addHit(QPcapHttpHit)));
}

void HttpppMainWindow::loadFile(QString filename) {
  QPcapTcpConversation::resetConversationCounter();
  _tcpConversationModel.clear();
  _tcpPacketModel.clear();
  _httpHitModel.clear();
  _tcpStack.reset();
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
  } else if (sender() == ui->pushHttpHits) {
    ui->panelHttpHits->setVisible(visible);
  } else if (sender() == ui->pushDetails) {
    ui->panelDetails->setVisible(visible);
  }
}

void HttpppMainWindow::forwardSelection(QModelIndex currentIndex) {
  if (sender() == ui->tcpConversationsView) {
    //qDebug() << "forwardSelection from TCP conv";
    QPcapTcpConversation c = _tcpConversationModel.conversation(currentIndex);
    selectConversationInPackets(c);
    showDetails(c);
  } else if (sender() == ui->tcpPacketsView) {
    //qDebug() << "forwardSelection from TCP packets";
    QPcapTcpPacket p = _tcpPacketModel.packet(currentIndex);
    QPcapTcpConversation c = _tcpPacketModel.conversation(currentIndex);
    selectConversationInConversations(c);
    //qDebug() << "sender = tcpPacketView" << p.isNull() << p << c;
    if (p.isNull())
      showDetails(c);
    else
      showDetails(p);
  } else if (sender() == ui->httpHitsView) {
    QPcapHttpHit hit = _httpHitModel.hit(currentIndex);
    selectConversationInConversations(hit.conversation());
    selectPacketInPackets(hit.firstRequestPacket());
    showDetails(hit.firstRequestPacket()); // LATER do something better
  } else {
    //qDebug() << "forwardSelection from nowhere";
  }
}

void HttpppMainWindow::selectConversationInConversations(
    QPcapTcpConversation conversation) {
  if (ui->tcpConversationsView->isVisible()) {
    int r = _tcpConversationModel.row(conversation);
    QModelIndex i = _tcpConversationModel.index(r, 0, QModelIndex());
    ui->tcpConversationsView->scrollTo(i);
    if (r >= 0)
      ui->tcpConversationsView->selectRow(r);
  }
}

void HttpppMainWindow::selectConversationInPackets(
    QPcapTcpConversation conversation) {
  if (ui->tcpPacketsView->isVisible()) {
    QModelIndex i = _tcpPacketModel.index(conversation);
    ui->tcpPacketsView->scrollTo(i);
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

void HttpppMainWindow::selectPacketInPackets(QPcapTcpPacket packet) {
  if (ui->tcpPacketsView->isVisible()) {
    QModelIndex i = _tcpPacketModel.index(packet);
    ui->tcpPacketsView->setExpanded(_tcpPacketModel.parent(i), true);
    ui->tcpPacketsView->scrollTo(i);
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

void HttpppMainWindow::showDetails(QPcapTcpConversation conversation) {
  //qDebug() << "show details" << ui->detailsView->isVisible() << conversation
  //         << conversation.packets().size();
  if (ui->detailsView->isVisible()) {
    QString text;
    //ui->detailsView->clear();
    foreach (QPcapTcpPacket p, conversation.packets()) {
      bool upstream = conversation.matchesSameStream(p);
      text.append(QString("<p><font color=%1>%2")
                  .arg(upstream ? "red" : "blue")
                  .arg(p.payload().constData())); // TODO escape
    }
    ui->detailsView->setHtml(text);
  }
}

void HttpppMainWindow::showDetails(QPcapTcpPacket packet) {
  if (ui->detailsView->isVisible()) {
    ui->detailsView->setHtml(QString("<p>%1<p>%2").arg(packet.english())
                             .arg(packet.payload().constData())); // TODO escape
  }
}

void HttpppMainWindow::loadFileDialog() {
  QString filename;
  filename = QFileDialog ::getOpenFileName(window(), tr("Load Capture File"),
                                           "", tr("Capture files (*)"), 0);
  if (filename.size()) {
    loadFile(filename);
  }
}

void HttpppMainWindow::startCaptureDialog() {
  // LATER
}

void HttpppMainWindow::httpHitsToCsvDialog() {
  QString filename;
  filename = QFileDialog ::getSaveFileName(window(), tr("Save CSV File"),
                                           "", tr("CSV files (*)"), 0);
  if (filename.size()) {
    CsvWriter::write(_httpHitModel, filename);
  }
}
