#include <QFile>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QUndoGroup>

#include "cell.h"
#include "color.h"
#include "editor.h"
#include "globalstate.h"
#include "sparsemap.h"
#include "stitch.h"
#include "utils.h"

#include "document.h"

Document::Document(QObject *parent)
    : QObject(parent)
{
  changed_ = false;

  scene_ = new QGraphicsScene(this);
  editor_ = new Editor(this);
  map_ = new SparseMap(this);

  connect(editor_, SIGNAL(changed()), this, SLOT(documentChanged_()));

  grid_ = NULL;
  resetGrid();

  GlobalState::self()->undoGroup()->addStack(editor_);
}

Document::Document(const QSize &size, QObject *parent)
    : QObject(parent), size_(size)
{
  changed_ = false;

  scene_ = new QGraphicsScene(this);
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

  delete map_;
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
  scene_->addItem(grid_);
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
