#ifndef _EDITORACTIONS_H_
#define _EDITORACTIONS_H_

#include <QList>
#include <QUndoCommand>

class Cell;
class SelectionGroup;
class SparseMap;

class EditorAction : public QUndoCommand
{
 public:
  EditorAction(Document *document);
  virtual ~EditorAction();

 protected:
  Document *document_;
};

class ActionMerge : public EditorAction
{
 public:
  ActionMerge(Document *document, SparseMap *map);
  virtual ~ActionMerge();

 protected:
  void replaceWith(const QList<Cell> &cells);
  void mergeWith_(const QList<Cell> &cells);

  QList<Cell> previousState_;
  QList<Cell> drawn_;
};

class ActionDraw : public ActionMerge
{
 public:
  ActionDraw(Document *document, SparseMap *map);
  ~ActionDraw();

  void redo();
  void undo();
};

class ActionErase : public ActionMerge
{
 public:
  ActionErase(Document *document, SparseMap *map);
  ~ActionErase();

  void redo();
  void undo();
};

class ActionMove : public EditorAction
{
 public:
  ActionMove(Document *document, const QPoint &originalPosition,
	     const QSize &size, SelectionGroup *group);
  ~ActionMove();

  void redo();
  void undo();

 private:
  void setData(SelectionGroup *group);

 private:
  QSize size_;
  QPoint originalPosition_;
  QPoint targetPosition_;
  SparseMap *map_;
  QList<Cell> previousState_;
};

#endif
