#include "multipixmapbutton.h"
#include <QPixmap>
#include <QtDebug>

int MultiPixmapButton::appendPixmap(QString filename) {
  QPixmap p(filename);
  if (p.isNull()) {
    qDebug() << "MultiPixmapButton::appendPixmap() cannot create pixmap from "
                "file" << filename;
    return -1;
  }
  int s = _pixmaps.size();
  _pixmaps.append(p);
  if (!s)
    setCurrentIndex(s);
  return s;
}

void MultiPixmapButton::clear() {
  setPixmap(QPixmap());
  _pixmaps.clear();
  update();
}

void MultiPixmapButton::setCurrentIndex(int index) {
  if (index >= 0 && index < _pixmaps.size()) {
    setPixmap(_pixmaps.value(_currentIndex = index));
    update();
  }
}

void MultiPixmapButton::mouseReleaseEvent(QMouseEvent *ev) {
  Q_UNUSED(ev);
  int s = _pixmaps.size();
  if (s > 1)
    setCurrentIndex(_currentIndex = (_currentIndex+1)%s);
}
