#include "enhancedtableview.h"

void EnhancedTableView::currentChanged(const QModelIndex &current,
                                       const QModelIndex &previous) {
  QTableView::currentChanged(current, previous);
  emit currentIndexChanged(current);
}
