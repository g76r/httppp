#include "httpppmainwindow.h"
#include "ui_httpppmainwindow.h"
#include <QFileDialog>
#include "util/csvwriter.h"
#include <QMetaObject>

static HttpppMainWindow *instance;

HttpppMainWindow::HttpppMainWindow(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::HttpppMainWindow), _firstShow(true) {
  ui->setupUi(this);
  ui->labelLoading->setVisible(false);
  ui->panelsSplitter->setStretchFactor(0, 0);
  ui->panelsSplitter->setStretchFactor(1, 0);
  ui->panelsSplitter->setStretchFactor(2, 1);
  ui->panelsSplitter->setStretchFactor(3, 0);
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
  _pcapEngine = new QPcapEngine;
  _etherStack = new QPcapEthernetStack(0, _pcapEngine);
  // ethernet and ip stacks are almost straightforward and therefore more
  // balanced in the engine thread than the tcp/http one
  // more than 3 threads (engine + tcp/http + main/gui/models) have been
  // tested to be less efficient on a 2 cores 4 threads Core i5 PC with SSD
  _etherStack->moveToThread(_pcapEngine->thread());
  _ipStack = new QPcapIPv4Stack(0, _etherStack);
  _ipStack->moveToThread(_pcapEngine->thread());
  _tcpStack = new QPcapTcpStack(0, _ipStack);
  _tcpStack->moveToThread(&_thread1);
  _httpStack = new QPcapHttpStack(0, _tcpStack);
  // tcp and http stacks must share same thread because of discardXXXBuffer
  _httpStack->moveToThread(_tcpStack->thread());
  _tcpConversationProxyModel.setSourceModel(&_tcpConversationModel);
  ui->tcpConversationsView->setModel(&_tcpConversationProxyModel);
  ui->tcpPacketsView->setModel(&_tcpPacketModel);
  _httpHitProxyModel.setSourceModel(&_httpHitModel);
  ui->httpHitsView->setModel(&_httpHitProxyModel);
  connect(_pcapEngine, SIGNAL(packetsCountTick(ulong)),
          this, SLOT(updatePacketsCount(ulong)));
  connect(_pcapEngine, SIGNAL(captureFinished()),
          this, SLOT(captureFinished()));
  connect(_tcpStack, SIGNAL(conversationStarted(QPcapTcpConversation)),
          &_tcpConversationModel, SLOT(addConversation(QPcapTcpConversation)));
  connect(_tcpStack, SIGNAL(tcpUpstreamPacket(QPcapTcpPacket,QPcapTcpConversation)),
          &_tcpPacketModel, SLOT(addTcpUpstreamPacket(QPcapTcpPacket,QPcapTcpConversation)));
  connect(_tcpStack, SIGNAL(tcpDownstreamPacket(QPcapTcpPacket,QPcapTcpConversation)),
          &_tcpPacketModel, SLOT(addTcpDownstreamPacket(QPcapTcpPacket,QPcapTcpConversation)));
  connect(_httpStack, SIGNAL(httpHit(QPcapHttpHit)),
          &_httpHitModel, SLOT(addHit(QPcapHttpHit)));
  connect(_httpStack, SIGNAL(hitsCountTick(ulong)),
          this, SLOT(updateHitsCount(ulong)));
  _thread1.start();
}

HttpppMainWindow::~HttpppMainWindow() {
  _httpStack->deleteLater();
  _tcpStack->deleteLater();
  _ipStack->deleteLater();
  _etherStack->deleteLater();
  _pcapEngine->deleteLater();
  _thread1.exit();
  _thread1.wait(200);
  qInstallMsgHandler(0);
  delete ui;
}

void HttpppMainWindow::loadFile(QString filename) {
  if (_pcapEngine->isRunning()) { // LATER this is not sufficient since there are asynchronous signals
    qDebug() << "cannot load file when pcap engine is already running";
    return;
  }
  _tcpConversationModel.clear();
  _tcpPacketModel.clear();
  _httpHitModel.clear();
  _tcpConversationProxyModel.clear();
  _httpHitProxyModel.clear();
  _httpStack->clearFilters();
  if (!ui->comboField1->currentText().isEmpty())
    _httpStack->addFilter(
          ui->comboField1->currentText(),
          (QPcapHttpStack::QPcapHttpDirection)ui->switchField1->currentIndex());
  if (!ui->comboField2->currentText().isEmpty())
    _httpStack->addFilter(
          ui->comboField2->currentText(),
          (QPcapHttpStack::QPcapHttpDirection)ui->switchField2->currentIndex());
  if (!ui->comboField3->currentText().isEmpty())
    _httpStack->addFilter(
          ui->comboField3->currentText(),
          (QPcapHttpStack::QPcapHttpDirection)ui->switchField3->currentIndex());
  ui->labelLoading->setVisible(true);
  _pcapEngine->loadFile(filename);
  _pcapEngine->start();
}

void HttpppMainWindow::staticMessageHandler(QtMsgType type, const char *msg) {
  if (!instance)
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
  QMetaObject::invokeMethod(instance, "messageHandler",
                            Qt::QueuedConnection, Q_ARG(QString, text));
}

void HttpppMainWindow::messageHandler(QString text) {
  if (!ui->pushEnableMessages->isChecked())
    return;
  ui->messages->appendPlainText(text);
  if (ui->pushAutoOpenMessages->isChecked()) {
    QList<int> sizes = ui->mainSplitter->sizes();
    if (sizes[1] == 0) {
      sizes[1] = ui->panelMessages->sizeHint().height();
      sizes[0] = ui->mainSplitter->sizeHint().height()
          - ui->panelMessages->sizeHint().height();
      ui->mainSplitter->setSizes(sizes);
    }
  }
}

void HttpppMainWindow::showEvent(QShowEvent *e) {
  QMainWindow::showEvent(e);
  if (_firstShow) {
    toggleMessagesPanel();
    _firstShow = false;
  }
}

void HttpppMainWindow::toggleMessagesPanel() {
  QList<int> sizes = ui->mainSplitter->sizes();
  if (sizes[1] == 0) {
    sizes[1] = ui->panelMessages->sizeHint().height();
    sizes[0] = ui->mainSplitter->sizeHint().height()
        - ui->panelMessages->sizeHint().height();
    ui->mainSplitter->setSizes(sizes);
  } else {
    sizes[1] = 0;
    ui->mainSplitter->setSizes(sizes);
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
          .arg(packet.toText()).arg(upstream ? "red" : "blue")
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

void HttpppMainWindow::updatePacketsCount(unsigned long count) {
  ui->labelPacketsCount->setText(QString::number(count));
}

void HttpppMainWindow::updateHitsCount(unsigned long count) {
  ui->labelHitsCount->setText(QString::number(count));
}

void HttpppMainWindow::captureFinished() {
  ui->labelLoading->setVisible(false);
}
