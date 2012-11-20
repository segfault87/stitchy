#include <QUndoGroup>

#include "globalstate.h"

GlobalState* GlobalState::instance_ = NULL;

GlobalState::GlobalState(QObject *parent)
    : QObject(parent)
{
  instance_ = this;

  renderingMode_ = RenderingMode_Full;
  toolMode_ = ToolMode_Full;
  color_ = NULL;
  document_ = NULL;
  undoGroup_ = new QUndoGroup(this);
}

GlobalState::~GlobalState()
{
  
}

void GlobalState::setRenderingMode(RenderingMode mode)
{
  renderingMode_ = mode;
}

void GlobalState::setToolMode(ToolMode mode)
{
  toolMode_ = mode;
}

void GlobalState::setColor(const Color *color)
{
  color_ = color;
}

void GlobalState::setActiveDocument(Document *doc)
{
  document_ = doc;
}
