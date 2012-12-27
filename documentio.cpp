#include <QFile>
#include <QMap>
#include <QTextStream>

#include <qjson/parser.h>
#include <qjson/serializer.h>

#include "cell.h"
#include "colormanager.h"
#include "document.h"
#include "globalstate.h"
#include "kdtree.h"
#include "sparsemap.h"

#include "documentio.h"

/* DocumentIo */

DocumentIo::DocumentIo(Document *doc)
    : document_(doc)
{

}

DocumentIo::~DocumentIo()
{

}

QVariant DocumentIo::save(QString &error)
{
  VariantMap output;

  output["version"] = version();
  output["data"] = serialize(error);

  if (output["data"].isNull())
    return QVariant();

  return QVariant(output);
}

bool DocumentIo::load(const QVariant &data, QString &error)
{
  if (data.type() != QVariant::Map) {
    error = QObject::tr("Invalid JSON format.");
    return false;
  }

  return deserialize(data.toMap(), error);
}

/* DocumentIoV1 */

DocumentIoV1::DocumentIoV1(Document *doc)
    : DocumentIo(doc)
{

}

DocumentIoV1::~DocumentIoV1()
{
  
}

QVariant DocumentIoV1::serialize(QString &error) const
{
  VariantMap root;

  const QSize &dim = document_->size();
  root["rows"] = dim.height();
  root["columns"] = dim.width();
  root["title"] = document_->title();
  root["author"] = document_->author();
  root["colors"] = serializeColors(error);
  root["stitches"] = serializeStitches(error);

  if (root["colors"].isNull() || root["stitches"].isNull())
    return QVariant();

  return QVariant(root);
}

bool DocumentIoV1::deserialize(const VariantMap &data, QString &error)
{
  QSize size(data["columns"].toInt(), data["rows"].toInt());

  if (size == QSize()) {
    error = QObject::tr("Invalid document size.");
    return false;
  }
  document_->setSize(size);

  document_->setTitle(data["title"].toString());
  document_->setAuthor(data["author"].toString());

  QVariant sv = data["stitches"];
  if (sv.isNull() || sv.type() != QVariant::List) {
    error = QObject::tr("No stitch data!");
    return false;
  }

  VariantList sl = sv.toList();
  if (!deserializeStitches(sl, error))
    return false;

  return true;
}

VariantList DocumentIoV1::serializeColors(QString &error) const
{
  Q_UNUSED(error);

  VariantList list;

  const ColorUsageTracker *colorTracker = document_->colorTracker();
  const QVector<const Color *> colors = colorTracker->colorList();

  foreach (const Color *c, colors) {
    VariantMap item;
    if (c->parent())
      item["category"] = c->parent()->id();
    item["id"] = c->id();
    item["name"] = c->name();
    item["color"] = c->color().name();
    list.append(QVariant(item));
  }

  return list;
}

VariantList DocumentIoV1::serializeStitches(QString &error) const
{
  Q_UNUSED(error);

  VariantList list;

  const CellMap &cells = document_->map()->cells();

  for (CellMap::ConstIterator it = cells.begin(); it != cells.end(); ++it) {
    const QPoint &pos = it.key();
    const Cell *cell = it.value();

    VariantMap map;
    map["x"] = pos.x();
    map["y"] = pos.y();
    
    VariantList features;
    for (int i = 0; i < CELL_COUNT; ++i) {
      if (cell->contains(i)) {
        VariantList cellItem;
        const Color *c = cell->color(i);
        if (c->parent())
          cellItem << c->parent()->id();
        else
          cellItem << QVariant();
        cellItem << c->id();
        cellItem << i;
        features.append(QVariant(cellItem));
      }
    }

    if (features.length() == 0)
      continue;

    map["features"] = features;

    list.append(QVariant(map));
  }

  return list;
}

