#include <QApplication>
#include "gui/httpppmainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  HttpppMainWindow w;
  w.show();
  w.loadFilename(argc > 1 ? argv[1] : "sample.pcap");
  return a.exec();
}
