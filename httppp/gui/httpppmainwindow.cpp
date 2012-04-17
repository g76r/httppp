#include "httpppmainwindow.h"
#include "ui_httpppmainwindow.h"
#include <QFileDialog>
#include "util/csvwriter.h"
#include <QMetaObject>

static HttpppMainWindow *instance;

HttpppMainWindow::HttpppMainWindow(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::HttpppMainWindow) {
  ui->setupUi(this);
  instance = this;
  qInstallMsgHandler(staticMessageHandler);
  ui->switchField1->appendPixmap(":/icons/up.svg");
  ui->switchField1->appendPixmap(":/icons/down.svg");
  ui->switchField1->appendPixmap(":/icons/updown.svg");
  ui->switchField2->appendPixmap(":/icons/up.svg");
  ui->switchField2->appendPixmap(":/icons/down.svg");
  ui->switchField2->appendPixmap(":/icons/updown.svg");
  ui->switchField3->appendPixmap(":/icons/up.svg");
  ui->switchField3->appendPixmap(":/icons/down.svg");
  ui->switchField3->appendPixmap(":/icons/updown.svg");
  _etherStack.moveToThread(&_thread1);
  _ipStack.moveToThread(&_thread1);
  _tcpStack.moveToThread(&_thread1);
  _httpStack.moveToThread(&_thread1);
  _thread1.start();
  //_thread2.start();
  //_thread3.start();
  _tcpConversationProxyModel.setSourceModel(&_tcpConversationModel);
  ui->tcpConversationsView->setModel(&_tcpConversationProxyModel);
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

HttpppMainWindow::~HttpppMainWindow() {
  qInstallMsgHandler(0);
  delete ui;
}

void HttpppMainWindow::loadFile(QString filename) {
  QPcapTcpPacket::resetPacketCounter();
  QPcapTcpConversation::resetConversationCounter();
  _tcpConversationModel.clear();
  _tcpPacketModel.clear();
  _httpHitModel.clear();
  _tcpStack.reset();
  _httpStack.clearFilters();
  if (!ui->comboField1->currentText().isEmpty())
    _httpStack.addFilter(
          ui->comboField1->currentText(),
          (QPcapHttpStack::QPcapHttpDirection)ui->switchField1->currentIndex());
  if (!ui->comboField2->currentText().isEmpty())
    _httpStack.addFilter(
          ui->comboField2->currentText(),
          (QPcapHttpStack::QPcapHttpDirection)ui->switchField2->currentIndex());
  if (!ui->comboField3->currentText().isEmpty())
    _httpStack.addFilter(
          ui->comboField3->currentText(),
          (QPcapHttpStack::QPcapHttpDirection)ui->switchField3->currentIndex());
  _pcapEngine.loadFile(filename);
  _pcapEngine.start();
}

void HttpppMainWindow::staticMessageHandler(QtMsgType type, const char *msg) {
  if (instance)
    instance->messageHandler(type, msg);
}

void HttpppMainWindow::messageHandler(QtMsgType type, const char *msg) {
  if (!ui->pushEnableMessages->isChecked())
    return;
  QString prefix = QString::number(type);
  switch (type) {
  case QtDebugMsg:
    prefix = "D";
    break;
  case QtWarningMsg:
    prefix = "W";
    break;
  case QtCriticalMsg:
    prefix = "C";
    break;
  case QtFatalMsg:
    prefix = "F";
  }
  QString text = QString("%1: %2").arg(prefix).arg(msg?:"<null>");
  QMetaObject::invokeMethod(ui->messages, "appendPlainText",
                            Qt::QueuedConnection, Q_ARG(QString, text));
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
  } else if (sender() == ui->pushMessages) {
    ui->panelMessages->setVisible(visible);
  }
}

void HttpppMainWindow::forwardSelection(QModelIndex currentIndex) {
  if (sender() == ui->tcpConversationsView) {
    //qDebug() << "forwardSelection from TCP conv";
    QPcapTcpConversation c = _tcpConversationModel.conversation(
          _tcpConversationProxyModel.mapToSource(currentIndex));
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
      showDetails(c, p);
  } else if (sender() == ui->httpHitsView) {
    QPcapHttpHit hit = _httpHitModel.hit(
          _httpHitProxyModel.mapToSource(currentIndex));
    selectConversationInConversations(hit.conversation());
    selectPacketInPackets(hit.firstRequestPacket());
    // LATER should display all packets in HTTP hit rather than first one
    showDetails(QPcapTcpConversation(), hit.firstRequestPacket());
  } else {
    //qDebug() << "forwardSelection from nowhere";
  }
}

void HttpppMainWindow::selectConversationInConversations(
    QPcapTcpConversation conversation) {
  if (ui->tcpConversationsView->isVisible()) {
    QModelIndex i = _tcpConversationModel.index(conversation);
    if (i.isValid()) {
      ui->tcpConversationsView->scrollTo(
            _tcpConversationProxyModel.mapFromSource(i));
      ui->tcpConversationsView->selectRow(
            _tcpConversationProxyModel.mapFromSource(i).row());
    }
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
  if (ui->detailsView->isVisible()) {
    QString text;
    foreach (QPcapTcpPacket p, conversation.packets()) {
      QByteArray ba = p.payload();
      ba.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
          .replace("\n", "<br/>");
      bool upstream = conversation.matchesSameStream(p);
      text.append(QString("<p><font color=%1><pre>%2</pre></font></p>")
                  .arg(upstream ? "red" : "blue").arg(ba.constData()));
    }
    ui->detailsView->setHtml(text);
  }
}

void HttpppMainWindow::showDetails(QPcapTcpConversation conversation,
                                   QPcapTcpPacket packet) {
  if (ui->detailsView->isVisible()) {
    QByteArray ba = packet.payload();
    ba.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
        .replace("\n", "<br/>");
    bool upstream = conversation.isNull()
        || conversation.matchesSameStream(packet);
    ui->detailsView->setHtml(
          QString("<p>%1</p><p><font color=%2><pre>%3</pre></font></p>")
          .arg(packet.english()).arg(upstream ? "red" : "blue")
          .arg(ba.constData()));
  }
}

void HttpppMainWindow::loadFileDialog() {
  QString filename;
  filename = QFileDialog ::getOpenFileName(window(), tr("Load Capture File"),
                                           "", tr("Capture files (*)"), 0);
  if (filename.size()) {
    loadFile(_lastLoadedFilename = filename);
  }
}

void HttpppMainWindow::reload() {
  if (!_lastLoadedFilename.isEmpty())
    loadFile(_lastLoadedFilename);
}

void HttpppMainWindow::startCaptureDialog() {
  // LATER add direct capture feature
}

void HttpppMainWindow::httpHitsToCsvDialog() {
  QString filename;
  filename = QFileDialog ::getSaveFileName(window(), tr("Save CSV File"),
                                           "", tr("CSV files (*)"), 0);
  if (filename.size()) {
    CsvWriter::write(_httpHitModel, filename);
  }
}
