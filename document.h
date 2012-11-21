#ifndef _DOCUMENT_H_
#define _DOCUMENT_H_

#include <QSet>
#include <QSize>
#include <QObject>

#include "colormanager.h"

class QGraphicsItem;
class QGraphicsItemGroup;
class QGraphicsScene;

class Editor;
class SparseMap;
class StitchItem;

class Document : public QObject
{
  Q_OBJECT;

 public:
  Document(const QSize &size, QObject *parent = NULL);
  ~Document();

  const QString& name() const { return name_; }
  bool changed() const { return changed_; }

  Editor* editor() { return editor_; }
  QGraphicsScene* scene() { return scene_; }
  ColorUsageTracker* colorTracker() { return &colors_; }
  SparseMap* map() { return map_; }
  const QSize& size() { return size_; }

  void acquire(StitchItem *item);
  void release(StitchItem *item);

 signals:
  void documentChanged();

 public slots:
  void setSize(const QSize &size);

 private slots:
  void documentChanged_();

 private:
  void resetGrid();
  QGraphicsItemGroup* grid();

 private:
  QString name_;
  QSize size_;

  bool changed_;

  Editor *editor_;
  SparseMap *map_;
  QGraphicsScene *scene_;
  QGraphicsItemGroup *grid_;
  ColorUsageTracker colors_;
  QSet<StitchItem *> stitches_;
};

#endif
