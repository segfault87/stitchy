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

  void redo();
  void undo();

  void replaceWith(const QList<Cell> &cells);

 private:
  QList<Cell> previousState_;
  QList<Cell> drawn_;
};

class ActionDraw : public ActionMerge
{
 public:
  ActionDraw(Document *document, SparseMap *map);
  ~ActionDraw();
};

class ActionErase : public ActionMerge
{
 public:
  ActionErase(Document *document, SparseMap *map);
  ~ActionErase();
};

#endif
