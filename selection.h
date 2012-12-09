#ifndef _SELECTION_H_
#define _SELECTION_H_

#include <QGraphicsItem>

class Selection : public QGraphicsItem
{
 public:
  Selection();
  ~Selection();

  void paint(QPainter *painter,
             const QStyleOptionGraphicsItem *option,
             QWidget *widget);
  QRectF boundingRect() const;

  const QRect& rect() const { return rect_; }

  void set(const QRect &rect);
  void move(const QPoint &point);
  void clear();

  bool within(const QPoint &point) const;

 private:
  void resetGeometry();

 private:
  QRect rect_;
};

#endif
