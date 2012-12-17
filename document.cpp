#include <QFile>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QUndoGroup>

#include "cell.h"
#include "color.h"
#include "editor.h"
#include "globalstate.h"
#include "sparsemap.h"
#include "selection.h"
#include "stitch.h"
#include "utils.h"

#include "document.h"

Document::Document(QObject *parent)
    : QGraphicsScene(parent)
{
  selection_ = NULL;
  changed_ = false;

  editor_ = new Editor(this);
  map_ = new SparseMap(this);

  connect(editor_, SIGNAL(changed()), this, SLOT(documentChanged_()));

  grid_ = NULL;
  resetGrid();

  GlobalState::self()->undoGroup()->addStack(editor_);
}

Document::Document(const QSize &size, QObject *parent)
    : QGraphicsScene(parent), size_(size)
{
  selection_ = NULL;
  changed_ = false;

  editor_ = new Editor(this);
  map_ = new SparseMap(this);

  connect(editor_, SIGNAL(changed()), this, SLOT(documentChanged_()));

  grid_ = NULL;
  resetGrid();

  GlobalState::self()->undoGroup()->addStack(editor_);
}

Document::~Document()
{
  GlobalState::self()->undoGroup()->removeStack(editor_);

  if (selection_)
    delete selection_;

  delete map_;
}

QRect Document::boundingRect() const
{
  return QRect(QPoint(0, 0), size_);
}

Selection* Document::createSelection()
{
  if (selection_)
    delete selection_;

  selection_ = new Selection();
  addItem(selection_);
}

void Document::clearSelection()
{
  if (selection_) {
    delete selection_;
    selection_ = NULL;
  }
}

void Document::acquire(StitchItem *item)
{
  colors_.acquire(item);
  stitches_.insert(item);
}

void Document::release(StitchItem *item)
{
  colors_.release(item);
  stitches_.remove(item);
}

void Document::setName(const QString &name)
{
  name_ = name;
}

void Document::setTitle(const QString &title)
{
  title_ = title;
}

void Document::setAuthor(const QString &author)
{
  author_ = author;
}

void Document::setSize(const QSize &size)
{
  if (size_ == size)
    return;

  size_ = size;

  resetGrid();
}

void Document::setChanged(bool b)
{
  changed_ = b;
  if (changed_)
    emit documentChanged();
  else
    emit documentSaved();
}

void Document::documentChanged_()
{
  setChanged(true);
}

void Document::resetGrid()
{
  if (grid_)
    delete grid_;

  grid_ = grid();
  addItem(grid_);
}

QGraphicsItemGroup* Document::grid()
{
  QGraphicsItemGroup *group = new QGraphicsItemGroup();
  group->setZValue(-1.0);

  QPen gridPen(QBrush(QColor("#DDDDDD")), 0.0);
  QPen gridPenDec(QBrush(QColor("#AAAAAA")), 0.0);
  QPen *activePen;
  double margin;

  for (int i = 0; i < size_.height() + 1; ++i) {
    if (i % 10 == 0) {
      margin = 10.0;
      activePen = &gridPenDec;
    } else {
      margin = 5.0;
      activePen = &gridPen;
    }

    QGraphicsLineItem *line = new QGraphicsLineItem(-margin,
                                                    10.0 * i,
                                                    10.0 * size_.width() + margin,
                                                    10.0 * i,
                                                    group);
    line->setPen(*activePen);
  }

  for (int i = 0; i < size_.width() + 1; ++i) {
    if (i % 10 == 0) {
      margin = 10.0;
      activePen = &gridPenDec;
    } else {
      margin = 5.0;
      activePen = &gridPen;
    }

    QGraphicsLineItem *line = new QGraphicsLineItem(10.0 * i,
                                                    -margin,
                                                    10.0 * i,
                                                    10.0 * size_.height() + margin,
                                                    group);
    line->setPen(*activePen);
  }

  return group;
}
