#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <QGraphicsView>

#include "cell.h"

class QMouseEvent;
class QWheelEvent;

class Selection;
class SparseMap;

class Canvas : public QGraphicsView
{
  Q_OBJECT;

 public:
  Canvas(QWidget *parent = NULL);
  ~Canvas();

  bool mapToGrid(const QPoint &pos, QPoint &out);
  bool mapToGrid(const QPoint &pos, QPoint &out, Subarea &subareaOut);
  
 signals:
  void madeSelection(const QRect &rect);
  void clearedSelection();

 public slots:
  void zoomIn();
  void zoomOut();
  void zoomReset();
  void toggleGrid(bool enabled);

 private:
  void setCenter(const QPointF &centerPoint);

  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);

 private:
  Selection *selection_;
  SparseMap *drawmap_;

  /* states */
  bool selecting_;
  bool moving_;
  bool dragging_;
  bool drawing_;
  bool erasing_;
  bool rectangle_;

  /* coords */
  QPoint lastPos_;
  QPoint startPos_;
  QRect lastRect_;
  QPoint cursor_;
  Subarea subcursor_;
  QPointF center_;
};

#endif
