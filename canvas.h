#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <QGraphicsView>

#include "cell.h"

class QMouseEvent;
class QWheelEvent;

class SparseMap;

class Canvas : public QGraphicsView
{
  Q_OBJECT;

 public:
  Canvas(QWidget *parent = NULL);
  ~Canvas();

  bool mapToGrid(const QPoint &pos, QPoint &out, Subarea &subareaOut);

 private:
  void setCenter(const QPointF &centerPoint);

  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);

 private:
  SparseMap *drawboard_;
  bool dragging_;
  bool drawing_;
  bool erasing_;
  QPoint lastPos_;
  QPoint cursor_;
  Subarea subcursor_;
  QPointF center_;
};

#endif
