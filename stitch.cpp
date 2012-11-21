#include <QPainter>

#include "color.h"
#include "document.h"
#include "globalstate.h"

#include "stitch.h"

#define PI_2 1.414213562373095

#define PHI 0.3f

const QPointF shapeBackslash[] = {
  QPointF(0.0f, 0.0f),
  QPointF(PHI, 0.0f),
  QPointF(1.0f, 1.0f - PHI),
  QPointF(1.0f, 1.0f),
  QPointF(1.0f - PHI, 1.0f),
  QPointF(0.0f, PHI)
};

const QPointF shapeSlash[] = {
  QPointF(1.0f, 0.0f),
  QPointF(1.0f, PHI),
  QPointF(PHI, 1.0f),
  QPointF(0.0f, 1.0f),
  QPointF(0.0f, 1.0f - PHI),
  QPointF(1.0f - PHI, 0.0f)
};

const QPointF shapePart[] = {
  QPointF(0.0f, 0.0f),
  QPointF(PHI, 0.0f),
  QPointF(0.5f, 0.5f - PHI),
  QPointF(0.5f, 0.5f),
  QPointF(0.5f - PHI, 0.5f),
  QPointF(0.0f, PHI)
};

qreal zv = 0;

/*
 * StitchItem
 */

StitchItem::StitchItem(const Color *color, Document *document, QGraphicsItem *parent)
    : QGraphicsItem(parent), color_(color), document_(document)
{
  if (!color_) {
    color_ = &Color::defaultColor;
  }

  setZValue(zv);
  zv += 0.1;
}

StitchItem::~StitchItem()
{

}

void StitchItem::acquire()
{
  if (document_)
    document_->acquire(this);
}

void StitchItem::release()
{
  if (document_)
    document_->release(this);
}

/*
 * PositionedStitchItem
 */

PositionedStitchItem::PositionedStitchItem(const QPointF &position,
                                           const QSizeF &size,
                                           const Color *color,
                                           Document *document,
                                           QGraphicsItem *parent)
    : StitchItem(color, document, parent), size_(size)
{
  setPos(position);
}

PositionedStitchItem::~PositionedStitchItem()
{

}

QRectF PositionedStitchItem::boundingRect() const
{
  return QRectF(QPointF(0.0f, 0.0f), size_);
}

void PositionedStitchItem::paint(QPainter *painter,
                                 const QStyleOptionGraphicsItem *option,
                                 QWidget *widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);

  painter->setBrush(color_->brush());

  switch (GlobalState::self()->renderingMode()) {
    case RenderingMode_Simple:
      painter->setPen(QPen(color_->color().darker(), 0.5));
      painter->drawRect(boundingRect());
      break;
    case RenderingMode_Full:
      painter->scale(size_.width(), size_.height());
      painter->setPen(QPen(color_->color().darker(), 0.5 / size_.width()));
      paintStitch(painter);
      break;
    default:
      break;
  }
}

/*
 * FullStitchItem
 */

FullStitchItem::FullStitchItem(const QPointF &position,
                               const Color *color,
                               Document *document,
                               QGraphicsItem *parent)
    : PositionedStitchItem(position, QSizeF(10.0f, 10.0f), color, document, parent)
{
  
}

FullStitchItem::~FullStitchItem()
{

}

qreal FullStitchItem::weight() const
{
  return PI_2 * 2.0;
}

void FullStitchItem::paintStitch(QPainter *painter)
{
  painter->drawPolygon(shapeSlash, sizeof(shapeSlash) / sizeof(QPointF));
  painter->drawPolygon(shapeBackslash, sizeof(shapeBackslash) / sizeof(QPointF));
}

/*
 * PetiteStitchItem
 */

PetiteStitchItem::PetiteStitchItem(const QPointF &position,
                                   const Color *color,
                                   Document *document,
                                   QGraphicsItem *parent)
    : PositionedStitchItem(position, QSizeF(5.0f, 5.0f), color, document, parent)
{
  
}

PetiteStitchItem::~PetiteStitchItem()
{

}

qreal PetiteStitchItem::weight() const
{
  return PI_2;
}

void PetiteStitchItem::paintStitch(QPainter *painter)
{
  painter->drawPolygon(shapeSlash, sizeof(shapeSlash) / sizeof(QPointF));
  painter->drawPolygon(shapeBackslash, sizeof(shapeBackslash) / sizeof(QPointF));
}

/*
 * HalfStitchItem
 */

HalfStitchItem::HalfStitchItem(Orientation orientation,
                               const QPointF &position,
                               const Color *color,
                               Document *document,
                               QGraphicsItem *parent)
    : PositionedStitchItem(position, QSizeF(10.0f, 10.0f), color, document, parent),
      orientation_(orientation)
{
  
}

HalfStitchItem::~HalfStitchItem()
{

}

qreal HalfStitchItem::weight() const
{
  return PI_2;
}

void HalfStitchItem::paintStitch(QPainter *painter)
{
  if (orientation_ == Orientation_Slash)
    painter->drawPolygon(shapeSlash, sizeof(shapeSlash) / sizeof(QPointF));
  else
    painter->drawPolygon(shapeBackslash, sizeof(shapeBackslash) / sizeof(QPointF));
}

/*
 * QuarterStitchItem
 */

QuarterStitchItem::QuarterStitchItem(Orientation orientation,
                                     const QPointF &position,
                                     const Color *color,
                                     Document *document, 
                                     QGraphicsItem *parent)
    : PositionedStitchItem(position, QSizeF(5.0f, 5.0f), color, document, parent),
      orientation_(orientation)
{
  
}

QuarterStitchItem::~QuarterStitchItem()
{

}

qreal QuarterStitchItem::weight() const
{
  return PI_2 / 2.0;
}

void QuarterStitchItem::paintStitch(QPainter *painter)
{
  if (orientation_ == Orientation_Slash)
    painter->drawPolygon(shapeSlash, sizeof(shapeSlash) / sizeof(QPointF));
  else
    painter->drawPolygon(shapeBackslash, sizeof(shapeBackslash) / sizeof(QPointF));
}
