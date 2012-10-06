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
  _tcpHttpThread = new QThread(this);
  _regexThread = new QThread(this);
  _ipStack = new QPcapIPv4Stack(0, _etherStack);
#ifndef MONOTHREAD_PROFILING
  // ethernet and ip stacks are almost straightforward and therefore more
  // balanced in the engine thread than the tcp/http one
  _etherStack->moveToThread(_pcapEngine->thread());
  _ipStack->moveToThread(_pcapEngine->thread());
#endif
  _tcpStack = new QPcapTcpStack(0, _ipStack);
  _httpStack = new QPcapHttpStack(0, _tcpStack);
#ifndef MONOTHREAD_PROFILING
  // tcp and http stacks must share same thread because of discardXXXBuffer
  _tcpStack->moveToThread(_tcpHttpThread);
  _httpStack->moveToThread(_tcpStack->thread());
#endif
  _customFieldAnalyzer = new HttpCustomFieldAnalyzer(0);
  _customFieldAnalyzer->connectToSource(_httpStack);
#ifndef MONOTHREAD_PROFILING
  // custom field analyzer has a separate thread because regex computation costs
  // much cpu
  // FIXME make it possible to have several regex threads rather than only one
  _customFieldAnalyzer->moveToThread(_regexThread);
#endif
  _tcpData = new TcpData(0);
  _httpData = new HttpData(0);
  _tcpData->connectToSource(_tcpStack);
  _httpData->connectToSource(_customFieldAnalyzer);
#ifndef MONOTHREAD_PROFILING
  // data object may be in a thread distinct from http and tcp stacks analysis
  // however it has been tested less performant (both longer and mor memory
  // consuming) on a 4 ways 2.6 GHz computer (1 × i7 × 2 cores × 2 threads)
  _tcpData->moveToThread(_tcpHttpThread);
  _httpData->moveToThread(_tcpHttpThread);
#endif
  _tcpConversationProxyModel = new QSortFilterProxyModel(this);
  _tcpConversationProxyModel->setSourceModel(
        _tcpConversationModel = new TcpConversationModel(this, _tcpData));
  ui->tcpConversationsView->setModel(_tcpConversationProxyModel);
  ui->tcpPacketsView->setModel(
        _tcpPacketModel = new TcpPacketModel(this, _tcpData));
  _httpHitProxyModel = new QSortFilterProxyModel(this);
  _httpHitProxyModel->setSourceModel(
        _httpHitModel = new HttpHitModel(this, _httpData));
  ui->httpHitsView->setModel(_httpHitProxyModel);
  connect(_pcapEngine, SIGNAL(packetsCountTick(ulong)),
          this, SLOT(updatePacketsCount(ulong)));
  connect(_httpStack, SIGNAL(hitsCountTick(ulong)),
          this, SLOT(updateHitsCount(ulong)));
  connect(_httpData, SIGNAL(captureFinished()),
          this, SLOT(captureFinished())); // FIXME
  _tcpHttpThread->start();
  _regexThread->start();
}

HttpppMainWindow::~HttpppMainWindow() {
  /*_httpStack->deleteLater();
  _tcpStack->deleteLater();
  _ipStack->deleteLater();
  _etherStack->deleteLater();
  _pcapEngine->deleteLater();
  _customFieldAnalyzer->deleteLater();
  _tcpData->deleteLater();
  // LATER ensure that objects are deleted, currently they are probably not
  // because deleteLater is likely not to work since threads are exited
  // immediatly
  _tcpHttpThread->exit();
  _regexThread->exit();
  _tcpHttpThread->wait(200);
  _regexThread->wait(200);*/
  // FIXME
  qInstallMsgHandler(0);
  delete ui;
}

void HttpppMainWindow::loadFile(QString filename) {
  if (_pcapEngine->isRunning()) { // LATER this is not sufficient since there are asynchronous signals
    qDebug() << "cannot load file when pcap engine is already running";
    return;
  }
  _tcpData->clear();
  _httpData->clear();
  _customFieldAnalyzer->clearFilters();
  if (!ui->comboField1->currentText().isEmpty())
    _customFieldAnalyzer->addFilter(
          ui->comboField1->currentText(),
          (QPcapHttpStack::QPcapHttpDirection)ui->switchField1->currentIndex());
  if (!ui->comboField2->currentText().isEmpty())
    _customFieldAnalyzer->addFilter(
          ui->comboField2->currentText(),
          (QPcapHttpStack::QPcapHttpDirection)ui->switchField2->currentIndex());
  if (!ui->comboField3->currentText().isEmpty())
    _customFieldAnalyzer->addFilter(
          ui->comboField3->currentText(),
          (QPcapHttpStack::QPcapHttpDirection)ui->switchField3->currentIndex());
  ui->labelLoading->setVisible(true);
  _pcapEngine->loadFile(filename);
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
    QPcapTcpConversation c = _tcpConversationModel->conversation(
          _tcpConversationProxyModel->mapToSource(currentIndex));
    selectConversationInPackets(c);
    showDetails(c);
  } else if (sender() == ui->tcpPacketsView) {
    //qDebug() << "forwardSelection from TCP packets";
    QPcapTcpPacket p = _tcpPacketModel->packet(currentIndex);
    QPcapTcpConversation c = _tcpPacketModel->conversation(currentIndex);
    selectConversationInConversations(c);
    //qDebug() << "sender = tcpPacketView" << p.isNull() << p << c;
    if (p.isNull())
      showDetails(c);
    else
      showDetails(c, p);
  } else if (sender() == ui->httpHitsView) {
    QPcapHttpHit hit = _httpHitModel->hit(
          _httpHitProxyModel->mapToSource(currentIndex));
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
    QModelIndex i = _tcpConversationModel->index(conversation);
    if (i.isValid()) {
      ui->tcpConversationsView->scrollTo(
            _tcpConversationProxyModel->mapFromSource(i));
      ui->tcpConversationsView->selectRow(
            _tcpConversationProxyModel->mapFromSource(i).row());
    }
  }
}

void HttpppMainWindow::selectConversationInPackets(
    QPcapTcpConversation conversation) {
  if (ui->tcpPacketsView->isVisible()) {
    QModelIndex i = _tcpPacketModel->index(conversation);
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
    QModelIndex i = _tcpPacketModel->index(packet);
    ui->tcpPacketsView->setExpanded(_tcpPacketModel->parent(i), true);
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
