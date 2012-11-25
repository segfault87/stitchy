#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QMainWindow>

class QActionGroup;
class QCloseEvent;
class QMenu;

class Canvas;
class Document;
class GlobalState;
class MetaColorManager;
class PaletteWidget;
class Settings;

class MainWindow : public QMainWindow
{
  Q_OBJECT;

 public:
  MainWindow(QWidget *parent = NULL);
  ~MainWindow();

 signals:
  void needsUpdate();
  void documentChanged(Document *document);

 private slots:
  void newFile();
  void openFile();
  void closeFile();
  void saveFile();
  void saveFileAs();
  void quit();
  void viewModeAction(QAction *action);
  void showColorEditor();
  void toolModeAction(QAction *action);

  void setActiveDocument(Document *document);

 private:
  QAction* createAction(const QString &name, QObject *receiver,
                        const char *slot, const QKeySequence &shortcut,
                        const QIcon &icon);
  QAction* createAction(const QString &name, const QKeySequence &shortcut,
                        const QIcon &icon);
  void closeEvent(QCloseEvent *event);
  bool confirmClose();
  bool saveDocument(bool newName);

  void initWidgets();
  void initActions();
  void initMenus();
  void initToolbars();
  void initConnections();

 private:
  PaletteWidget *palette_;
  Canvas *graphicsView_;
  Settings *settings_;
  GlobalState *state_;

  QMenu *menuFile_;
  QMenu *menuEdit_;
  QMenu *menuView_;
  QMenu *menuTool_;
  QMenu *menuWindow_;

  /* file actions */
  QAction *actionNewFile_;
  QAction *actionOpenFile_;
  QAction *actionCloseFile_;
  QAction *actionSaveFile_;
  QAction *actionSaveFileAs_;
  QAction *actionQuit_;
  
  /* edit actions */
  QAction *actionUndo_;
  QAction *actionRedo_;

  /* view actions */
  QAction *actionZoomIn_;
  QAction *actionZoomOut_;
  QAction *actionZoomReset_;
  QActionGroup *actionViewMode_;
  QAction *actionViewFull_;
  QAction *actionViewSimplified_;
  QAction *actionViewSymbols_;
  QAction *actionViewGrids_;

  /* tool actions */
  QActionGroup *actionGroupMode_;
  QAction *actionModeSelect_;
  QAction *actionModeMove_;
  QAction *actionModeErase_;
  QAction *actionModeRectangle_;
  QAction *actionModeDrawFull_;
  QAction *actionModeDrawHalf_;
  QAction *actionModeDrawPetite_;
  QAction *actionModeDrawQuarter_;

  /* window actions */
  QAction *actionColorEditor_;
};

#endif
