#ifndef MULTIPIXMAPBUTTON_H
#define MULTIPIXMAPBUTTON_H

#include <QLabel>
#include <QList>

class QPixmap;

class MultiPixmapButton: public QLabel {
  Q_OBJECT
private:
  QList<QPixmap> _pixmaps;
  int _currentIndex;

public:
  inline explicit MultiPixmapButton(QWidget *parent = 0) : QLabel(parent) { }
  /** @return new pixmap index, or -1 on error (e.g. cannot load file)
    */
  int appendPixmap(QString filename);
  inline int pixmapsCount() const { return _pixmaps.size(); }
  inline bool isEmpty() const { return _pixmaps.isEmpty(); }
  /** @return -1 if empty
    */
  inline int currentIndex() const {
    return _pixmaps.size() ? _currentIndex : -1; }
  void mouseReleaseEvent(QMouseEvent *ev);

signals:
  void	currentIndexChanged(int index);

public slots:
  void	clear();
  void	setCurrentIndex(int index);
};


#endif // MULTIPIXMAPBUTTON_H
