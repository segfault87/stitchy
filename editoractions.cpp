#include "canvas.h"
#include "cell.h"
#include "document.h"
#include "selection.h"
#include "selectiongroup.h"
#include "sparsemap.h"

#include "editoractions.h"

EditorAction::EditorAction(Document *document)
    : QUndoCommand(), document_(document)
{

}

EditorAction::~EditorAction()
{
  
}

CanvasAction::CanvasAction(Document *document, Canvas *canvas)
  : EditorAction(document), canvas_(canvas)
{

}

CanvasAction::~CanvasAction()
{

}

MergeAction::MergeAction(Document *document, SparseMap *map)
    : EditorAction(document)
{
  const CellMap &cells = document->map()->cells();
  const CellMap &newcells = map->cells();

  for (CellMap::ConstIterator it = newcells.begin();
       it != newcells.end();
       ++it) {
    if (cells.contains(it.key()))
      previousState_.append(Cell(*cells[it.key()]));
    else 
      previousState_.append(Cell(it.key(), document_));
    drawn_.append(Cell(*it.value()));
  }
}

MergeAction::~MergeAction()
{

}

void MergeAction::replaceWith(const QList<Cell> &cells)
{
  SparseMap *map = document_->map();

  foreach (const Cell &cell, cells) {
    Cell *c = map->overwrite(cell);
    if (c)
      c->createGraphicsItems();
  }
}

void MergeAction::mergeWith_(const QList<Cell> &cells)
{
  SparseMap *map = document_->map();

  foreach (const Cell &cell, cells) {
    Cell *c = map->merge(cell);
    if (c)
      c->createGraphicsItems();
  }
}

ActionDraw::ActionDraw(Document *document, SparseMap *map)
    : MergeAction(document, map)
{
  setText(QObject::tr("Drawing"));
}

ActionDraw::~ActionDraw()
{

}

void ActionDraw::redo()
{
  mergeWith_(drawn_);
}

void ActionDraw::undo()
{
  replaceWith(previousState_);
}

ActionErase::ActionErase(Document *document, SparseMap *map)
    : MergeAction(document, map)
{
  setText(QObject::tr("Erasing"));
}

ActionErase::~ActionErase()
{

}

void ActionErase::redo()
{
  replaceWith(drawn_);
}

void ActionErase::undo()
{
  mergeWith_(previousState_);
}

ActionMove::ActionMove(Document *document, const QPoint &originalPosition,
		       const QSize &size, SelectionGroup *group)
  : EditorAction(document), size_(size), originalPosition_(originalPosition)
{
  map_ = NULL;

  setText(QObject::tr("Moving"));

  setData(group);
}

ActionMove::~ActionMove()
{
  if (map_)
    delete map_;
}

void ActionMove::redo()
{
  SparseMap *orig = document_->map();

  const CellMap &cells = map_->cells();
  for (CellMap::ConstIterator it = cells.begin(); it != cells.end(); ++it) {
    QPoint po = originalPosition_ + it.key();
    QPoint pt = targetPosition_ + it.key();

    /* remove original pos */
    if (orig->contains(po))
      orig->remove(po);

    /* add new */
    Cell *c = orig->cellAt(pt);
    if (!c)
      continue;
    c->merge(*it.value());
    c->createGraphicsItems();
  }

  document_->createSelection(QRect(targetPosition_, size_));
}

void ActionMove::undo()
{
  SparseMap *orig = document_->map();

  QSet<QPoint> prevSet;

  foreach (const Cell &c, previousState_) {
    Cell *newcell = orig->overwrite(c);
    prevSet.insert(c.pos());
    if (newcell)
      newcell->createGraphicsItems();
  }

  const CellMap &cells = map_->cells();
  for (CellMap::ConstIterator it = cells.begin(); it != cells.end(); ++it) {
    QPoint po(originalPosition_ + it.key());
    QPoint pt(targetPosition_ + it.key());

    if (orig->contains(pt) && !prevSet.contains(pt))
      orig->remove(pt);
    
    /* add new */
    Cell *c = orig->cellAt(po);
    if (!c)
      continue;
    c->merge(*it.value());
    c->createGraphicsItems();
  }

  document_->createSelection(QRect(originalPosition_, size_));
}

void ActionMove::setData(SelectionGroup *group)
{
  if (map_)
    delete map_;

  targetPosition_ = group->region().topLeft();
  SparseMap *origmap = document_->map();
  map_ = new SparseMap(*group->map());

  const CellMap &cells = map_->cells();
  previousState_.clear();
  for (CellMap::ConstIterator it = cells.begin(); it != cells.end(); ++it) {
    QPoint t(targetPosition_ + it.key());
    if (origmap->contains(t)) {
      const Cell *origcell = origmap->cellAt(t);
      if (!origcell)
        continue;
      previousState_.append(Cell(*origcell));
    } else {
      previousState_.append(Cell(t, document_));
    }
  }
}

ActionPaste::ActionPaste(Document *document, Canvas *canvas, const QByteArray &data)
  : CanvasAction(document, canvas), data_(data)
{
  setText(QObject::tr("Pasting"));
}

ActionPaste::~ActionPaste()
{

}

void ActionPaste::redo()
{
  canvas_->paste(data_);
}

void ActionPaste::undo()
{
  canvas_->clearFloatingSelection();
}

ActionFloatMove::ActionFloatMove(Document *document, Canvas *canvas,
				 const QPoint &from, const QPoint &to)
  : CanvasAction(document, canvas), from_(from), to_(to)
{

}

ActionFloatMove::~ActionFloatMove()
{

}

void ActionFloatMove::redo()
{
  canvas_->moveFloatingSelection(to_);
}

void ActionFloatMove::undo()
{
  canvas_->moveFloatingSelection(from_);
}

uint qHash(const QPoint &p)
{
  return qHash((quint64)p.x() << 32 | (quint64)p.y());
}

