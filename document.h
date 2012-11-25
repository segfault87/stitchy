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
  Document(QObject *parent = NULL);
  Document(const QSize &size, QObject *parent = NULL);
  ~Document();

  const QSize& size() { return size_; }
  const QString& name() const { return name_; }
  const QString& author() const { return author_; }
  bool changed() const { return changed_; }

  Editor* editor() { return editor_; }
  QGraphicsScene* scene() { return scene_; }
  ColorUsageTracker* colorTracker() { return &colors_; }
  SparseMap* map() { return map_; }

  void acquire(StitchItem *item);
  void release(StitchItem *item);

 signals:
  void documentChanged();
  void documentSaved();

 public slots:
  void setName(const QString &name);
  void setAuthor(const QString &author);
  void setSize(const QSize &size);
  void setChanged(bool b);

 private slots:
  void documentChanged_();

 private:
  void resetGrid();
  QGraphicsItemGroup* grid();

 private:
  QString name_;
  QString author_;
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
