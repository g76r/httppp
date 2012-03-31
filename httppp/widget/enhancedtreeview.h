#ifndef ENHANCEDTREEVIEW_H
#define ENHANCEDTREEVIEW_H

#include <QTreeView>

class EnhancedTreeView : public QTreeView {
  Q_OBJECT
public:
  explicit inline EnhancedTreeView(QWidget *parent = 0) : QTreeView(parent) { }

signals:
  void currentIndexChanged(const QModelIndex &current);

protected slots:
  void currentChanged(const QModelIndex &current, const QModelIndex &previous);

private:
  Q_DISABLE_COPY(EnhancedTreeView)
};

#endif // ENHANCEDTREEVIEW_H
