#include "cell.h"
#include "document.h"

#include "sparsemap.h"

SparseMap::SparseMap(Document *parent)
    : document_(parent)
{

}

SparseMap::~SparseMap()
{
  clear();
}

bool SparseMap::contains(const QPoint &pos) const
{
  return cells_.contains(pos);
}

Cell* SparseMap::cellAt(const QPoint &pos)
{
  CellMap::iterator it = cells_.find(pos);

  if (it == cells_.end()) {
    Cell *c = new Cell(pos, document_);
    cells_[pos] = c;
    return c;
  }

  return it.value();
}

Cell* SparseMap::overwrite(const Cell &c)
{
  if (cells_.contains(c.pos())) {
    delete cells_[c.pos()];
    cells_.remove(c.pos());
  }

  if (!c.isEmpty()) {
    Cell *newCell = new Cell(c);
    cells_.insert(c.pos(), newCell);

    return newCell;
  }

  return NULL;
}

Cell* SparseMap::merge(const Cell &c)
{
  if (cells_.contains(c.pos())) {
    Cell *oc = cells_[c.pos()];
    oc->merge(c);
    
    return oc;
  } else {
    Cell *newCell = new Cell(c);
    cells_.insert(c.pos(), newCell);
    
    return newCell;
  }
}

void SparseMap::clear()
{
  for (QMap<QPoint, Cell *>::iterator it = cells_.begin();
       it != cells_.end();
       ++it) {
    delete it.value();
  }

  cells_.clear();
}

bool operator<(const QPoint &a, const QPoint &b)
{
  long long al = (long long)a.y() << 32 | (long long)a.x();
  long long bl = (long long)b.y() << 32 | (long long)b.x();
  
  return al < bl;
}
