#ifndef _EDITOR_H_
#define _EDITOR_H_

#include <QUndoStack>

class Document;
class EditorAction;

enum ToolMode
{
  ToolMode_Select,
  ToolMode_Move,
  ToolMode_Rectangle,
  ToolMode_Full,
  ToolMode_Half,
  ToolMode_Petite,
  ToolMode_Quarter,
  ToolMode_Straight,
  ToolMode_Back,
  ToolMode_Knot
};

class Editor : public QUndoStack
{
  Q_OBJECT;
  
 public:
  Editor(Document *document, QObject *parent = NULL);
  ~Editor();

  void edit(EditorAction *action);

 signals:
  void changed();

 private:
  Document *document_;
};

#endif
