#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QMainWindow>

class QActionGroup;
class QClipboard;
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
  void importFile();
  void closeFile();
  void saveFile();
  void saveFileAs();
  void quit();
  void about();
  void viewModeAction(QAction *action);
  void showColorEditor();
  void toolModeAction(QAction *action);
  void updateTitle();
  void setActiveDocument(Document *document);
  void documentChangeAction(Document *document);
  void selectionChanged(const QRect &rect);
  void selectionCleared();
  void clipboardChanged();

 private:
  QAction* createAction(const QString &name, QObject *receiver,
                        const char *slot, const QKeySequence &shortcut,
                        const QIcon &icon);
  QAction* createAction(const QString &name, const QKeySequence &shortcut,
                        const QIcon &icon);
  void closeEvent(QCloseEvent *event);
  bool confirmClose();
  bool saveDocument(bool newName);
  void setEnabled(QList<QAction *> &actions, bool enabled);

  void initWidgets();
  void initActions();
  void initMenus();
  void initToolbars();
  void initConnections();
  void initConnections(Document *doc);
  void disconnectConnections(Document *doc);

 private:
  PaletteWidget *palette_;
  Canvas *canvas_;
  Settings *settings_;
  GlobalState *state_;
  QClipboard *clipboard_;

  QMenu *menuFile_;
  QMenu *menuEdit_;
  QMenu *menuView_;
  QMenu *menuTool_;
  QMenu *menuWindow_;
  QMenu *menuHelp_;

  /* file actions */
  QAction *actionNewFile_;
  QAction *actionOpenFile_;
  QAction *actionImportFile_;
  QAction *actionCloseFile_;
  QAction *actionSaveFile_;
  QAction *actionSaveFileAs_;
  QAction *actionQuit_;
  
  /* edit actions */
  QAction *actionUndo_;
  QAction *actionRedo_;
  QAction *actionCut_;
  QAction *actionCopy_;
  QAction *actionPaste_;
  QAction *actionDeleteSelected_;
  QAction *actionDocumentProperties_;

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

  /* help actions */
  QAction *actionAbout_;
  QAction *actionAboutQt_;

  QList<QAction *> documentActions_;
  QList<QAction *> selectionActions_;
};

#endif
