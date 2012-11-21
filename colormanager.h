#ifndef _COLORMANAGER_H_
#define _COLORMANAGER_H_

#include <QHash>
#include <QObject>
#include <QSet>
#include <QVector>

#include "color.h"

#define COLOR_TABLE ":/res/colors.json"

class StitchItem;

typedef QHash<const Color *, QSet<StitchItem *> > BackRefMap;

class ColorUsageTracker
{
 public:
  ColorUsageTracker();
  ~ColorUsageTracker();

  void acquire(StitchItem *item);
  void release(StitchItem *item);

  const BackRefMap& backrefMap() const { return backrefMap_; }
  const QSet<StitchItem *>* items(const Color *) const;

 private:
  BackRefMap backrefMap_;
};

class ColorManager : public QObject
{
  Q_OBJECT;

 public:
  ColorManager();
  ColorManager(const QString &id, const QString &name,
               QObject *parent = NULL);
  ColorManager(const QString &id, const QString &name,
               const QString &path, QObject *parent = NULL);
  ~ColorManager();

  void load(const QVariant &list);

  const QString& name() const { return name_; }
  const QString& id() const { return id_; }

  void add(const Color &c);
  void insert(const Color &c, int before);
  void remove(const QString &key);
  void swap(int index1, int index2);
  const Color* get(const QString &key) const;
  const Color* itemAt(int index) const;
  int count() const;
  void clear();

  const QVector<const Color *>& colorList() const { return colorList_; }

 signals:
  void listChanged();
  void colorAppended();
  void colorInserted(int before);
  void colorDeleted(int index);
  void colorSwapped(int index1, int index2);

 private:
  QString id_;
  QString name_;
  QHash<QString, Color> colorMap_;
  QVector<const Color *> colorList_;
};

class MetaColorManager : public QObject
{
 public:
  MetaColorManager(QObject *parent = NULL);
  MetaColorManager(const QString &path, QObject *parent = NULL);
  ~MetaColorManager();

  void populateMyColors();

  const Color* get(const QString &category, const QString &id);

  ColorManager* createColorManager(const QString &id, const QString &name);
  ColorManager* colorManager(const QString &id);
  ColorManager* localSwatches() { return &localSwatches_; }
  QList<ColorManager *>& colorManagers() { return colorManagerList_; }

 private:
  QList<ColorManager *> colorManagerList_;
  QHash<QString, ColorManager *> colorManagers_;
  ColorManager localSwatches_;
};

#endif
