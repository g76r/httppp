#ifndef ENHANCEDTABLEVIEW_H
#define ENHANCEDTABLEVIEW_H

#include <QTableView>

class EnhancedTableView : public QTableView {
  Q_OBJECT
public:
  explicit EnhancedTableView(QWidget *parent = 0) : QTableView(parent) { }

signals:
  void currentIndexChanged(const QModelIndex &current);

protected slots:
  void currentChanged(const QModelIndex &current, const QModelIndex &previous);

private:
  Q_DISABLE_COPY(EnhancedTableView)
};

#endif // ENHANCEDTABLEVIEW_H
