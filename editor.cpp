#include "document.h"
#include "editoractions.h"

#include "editor.h"

Editor::Editor(Document *document, QObject *parent)
    : QUndoStack(parent), document_(document)
{
  /* this would be okay */
  setUndoLimit(50);
}

Editor::~Editor()
{

}

void Editor::edit(EditorAction *action)
{
  push(action);

  emit changed();
}
