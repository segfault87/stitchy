#include <QFile>
#include <QMap>
#include <QTextStream>

#include <qjson/parser.h>
#include <qjson/serializer.h>

#include "cell.h"
#include "document.h"
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
  const VariantMap &map = data.toMap();

  if (map["version"].toInt() != version()) {
    error = QObject::tr("Document version mismatch!");
    return false;
  }

  const VariantMap &sdata = map["data"].toMap();
  return deserialize(sdata, error);
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

  return false;
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

  bool ret;
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