bool DocumentIoV1::deserializeStitches(const VariantList &list, QString &error)
{
  SparseMap *map = document_->map();
  MetaColorManager *cm = GlobalState::self()->colorManager();

  int cnt = 0;
  foreach (const QVariant &v, list) {
    if (v.type() != QVariant::Map)
      continue;

    VariantMap m = v.toMap();
    int x = m["x"].toInt();
    int y = m["y"].toInt();
    
    if (x == 0 || y == 0)
      continue;

    VariantList features = m["features"].toList();
    if (features.length() == 0)
      continue;

    Cell *c = map->cellAt(QPoint(x, y));
    foreach (const QVariant &f, features) {
      const VariantList &fi = f.toList();

      const Color *color = cm->get(fi[0].toString(), fi[1].toString());
      c->addFeature(fi[2].toInt(), color);
    }
    c->createGraphicsItems();

    ++cnt;
  }

  if (!cnt) {
    error = QObject::tr("No stitch item!");
    return false;
  }

  return true;
}

/* DocumentFactory */

DocumentIo* DocumentFactory::defaultSerializer(Document *doc)
{
  return new DocumentIoV1(doc);
}

Document* DocumentFactory::load(const QString &path, QString &error)
{
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly)) {
    error = QObject::tr("Coule not open file %1.").arg(path);
    return NULL;
  }

  QTextStream stream(&file);
  QString jsonText = stream.readAll();
  file.close();

  QJson::Parser parser;
  bool ok;
  QVariant body = parser.parse(jsonText.toUtf8(), &ok);

  if (!ok) {
    error = QObject::tr("This file is corrupted.");
    return NULL;
  }

  VariantMap map = body.toMap();
  if (map.isEmpty()) {
    error = QObject::tr("Invalid document format.");
    return NULL;
  }

  Document *doc = new Document();

  DocumentIo *io = NULL;
  int version = map["version"].toInt();
  switch (map["version"].toInt()) {
    case 1:
      io = new DocumentIoV1(doc);
      break;
    default:
      error = QObject::tr("Unsupport version %1").arg(version);
      delete doc;
      return NULL;
  }

  const QVariant &subdata = map["data"];
  if (!io->load(subdata, error)) {
    delete doc;
    doc = NULL;
  }

  delete io;

  return doc;
}

Document* DocumentFactory::load(const QImage &image, ColorManager *manager,
				int width, const QColor *transparentColor)
{
  if (image.isNull())
    return NULL;

  int height = image.height() / (image.width() / (float) width);
  QImage scaled = image.scaled(width + 1, height + 1, Qt::KeepAspectRatio);

  Document *doc = new Document(QSize(width, height));
  SparseMap *map = doc->map();
  Color *transparent = NULL;
  if (transparentColor)
    transparent = new Color("Transparent color", "transparent", *transparentColor);

  KdTree kdtree(manager, transparent);
  
  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      QRgb pix = scaled.pixel(x, y);

      QColor c;
      if (qAlpha(pix) < 64)
	continue;
      else
	c = QColor(pix);

      const Color *stitch = kdtree.nearest(c);
      if (stitch == transparent || !stitch)
	continue;

      Cell *cell = map->cellAt(QPoint(x, y));
      cell->addFullStitch(stitch);
      cell->createGraphicsItems();
    }
  }
  
  if (transparent)
    delete transparent;

  return doc;
}

bool DocumentFactory::save(Document *doc, const QString &path, QString &error)
{
  QFile file(path);
  if (!file.open(QIODevice::WriteOnly)) {
    error = QObject::tr("Coule not open file %1.").arg(path);
    return false;
  }

  DocumentIo *io = defaultSerializer(doc);
  QVariant out = io->save(error);
  if (out.isNull()) {
    delete io;
    return false;
  }

  QJson::Serializer sr;
  //sr.setIndentMode(QJson::IndentMedium);
  QByteArray output = sr.serialize(out);

  if (output.isNull()) {
    delete io;
    error = QObject::tr("Serialization error.");
    file.close();
    return false;
  }

  QTextStream outStream(&file);
  outStream << output;
  file.close();
  
  delete io;

  return true;
}
