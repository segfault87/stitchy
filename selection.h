#ifndef _SELECTION_H_
#define _SELECTION_H_

#include <QGraphicsItem>

class Selection : public QGraphicsItem
{
 public:
  Selection();
  ~Selection();

  void paint(QPainter *painter,
             const QStyleOptionGraphicItem *option,
             QWidget *widget);
  QRectF boundingRect() const;

  const QRect& rect() const { return rect_; }

  void setRect(const QRect &rect) { rect_ = rect; }

 private:
  QRect rect_;
};

#endif
