#include "enhancedtreeview.h"

void EnhancedTreeView::currentChanged(const QModelIndex &current,
                                      const QModelIndex &previous) {
  QTreeView::currentChanged(current, previous);
  emit currentIndexChanged(current);
}
