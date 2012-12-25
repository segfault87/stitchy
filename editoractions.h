#ifndef _EDITORACTIONS_H_
#define _EDITORACTIONS_H_

#include <QList>
#include <QUndoCommand>

class Canvas;
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

class CanvasAction : public EditorAction
{
 public:
  CanvasAction(Document *document, Canvas *canvas);
  virtual ~CanvasAction();

 protected:
  Canvas *canvas_;
};

class MergeAction : public EditorAction
{
 public:
  MergeAction(Document *document, SparseMap *map);
  virtual ~MergeAction();

 protected:
  void replaceWith(const QList<Cell> &cells);
  void mergeWith_(const QList<Cell> &cells);

  QList<Cell> previousState_;
  QList<Cell> drawn_;
};

class ActionDraw : public MergeAction
{
 public:
  ActionDraw(Document *document, SparseMap *map);
  ~ActionDraw();

  void redo();
  void undo();
};

class ActionErase : public MergeAction
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

class ActionPaste : public CanvasAction
{
 public:
  ActionPaste(Document *document, Canvas *canvas, const QByteArray &data);
  ~ActionPaste();

  void redo();
  void undo();

 private:
  QByteArray data_;
};

class ActionFloatMove : public CanvasAction
{
 public:
  ActionFloatMove(Document *document, Canvas *canvas,
		  const QPoint &from, const QPoint &to);
  ~ActionFloatMove();

  void redo();
  void undo();
  
 private:
  QPoint from_;
  QPoint to_;
};

class ActionFloatCommit : public MergeAction
{
 public:
  ActionFloatCommit(Document *document, Canvas *canvas);
  ~ActionFloatCommit();

  void redo();
  void undo();

 private:
  Canvas *canvas_;
  QByteArray data_;
};

#endif
