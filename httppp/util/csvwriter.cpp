#include "csvwriter.h"
#include <QFile>
#include <QtDebug>

void CsvWriter::write(QAbstractItemModel &model, QString filename) {
  QFile file(filename);
  if (file.exists()) {
    qDebug() << "won't overwrite" << filename;
    return;
  }
  if (!file.open(QIODevice::WriteOnly)) {
    qDebug() << "cannot open" << filename;
  }
  int columns = model.columnCount(), rows = model.rowCount();
  for (int i = 0; i < columns; ++i) {
    if (i)
      file.write(";");
    file.write(model.headerData(i, Qt::Horizontal).toString().toUtf8());
  }
  file.write("\n");
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (j)
        file.write(";");
      file.write(model.data(model.index(i, j)).toString().toUtf8());
    }
    file.write("\n");
  }
}
