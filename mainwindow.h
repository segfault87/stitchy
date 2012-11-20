#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QIcon>
#include <QMainWindow>

class QCloseEvent;
class QMenu;

class Canvas;
class ColorManager;
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

 private slots:
  void newFile();
  void openFile();
  void closeFile();
  void saveFile();
  void saveFileAs();
  void quit();

  void setActiveDocument(Document *document);

 private:
  QIcon icon(const QString &name);
  QAction* createAction(const QString &name, QObject *receiver,
                        const char *slot, const QKeySequence &shortcut,
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
  MetaColorManager *colorManager_;
  ColorManager *activeColorManager_;
  Settings *settings_;
  GlobalState *state_;

  QMenu *menuFile_;
  QMenu *menuEdit_;
  QMenu *menuView_;

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
};

#endif
