#include "cell.h"
#include "document.h"
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

ActionMerge::ActionMerge(Document *document, SparseMap *map)
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

ActionMerge::~ActionMerge()
{

}

void ActionMerge::replaceWith(const QList<Cell> &cells)
{
  SparseMap *map = document_->map();

  foreach (const Cell &cell, cells) {
    Cell *c = map->overwrite(cell);
    if (c)
      c->createGraphicsItems();
  }
}

void ActionMerge::mergeWith(const QList<Cell> &cells)
{
  SparseMap *map = document_->map();

  foreach (const Cell &cell, cells) {
    Cell *c = map->merge(cell);
    if (c)
      c->createGraphicsItems();
  }
}

ActionDraw::ActionDraw(Document *document, SparseMap *map)
    : ActionMerge(document, map)
{
  setText(QObject::tr("Drawing"));
}

ActionDraw::~ActionDraw()
{

}

void ActionDraw::redo()
{
  mergeWith(drawn_);
}

void ActionDraw::undo()
{
  replaceWith(previousState_);
}

ActionErase::ActionErase(Document *document, SparseMap *map)
    : ActionMerge(document, map)
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
  mergeWith(previousState_);
}

ActionMove::ActionMove(Document *document, const QPoint &originalPosition,
		       SelectionGroup *group)
  : EditorAction(document), originalPosition_(originalPosition)
{
  setText(QObject::tr("Moving"));

  setData(group);
}

ActionMove::~ActionMove()
{
  delete map_;
}

void ActionMove::redo()
{
  SparseMap *orig = document_->map();

  const CellMap &cells = map_->cells();
  
}

void ActionMove::undo()
{
  SparseMap *orig = document_->map();
}

void ActionMove::setData(SelectionGroup *group)
{
  targetPosition_ = group->position();
  map_ = new SparseMap(*group->map());

  const CellMap &cells = map_->cells();
  const CellMap &doccells = document_->map()->cells();

  for (CellMap::ConstIterator it = cells.begin(); it != cells.end(); ++it) {
    QPoint p(targetPosition_ + it.key());
    if (doccells.contains(p)) {
      previousState.append(Cell(p, document_));
    } else {
      Cell c(*doccells.cellAt(p));
      c.move(p);
      previousState.append(c);
    }
  }
}
