#ifndef CSVWRITER_H
#define CSVWRITER_H

#include <QString>
#include <QAbstractItemModel>

class CsvWriter {
public:
  static void write(QAbstractItemModel &model, QString filename);
};

#endif // CSVWRITER_H
