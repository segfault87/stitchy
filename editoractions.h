#ifndef _EDITORACTIONS_H_
#define _EDITORACTIONS_H_

#include <QList>
#include <QUndoCommand>

class Cell;
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
  void mergeWith(const QList<Cell> &cells);

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

#endif
