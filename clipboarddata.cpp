#include <QDataStream>

#include "cell.h"
#include "document.h"
#include "globalstate.h"
#include "sparsemap.h"

#include "clipboarddata.h"

ClipboardData::ClipboardData(Document *doc)
{
  map_ = new SparseMap(doc);
}

ClipboardData::ClipboardData(Document *doc, const QPoint &initialPosition)
    : initialPosition_(initialPosition)
{
  map_ = new SparseMap(doc);
}

ClipboardData::ClipboardData(Document *doc, QByteArray &array)
{
  map_ = new SparseMap(doc);

  deserialize(array);
}

ClipboardData::~ClipboardData()
{
  delete map_;
}

QByteArray ClipboardData::serialize() const
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

void ClipboardData::deserialize(QByteArray &array)
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
