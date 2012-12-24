#ifndef _DOCUMENT_H_
#define _DOCUMENT_H_

#include <QGraphicsScene>
#include <QSet>
#include <QSize>
#include <QObject>

#include "colormanager.h"

class QGraphicsItem;
class QGraphicsItemGroup;

class Editor;
class Selection;
class SparseMap;
class StitchItem;

class Document : public QGraphicsScene
{
  Q_OBJECT;

 public:
  Document(QObject *parent = NULL);
  Document(const QSize &size, QObject *parent = NULL);
  ~Document();

  const QSize& size() const { return size_; }
  QRect boundingRect() const;
  const QString& name() const { return name_; }
  const QString& title() const { return title_; }
  const QString& author() const { return author_; }
  bool changed() const { return changed_; }

  Editor* editor() { return editor_; }
  ColorUsageTracker* colorTracker() { return &colors_; }
  SparseMap* map() { return map_; }
  
  Selection* createSelection();
  Selection* createSelection(const QRect &region);
  Selection* selection() { return selection_; }
  void clearSelection();
  
  void acquire(StitchItem *item);
  void release(StitchItem *item);

 signals:
  void documentChanged();
  void documentSaved();
  void madeSelection(const QRect &rect);

 public slots:
  void setName(const QString &name);
  void setTitle(const QString &title);
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
  QString title_;
  QString author_;
  QSize size_;

  bool changed_;

  Selection *selection_;
  Editor *editor_;
  SparseMap *map_;
  QGraphicsItemGroup *grid_;
  ColorUsageTracker colors_;
  QSet<StitchItem *> stitches_;
};

#endif
