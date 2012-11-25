#ifndef _GLOBALSTATE_H_
#define _GLOBALSTATE_H_

#include <QObject>

#include "editor.h"
#include "stitch.h"

class QUndoGroup;

class Color;
class Document;
class MetaColorManager;

class GlobalState : public QObject
{
  Q_OBJECT;

 public:
  static GlobalState* self() { return instance_; }

  GlobalState(QObject *parent = NULL);
  ~GlobalState();

  MetaColorManager* colorManager() { return colorManager_; }
  RenderingMode renderingMode() { return renderingMode_; }
  ToolMode toolMode() { return toolMode_; }
  const Color* color() { return color_; }
  Document* activeDocument() { return document_; }
  QUndoGroup* undoGroup() { return undoGroup_; }
  
 public slots:
  void setRenderingMode(RenderingMode mode);
  void setToolMode(ToolMode mode);
  void setColor(const Color *color);
  void setActiveDocument(Document *document);

 private:
  static GlobalState *instance_;

  MetaColorManager *colorManager_;
  RenderingMode renderingMode_;
  ToolMode toolMode_;
  const Color *color_;
  Document *document_;
  QUndoGroup *undoGroup_;
};

#endif
