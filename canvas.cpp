#include <QWheelEvent>

#include "cell.h"
#include "document.h"
#include "editor.h"
#include "editoractions.h"
#include "globalstate.h"
#include "sparsemap.h"

#include "canvas.h"

#define MAGNIFICATION_RATE 0.2

Canvas::Canvas(QWidget *parent)
    : QGraphicsView(parent)
{
  dragging_ = false;
  drawing_ = false;

  cursor_ = QPoint(-1, -1);
  subcursor_ = Subarea_TopLeft;

  scale(2.0f, 2.0f);
}

Canvas::~Canvas()
{

}

bool Canvas::mapToGrid(const QPoint &pos, QPoint &out, Subarea &subareaOut)
{
  Document *d = GlobalState::self()->activeDocument();
  if (!d)
    return false;

  const QSize dim = d->size();

  QPointF coord = mapToScene(pos);
  QPoint cint(coord.x() / 10, coord.y() / 10);

  if (cint.x() < 0 || cint.x() >= dim.width() ||
      cint.y() < 0 || cint.y() >= dim.height())
    return false;

  out = cint;
  coord.setX(coord.x() - (cint.x() * 10));
  coord.setY(coord.y() - (cint.y() * 10));

  if (coord.x() > 5.0f) {
    if (coord.y() > 5.0f)
      subareaOut = Subarea_BottomRight;
    else
      subareaOut = Subarea_TopRight;
  } else {
    if (coord.y() > 5.0f)
      subareaOut = Subarea_BottomLeft;
    else
      subareaOut = Subarea_TopLeft;
  }

  return true;
}

void Canvas::zoomIn()
{
  scale(1.0 + MAGNIFICATION_RATE, 1.0 + MAGNIFICATION_RATE);
}

void Canvas::zoomOut()
{
  scale(1.0 - MAGNIFICATION_RATE, 1.0 - MAGNIFICATION_RATE);
}

void Canvas::zoomReset()
{
  resetMatrix();
  scale(2.0, 2.0);
}

void Canvas::toggleGrid(bool enabled)
{
  QPixmap pixmap(size());
  pixmap.fill(Qt::transparent);
  QPainter painter(&pixmap);;
  painter.setRenderHint(QPainter::Antialiasing);
  scene()->render(&painter);
  painter.end();
  pixmap.save("scene.png");
}

void Canvas::setCenter(const QPointF& centerPoint)
{
  //Get the rectangle of the visible area in scene coords
  QRectF visibleArea = mapToScene(rect()).boundingRect();
  
  //Get the scene area
  QRectF sceneBounds = sceneRect();
  
  double boundX = sceneBounds.x() + visibleArea.width() / 2.0;
  double boundY = sceneBounds.y() + visibleArea.height() / 2.0;
  double boundWidth = sceneBounds.width() - 2.0 * boundX;
  double boundHeight = sceneBounds.height() - 2.0 * boundY;
  
  //The max boundary that the centerPoint can be to
  QRectF bounds(boundX, boundY, boundWidth, boundHeight);

  if(bounds.contains(centerPoint)) {
    //We are within the bounds
    center_ = centerPoint;
  } else {
    //We need to clamp or use the center of the screen
    if(visibleArea.contains(sceneBounds)) {
      //Use the center of scene ie. we can see the whole scene
      center_ = sceneBounds.center();
    } else {
      center_ = centerPoint;
      
      //We need to clamp the center. The centerPoint is too large
      if(centerPoint.x() > bounds.x() + bounds.width()) {
        center_.setX(bounds.x() + bounds.width());
      } else if(centerPoint.x() < bounds.x()) {
        center_.setX(bounds.x());
      }
      
      if(centerPoint.y() > bounds.y() + bounds.height()) {
        center_.setY(bounds.y() + bounds.height());
      } else if(centerPoint.y() < bounds.y()) {
        center_.setY(bounds.y());
      }
      
    }
  }
  
  //Update the scrollbars
  centerOn(center_);
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
  if (event->button() & Qt::LeftButton) {
    ToolMode mode = GlobalState::self()->toolMode();

    Document *doc = GlobalState::self()->activeDocument();
    if (!GlobalState::self()->color() || !doc)
      return;

    if (mode == ToolMode_Full || mode == ToolMode_Half ||
        mode == ToolMode_Petite || mode == ToolMode_Quarter) {
      drawboard_ = new SparseMap(doc);
      drawing_ = true;

      mouseMoveEvent(event);
    }
  } else if (event->button() & Qt::RightButton) {
    dragging_ = true;
    lastPos_ = event->pos();
    event->accept();
    return;
  }

  event->ignore();
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
  if (dragging_) {
    QPointF delta = mapToScene(lastPos_) - mapToScene(event->pos());
    lastPos_ = event->pos();
    setCenter(center_ + delta);
  } else if (drawing_) {
     Subarea subcursor;
    QPoint cursor;

    if (!mapToGrid(event->pos(), cursor, subcursor))
      return;

    ToolMode mode = GlobalState::self()->toolMode();
    const Color *c = GlobalState::self()->color();
    
    if (cursor != cursor_) {
      Cell *cell = drawboard_->cellAt(cursor);

      cursor_ = cursor;

      if (mode == ToolMode_Full) {
        cell->addFullStitch(c);
      } else {
        Orientation o;
        if (subcursor == Subarea_TopLeft ||
            subcursor == Subarea_BottomRight)
          o = Orientation_Backslash;
        else
          o = Orientation_Slash;

        subcursor_ = subcursor;
        if (mode == ToolMode_Half)
          cell->addHalfStitch(o, c);
        else if (mode == ToolMode_Petite)
          cell->addPetiteStitch(subcursor, c);
        else if (mode == ToolMode_Quarter)
          cell->addQuarterStitch(o, subcursor, c);
      }

      cell->createGraphicsItems();
    }
  }
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
  if (dragging_ && event->button() & Qt::RightButton) {
    dragging_ = false;
    return;
  } else if (drawing_ && event->button() & Qt::LeftButton) {
    Document *doc = GlobalState::self()->activeDocument();

    if (!doc)
      return;
    
    drawing_ = false;
    cursor_ = QPoint(-1, -1);
    subcursor_ = Subarea_TopLeft;
    doc->editor()->edit(new ActionDraw(doc, drawboard_));
    delete drawboard_;
    return;
  }
}

void Canvas::wheelEvent(QWheelEvent *event)
{
  double mag;

  if (event->delta() > 0) {
    mag = 1.0 + MAGNIFICATION_RATE;
  } else {
    mag = 1.0 - MAGNIFICATION_RATE;
  }

  scale(mag, mag);
}


