#ifndef _SPARSEMAP_H_
#define _SPARSEMAP_H_

#include <QMap>
#include <QPoint>

class Cell;
class Document;

typedef QMap<QPoint, Cell *> CellMap;

class SparseMap
{
 public:
  SparseMap(Document *parent);
  ~SparseMap();

  Cell* cellAt(const QPoint &pos);
  Cell* overwrite(const Cell &c);
  Cell* merge(const Cell &c);
  
  const CellMap& cells() const { return cells_; }

  void clear();

 private:
  CellMap cells_;
  Document *document_;
};

bool operator<(const QPoint &a, const QPoint &b);

#endif
