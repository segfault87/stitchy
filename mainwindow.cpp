#include <QApplication>
#include <QCloseEvent>
#include <QDockWidget>
#include <QGraphicsView>
#include <QMenuBar>
#include <QMessageBox>
#include <QUndoGroup>

#include "canvas.h"
#include "color.h"
#include "coloreditor.h"
#include "document.h"
#include "globalstate.h"
#include "palettewidget.h"
#include "settings.h"
#include "utils.h"

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
  settings_ = new Settings();
  state_ = new GlobalState(this);
  colorManager_ = new MetaColorManager(COLOR_TABLE, this);

  initWidgets();
  initActions();
  initMenus();
  initToolbars();
  initConnections();

  setActiveDocument(new Document(QSize(30, 30)));

  QByteArray state = settings_->state();
  QByteArray geometry = settings_->geometry();

  if (!state.isEmpty())
    restoreState(state);
  if (!geometry.isEmpty())
    restoreGeometry(geometry);

  setWindowTitle(tr("Stitchy"));
}

MainWindow::~MainWindow()
{
  settings_->setState(saveState());
  settings_->setGeometry(saveGeometry());

  delete settings_;
}

void MainWindow::newFile()
{
  if (!confirmClose())
    return;
}

void MainWindow::openFile()
{
  if (!confirmClose())
    return;
}

void MainWindow::closeFile()
{
  if (!confirmClose())
    return;

  if (state_->activeDocument()) {
    delete state_->activeDocument();
    setActiveDocument(NULL);
  }
}

void MainWindow::saveFile()
{
  saveDocument(false);
}

void MainWindow::saveFileAs()
{
  saveDocument(true);
}

void MainWindow::quit()
{
  if (confirmClose())
    qApp->quit();
}

void MainWindow::showColorEditor()
{
  QDialog *dialog = new ColorEditor(colorManager_, this);
  dialog->show();
}

void MainWindow::setActiveDocument(Document *document)
{
  state_->setActiveDocument(document);

  if (state_->activeDocument()) {
    state_->undoGroup()->setActiveStack(document->editor());
    graphicsView_->setScene(state_->activeDocument()->scene());
  } else {
    state_->undoGroup()->setActiveStack(NULL);
    graphicsView_->setScene(NULL);
  }

  emit needsUpdate();
}

QAction* MainWindow::createAction(const QString &name, QObject *receiver,
                                  const char *slot, const QKeySequence &shortcut,
                                  const QIcon &icon)
{
  QAction *action = new QAction(name, this);
  if (!icon.isNull())
    action->setIcon(icon);
  if (!shortcut.isEmpty())
    action->setShortcut(shortcut);
  if (receiver && slot)
    connect(action, SIGNAL(triggered()), receiver, slot);

  return action;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if (confirmClose())
    event->accept();
  else
    event->ignore();
}

bool MainWindow::confirmClose()
{
  if (!state_->activeDocument())
    return true;

  Document *activeDocument = state_->activeDocument();

  if (state_->activeDocument()->changed()) {
    switch (QMessageBox::question(
        this,
        tr("Confirm"),
        tr("The document \"%1\" has been modified. Do you want to save it?")
          .arg(activeDocument->name().isEmpty() ? "Untitled" : activeDocument->name()),
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Cancel)) {
        case QMessageBox::Yes:
          if (saveDocument(false))
            return true;
          else
            return false;
          break;
        case QMessageBox::No:
          break;
        case QMessageBox::Cancel:
        default:
          return false;
    }
  }

  return true;
}

bool MainWindow::saveDocument(bool newName)
{
  if (!state_->activeDocument())
    return false;

  Document *activeDocument = state_->activeDocument();

  if (activeDocument->name().isEmpty())
    newName = true;

  QString filename;
  if (newName) {
    
  } else {
    filename = activeDocument->name();
  }

  return true;
}

void MainWindow::initActions()
{
  actionNewFile_ = createAction(tr("&New..."),
                                this,
                                SLOT(newFile()),
                                QKeySequence::New,
                                Utils::icon("document-new"));
  actionOpenFile_ = createAction(tr("&Open..."),
                                 this,
                                 SLOT(openFile()),
                                 QKeySequence::Open,
                                 Utils::icon("document-open"));
  actionCloseFile_ = createAction(tr("&Close"),
                                  this,
                                  SLOT(closeFile()),
                                  QKeySequence::Close,
                                  Utils::icon("document-close"));
  actionSaveFile_ = createAction(tr("&Save"),
                                 this,
                                 SLOT(saveFile()),
                                 QKeySequence::Save,
                                 Utils::icon("document-save"));
  actionSaveFileAs_ = createAction(tr("Save &As..."),
                                   this,
                                   SLOT(saveFileAs()),
                                   QKeySequence::SaveAs,
                                   Utils::icon("document-save-as"));
  actionQuit_ = createAction(tr("&Quit"),
                             this,
                             SLOT(quit()),
                             QKeySequence::Quit,
                             Utils::icon("application-exit"));

  actionUndo_ = state_->undoGroup()->createUndoAction(this, tr("Undo"));
  actionUndo_->setShortcut(QKeySequence::Undo);
  actionUndo_->setIcon(Utils::icon("edit-undo"));
  actionRedo_ = state_->undoGroup()->createRedoAction(this, tr("Redo"));
  actionRedo_->setShortcut(QKeySequence::Redo);
  actionRedo_->setIcon(Utils::icon("edit-redo"));

  actionColorEditor_ = createAction(tr("&Colors..."),
                                    this,
                                    SLOT(showColorEditor()),
                                    QKeySequence("F12"),
                                    QIcon());
}

void MainWindow::initMenus()
{
  menuFile_ = menuBar()->addMenu(tr("&File"));
  menuFile_->addAction(actionNewFile_);
  menuFile_->addAction(actionOpenFile_);
  menuFile_->addSeparator();
  menuFile_->addAction(actionSaveFile_);
  menuFile_->addAction(actionSaveFileAs_);
  menuFile_->addSeparator();
  menuFile_->addAction(actionCloseFile_);
  menuFile_->addSeparator();
  menuFile_->addAction(actionQuit_);

  menuEdit_ = menuBar()->addMenu(tr("&Edit"));
  menuEdit_->addAction(actionUndo_);
  menuEdit_->addAction(actionRedo_);

  menuView_ = menuBar()->addMenu(tr("&View"));

  menuWindow_ = menuBar()->addMenu(tr("&Window"));
  menuWindow_->addAction(actionColorEditor_);
}

void MainWindow::initToolbars()
{

}

void MainWindow::initWidgets()
{
  palette_ = new PaletteWidget(colorManager_);
  QDockWidget *paletteDock = new QDockWidget(tr("Color Swatches"));
  paletteDock->setObjectName("palette");
  paletteDock->setWidget(palette_);
  paletteDock->setFeatures(QDockWidget::DockWidgetMovable |
                           QDockWidget::DockWidgetFloatable);
  addDockWidget(Qt::LeftDockWidgetArea, paletteDock);

  graphicsView_ = new Canvas(this);
  setCentralWidget(graphicsView_);
}

void MainWindow::initConnections()
{
  connect(this, SIGNAL(needsUpdate()),
          graphicsView_, SLOT(update()));
  connect(palette_, SIGNAL(colorSelected(const Color *)),
          state_, SLOT(setColor(const Color *)));
  connect(palette_, SIGNAL(userColorSetIsEmpty()),
          this, SLOT(showColorEditor()));
}
