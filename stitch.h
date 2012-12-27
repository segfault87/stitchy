#ifndef _STITCH_H_
#define _STITCH_H_

#include <QGraphicsItem>

class Color;
class Document;

enum RenderingMode
{
  RenderingMode_Simple,
  RenderingMode_Full,
  RenderingMode_Symbol
};

enum Orientation {
  Orientation_Slash,
  Orientation_Backslash
};

class StitchItem : public QGraphicsItem
{
 public:
  StitchItem(const Color *color,
             Document *document = NULL,
             QGraphicsItem *parent = NULL);
  virtual ~StitchItem();

  void acquire();
  void release();

  const Color* color() const { return color_; }

  void setColor(Color *c) { color_ = c; }

  virtual int weight() const = 0;

 protected:
  const Color *color_;
  Document *document_;
};

class PositionedStitchItem : public StitchItem
{
 public:
  PositionedStitchItem(const QPointF &position,
                       const QSizeF &size,
                       const Color *color,
                       Document *document = NULL,
                       QGraphicsItem *parent = NULL);
  ~PositionedStitchItem();

  QRectF boundingRect() const;

  void paint(QPainter *painter,
             const QStyleOptionGraphicsItem *option,
             QWidget *widget);

 protected:
  virtual void paintStitch(QPainter *painter) = 0;

  void setSize(const QSizeF &size) { size_ = size; }

  QSizeF size_;
};

class FullStitchItem : public PositionedStitchItem
{
 public:
  FullStitchItem(const QPointF &position,
                 const Color *color,
                 Document *document = NULL,
                 QGraphicsItem *parent = NULL);
  ~FullStitchItem();

  virtual int weight() const;

 private:
  void paintStitch(QPainter *painter);
};

class PetiteStitchItem : public PositionedStitchItem
{
 public:
  PetiteStitchItem(const QPointF &position,
                   const Color *color,
                   Document *document = NULL,
                   QGraphicsItem *parent = NULL);
  ~PetiteStitchItem();

  virtual int weight() const;

 private:
  void paintStitch(QPainter *painter);
};

class HalfStitchItem : public PositionedStitchItem
{
 public:
  HalfStitchItem(Orientation orientation,
                 const QPointF &position,
                 const Color *color,
                 Document *document = NULL,
                 QGraphicsItem *parent = NULL);
  ~HalfStitchItem();

  virtual int weight() const;

 private:
  void paintStitch(QPainter *painter);

 private:
  Orientation orientation_;
};

class QuarterStitchItem : public PositionedStitchItem
{
 public:
  QuarterStitchItem(Orientation orientation,
                    const QPointF &position,
                    const Color *color,
                    Document *document = NULL,
                    QGraphicsItem *parent = NULL);
  ~QuarterStitchItem();

  virtual int weight() const;

 private:
  void paintStitch(QPainter *painter);

 private:
  Orientation orientation_;
};

#endif
