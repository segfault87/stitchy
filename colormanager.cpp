#include <QFile>
#include <QTextStream>
#include <QVariant>

#include <qjson/parser.h>

#include "color.h"
#include "stitch.h"

#include "colormanager.h"

ColorUsageTracker::ColorUsageTracker()
{
  
}

ColorUsageTracker::~ColorUsageTracker()
{

}

void ColorUsageTracker::acquire(StitchItem *item)
{
  backrefMap_[item->color()].insert(item);
}

void ColorUsageTracker::release(StitchItem *item)
{
  QSet<StitchItem *> &set = backrefMap_[item->color()];

  set.remove(item);
  if (set.size() == 0)
    backrefMap_.remove(item->color());
}

const QSet<StitchItem *>* ColorUsageTracker::items(const Color *color) const
{
  QHash<const Color *, QSet<StitchItem *> >::ConstIterator it =
      backrefMap_.find(color);

  if (it == backrefMap_.end())
    return NULL;

  return &(*it);
}

ColorManager::ColorManager()
    : QObject(), name_(QObject::tr("My Colors"))
{

}

ColorManager::ColorManager(const QString &id, const QString &name,
                           QObject *parent)
    : QObject(parent), id_(id), name_(name)
{

}

ColorManager::ColorManager(const QString &id, const QString &name,
                           const QString &path, QObject *parent)
    : QObject(parent), id_(id), name_(name)
{
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly))
    qFatal("Could not open color table!");
  QTextStream stream(&file);
  QString jsonText = stream.readAll();
  file.close();
  
  QJson::Parser parser;
  bool ok;
  QVariant result = parser.parse(jsonText.toUtf8(), &ok);
  if (!ok) {
    qFatal("Could not read color table!");
  }

  load(result);
}

ColorManager::~ColorManager()
{
  
}

void ColorManager::load(const QVariant &l)
{
  const QList<QVariant> &list = l.toList();
  foreach(const QVariant &i, list) {
    const QMap<QString, QVariant> &map = i.toMap();

    Color c(map["name"].toString(),
            map["id"].toString(),
            QColor(map["color"].toString()));
    c.setParent(this);
    
    add(c);
  }
}

void ColorManager::add(const Color &c)
{
  if (!colorMap_.contains(c.id())) {
    colorMap_.insert(c.id(), c);
    emit colorAppended();
    colorList_.append(&colorMap_[c.id()]);
    emit listChanged();
  }
}

void ColorManager::insert(const Color &c, int before)
{
  if (!colorMap_.contains(c.id())) {
    colorMap_.insert(c.id(), c);
    emit colorInserted(before);
    colorList_.insert(before, &colorMap_[c.id()]);
    emit listChanged();
  }
}

void ColorManager::remove(const QString &id)
{
  QHash<QString, Color>::Iterator it = colorMap_.find(id);
  if (it == colorMap_.end())
    return;

  int index = colorList_.indexOf(&(*it));
  if (index >= 0) {
    colorList_.remove(index);
    emit colorDeleted(index);
  }
  
  colorMap_.erase(it);

  emit listChanged();
}

void ColorManager::swap(int index1, int index2)
{
  const Color *temp;
  int size = colorList_.size();

  if (index1 >= size || index2 >= size)
    return;

  temp = colorList_[index1];
  colorList_[index1] = colorList_[index2];
  colorList_[index2] = temp;
}

const Color* ColorManager::get(const QString &id) const
{
  QHash<QString, Color>::const_iterator i = colorMap_.find(id);
  if (i == colorMap_.end())
    return NULL;

  return &(*i);
}

const Color* ColorManager::itemAt(int index) const
{
  if (index < 0 || index >= colorList_.count())
    return NULL;

  return colorList_[index];
}

int ColorManager::count() const
{
  return colorList_.size();
}

MetaColorManager::MetaColorManager(QObject *parent)
    : QObject(parent)
{

}

MetaColorManager::MetaColorManager(const QString &path, QObject *parent)
    : QObject(parent)
{
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly))
    qFatal("Could not open color table!");
  QTextStream stream(&file);
  QString jsonText = stream.readAll();
  file.close();
  
  QJson::Parser parser;
  bool ok;
  QVariant result = parser.parse(jsonText.toUtf8(), &ok);
  if (!ok) {
    qFatal("Could not read color table!");
  }

  const QList<QVariant> &list = result.toList();
  foreach (const QVariant &v, list) {
    const QMap<QString, QVariant> &map = v.toMap();
    ColorManager *cm = createColorManager(map["id"].toString(), map["name"].toString());
    cm->load(map["colors"]);
  }
}

MetaColorManager::~MetaColorManager()
{
  for (QHash<QString, ColorManager *>::Iterator it = colorManagers_.begin();
       it != colorManagers_.end();
       ++it) {
    delete it.value();
  }
}

const Color* MetaColorManager::get(const QString &category, const QString &id)
{
  QHash<QString, ColorManager *>::Iterator it = colorManagers_.find(category);

  if (it == colorManagers_.end())
    return NULL;

  return it.value()->get(id);
}

ColorManager* MetaColorManager::createColorManager(const QString &id,
                                                   const QString &name)
{
  QHash<QString, ColorManager *>::Iterator it = colorManagers_.find(id);

  if (it != colorManagers_.end()) {
    colorManagerList_.removeAll(it.value());
    delete it.value();
  }

  ColorManager *cm = new ColorManager(id, name);
  colorManagers_[id] = cm;
  colorManagerList_.append(cm);

  return cm;
}

ColorManager* MetaColorManager::colorManager(const QString &id)
{
  if (colorManagers_.contains(id))
    return colorManagers_[id];

  return NULL;
}
