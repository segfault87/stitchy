#include <QDataStream>
#include <QGraphicsScene>

#include "cell.h"
#include "document.h"
#include "globalstate.h"
#include "sparsemap.h"

#include "selectiongroup.h"

SelectionGroup::SelectionGroup(Document *doc)
    : QGraphicsItemGroup()
{
  map_ = new SparseMap(doc);
}

SelectionGroup::SelectionGroup(Document *doc, const QRect &region, bool move)
{
  initialPosition_ = region.topLeft();

  map_ = new SparseMap(doc);

  initialize(doc, region, move);
}

SelectionGroup::SelectionGroup(Document *doc, const QPoint &initialPosition)
    : QGraphicsItemGroup(), initialPosition_(initialPosition)
{
  map_ = new SparseMap(doc);
}

SelectionGroup::SelectionGroup(Document *doc, QByteArray &array)
    : QGraphicsItemGroup()
{
  map_ = new SparseMap(doc);

  deserialize(array);
}

SelectionGroup::~SelectionGroup()
{
  delete map_;
}

void SelectionGroup::moveTo(const QPoint &p)
{
  setPos(QPointF(p.x() * 10.0f, p.y() * 10.0f));
}

QByteArray SelectionGroup::serialize() const
{
  QByteArray array;
  QDataStream stream(&array, QIODevice::WriteOnly);

  stream << initialPosition_.x() << initialPosition_.y();
  
  const CellMap &map = map_->cells();
  stream << map.size();
  for (CellMap::ConstIterator it = map.begin(); it != map.end(); ++it) {
    const Cell *c = it.value();
    stream << c->pos().x() << c->pos().y();
    stream << c->featureMask();
    for (int i = 0; i < CELL_COUNT; ++i) {
      if (c->contains(i)) {
        const Color *color = c->color(i);
        stream << i;
        if (color->parent())
          stream << color->parent()->name();
        else
          stream << "";
        stream << color->name();
      }
    }
  }

  return array;
}

void SelectionGroup::deserialize(QByteArray &array)
{
  QDataStream stream(&array, QIODevice::ReadOnly);

  int x, y;
  stream >> x >> y;
  initialPosition_ = QPoint(x, y);

  map_->clear();
  int len;
  stream >> len;
  
  for (int i = 0; i < len; ++i) {
    int x, y;
    stream >> x >> y;
    Cell *cell = map_->cellAt(QPoint(x, y));
    int features;
    stream >> features;
    for (int j = 0; j < CELL_COUNT; ++j) {
      if (FeatureMaskTest(features, j)) {
        QString category, id;
        int feature;

        stream >> feature;
        stream >> category >> id;

        const Color *color = GlobalState::self()->colorManager()->get(category, id);
        cell->addFeature(j, color);
      }
    }

    cell->createGraphicsItems();
  }
}

void SelectionGroup::initialize(Document *doc, const QRect &region, bool move)
{
  SparseMap *map = doc->map();

  initialPosition_ = region.topLeft();
  
  for (int y = region.y(); y < region.y() + region.height(); ++y) {
    for (int x = region.x() ; x < region.x() + region.width(); ++x) {
      QPoint point(x, y);
      if (map->contains(point)) {
        Cell *o = map->cellAt(point);
        
        QPoint adjusted(x - region.x(), y - region.y());
        Cell *c = map_->cellAt(adjusted);
        c->merge(*o);
        c->createGraphicsItems(this);
        
        if (move)
          map->remove(point);
      }
    }
  }

  moveTo(initialPosition_);

  doc->scene()->addItem(this);
}
