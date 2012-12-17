#include <QPainter>

#include "selection.h"

Selection::Selection()
{
  rect_ = QRect();
}

Selection::~Selection()
{
}

void Selection::paint(QPainter *painter,
                      const QStyleOptionGraphicsItem *option,
                      QWidget *widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);

  QPen pen;

  pen.setStyle(Qt::DashLine);
  pen.setColor(Qt::blue);
  pen.setWidth(2);

  painter->setPen(pen);
  painter->setBrush(Qt::NoBrush);
  painter->drawRect(boundingRect());
}

QRectF Selection::boundingRect() const
{
  return QRectF(rect_.x() * 10.0f, rect_.y() * 10.0f,
                rect_.width() * 10.0f, rect_.height() * 10.0f);
}

void Selection::set(const QRect &rect)
{
  show();

  rect_ = rect;

  if (!rect_.isValid())
    rect_ = rect_.normalized();

  resetGeometry();
}

void Selection::move(const QPoint &point)
{
  rect_.moveTopLeft(point);

  resetGeometry();
}

void Selection::clear()
{
  hide();

  rect_ = QRect();

  resetGeometry();
}

bool Selection::within(const QPoint &point) const
{
  return rect_.contains(point);
}

void Selection::resetGeometry()
{
  prepareGeometryChange();
}

