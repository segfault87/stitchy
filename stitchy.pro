CONFIG += qt debug
TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
LIBS += -lqjson

# Input
RESOURCES += stitchy.qrc

FORMS += \
  coloreditor.ui \
  newdocumentdialog.ui

HEADERS += \
  canvas.h \
  cell.h \
  color.h \
  coloreditor.h \
  colormanager.h \
  common.h \
  document.h \
  documentio.h \
  editor.h \
  editoractions.h \
  globalstate.h \
  mainwindow.h \
  newdocumentdialog.h \
  palettemodel.h \
  palettewidget.h \
  selection.h \
  selectiongroup.h \
  settings.h \
  sparsemap.h \
  stitch.h \
  utils.h

SOURCES += \
  canvas.cpp \
  cell.cpp \
  color.cpp \
  coloreditor.cpp \
  colormanager.cpp \
  document.cpp \
  documentio.cpp \
  editor.cpp \
  editoractions.cpp \
  globalstate.cpp \
  mainwindow.cpp \
  newdocumentdialog.cpp \
  palettemodel.cpp \
  palettewidget.cpp \
  selection.cpp \
  selectiongroup.cpp \
  settings.cpp \
  sparsemap.cpp \
  stitch.cpp \
  utils.cpp \
  main.cpp
