#include <QFile>
#include <QTextStream>
#include <QVariant>

#include <qjson/parser.h>

#include "color.h"
#include "settings.h"
#include "stitch.h"

#include "colormanager.h"

ColorManager::ColorManager(QObject *parent)
    : QObject(parent), name_(QObject::tr("My Colors"))
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
  if (!isDependent_) {
    for (QHash<QString, const Color *>::iterator it = colorMap_.begin();
	 it != colorMap_.end();
	 ++it) {
      delete it.value();
    }
  }

  colorMap_.clear();
  colorList_.clear();
}

void ColorManager::load(const QVariant &l)
{
  const QList<QVariant> &list = l.toList();
  foreach(const QVariant &i, list) {
    const QMap<QString, QVariant> &map = i.toMap();

    Color *c = new Color(map["name"].toString(),
			 map["id"].toString(),
			 QColor(map["color"].toString()));
    c->setParent(this);
    
    add(c);
  }
}

void ColorManager::add(const Color *c)
{
  if (!colorMap_.contains(c->id())) {
    colorMap_.insert(c->id(), c);
    emit colorAppended();
    colorList_.append(colorMap_[c->id()]);
    emit listChanged();
  }
}

void ColorManager::insert(const Color *c, int before)
{
  if (!colorMap_.contains(c->id())) {
    colorMap_.insert(c->id(), c);
    emit colorInserted(before);
    colorList_.insert(before, colorMap_[c->id()]);
    emit listChanged();
  }
}

void ColorManager::remove(const QString &id)
{
  QHash<QString, const Color *>::Iterator it = colorMap_.find(id);
  if (it == colorMap_.end())
    return;

  int index = colorList_.indexOf(*it);
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

  emit listChanged();
  emit colorSwapped(index1, index2);
}

const Color* ColorManager::get(const QString &id) const
{
  QHash<QString, const Color *>::const_iterator i = colorMap_.find(id);
  if (i == colorMap_.end())
    return NULL;

  return *i;
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

void ColorManager::clear()
{
  colorMap_.clear();
  colorList_.clear();
}

ColorUsageTracker::ColorUsageTracker(QObject *parent)
    : ColorManager(parent), total_(0.0)
{
  setDependent(true);
}

ColorUsageTracker::~ColorUsageTracker()
{

}

void ColorUsageTracker::acquire(StitchItem *item)
{
  const Color *key = item->color();
  backrefMap_[key].insert(item);
  weightMap_[key] += item->weight();
  total_ += item->weight();
  const Color *c = item->color();
  if (colorMap_.find(c->id()) == colorMap_.end())
    add(c);
}

void ColorUsageTracker::release(StitchItem *item)
{
  const Color *key = item->color();
  QSet<StitchItem *> &set = backrefMap_[key];
  total_ -= item->weight();
  weightMap_[key] -= item->weight();
  set.remove(item);
  if (set.size() == 0) {
    remove(item->color()->id());
    weightMap_.remove(key);
    backrefMap_.remove(key);
  }
}

const QSet<StitchItem *>* ColorUsageTracker::items(const Color *color) const
{
  BackRefMap::ConstIterator it = backrefMap_.find(color);

  if (it == backrefMap_.end())
    return NULL;

  return &(*it);
}

MetaColorManager::MetaColorManager(QObject *parent)
    : QObject(parent)
{
  localSwatches_.setDependent(true);
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

  populateMyColors();
}

MetaColorManager::~MetaColorManager()
{
  for (QHash<QString, ColorManager *>::Iterator it = colorManagers_.begin();
       it != colorManagers_.end();
       ++it) {
    delete it.value();
  }
}

void MetaColorManager::populateMyColors()
{
  localSwatches_.clear();

  QStringList colors = Settings::self()->myColors();
  foreach (const QString &s, colors) {
    QStringList fr = s.split("|");
    const Color *c = get(fr[0], fr[1]);
    if (c)
      localSwatches_.add(c);
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
