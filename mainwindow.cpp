#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsView>
#include <QMenuBar>
#include <QMessageBox>
#include <QToolBar>
#include <QUndoGroup>

#include "canvas.h"
#include "color.h"
#include "coloreditor.h"
#include "document.h"
#include "documentio.h"
#include "documentpropertiesdialog.h"
#include "globalstate.h"
#include "importdialog.h"
#include "newdocumentdialog.h"
#include "palettewidget.h"
#include "selectiongroup.h"
#include "settings.h"
#include "utils.h"

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
  settings_ = new Settings();
  state_ = new GlobalState(this);
  clipboard_ = QApplication::clipboard();

  initWidgets();
  initActions();
  initMenus();
  initToolbars();
  initConnections();

  setEnabled(selectionActions_, false);

  QByteArray state = settings_->state();
  QByteArray geometry = settings_->geometry();

  if (!state.isEmpty())
    restoreState(state);
  if (!geometry.isEmpty())
    restoreGeometry(geometry);

  setActiveDocument(NULL);
}

MainWindow::~MainWindow()
{
  if (state_->activeDocument()) {
    delete state_->activeDocument();
    state_->setActiveDocument(NULL);
  }

  settings_->setState(saveState());
  settings_->setGeometry(saveGeometry());

  delete settings_;
}

void MainWindow::newFile()
{
  if (!confirmClose())
    return;

  NewDocumentDialog diag(this);
  diag.show();
  diag.exec();

  if (diag.result() == QDialog::Accepted) {
    Document *d = new Document(diag.documentSize(), this);
    d->setTitle(diag.title());
    d->setAuthor(diag.author());
    setActiveDocument(d);
  }
}

void MainWindow::openFile()
{
  closeFile();

  QString path = QFileDialog::getOpenFileName(
      this,
      tr("Choose a file to load"),
      QString(),
      tr("Stitchy Document (*.stitchy)"));

  if (!path.isEmpty()) {
    QString error;
    Document *doc = DocumentFactory::load(path, error);
    if (!doc) {
      QMessageBox::critical(this, tr("Error"), tr("Error loading file: %1").arg(error));
    } else {
      doc->setName(path);
      setActiveDocument(doc);
    }
  }
}

void MainWindow::importFile()
{
  closeFile();

  QString path = QFileDialog::getOpenFileName(
      this,
      tr("Choose an image file to load"),
      QString(),
      tr("Image Files (*.png *.jpg *.bmp)"));

  if (!path.isEmpty()) {
    QImage image(path);

    if (image.isNull()) {
      QMessageBox::critical(this, tr("Error"), tr("Error loading image file."));
      return;
    }

    ImportDialog diag(image, this);
    diag.show();
    diag.exec();

    if (diag.result() != QDialog::Accepted)
      return;

    const QColor *transparentColor = NULL;
    if (diag.hasTransparent())
      transparentColor = &diag.transparentColor();

    Document *doc = DocumentFactory::load(image, 
					  diag.colorManager(),
					  diag.documentWidth(),
					  transparentColor);
    if (!doc) {
      QMessageBox::critical(this, tr("Error"), tr("Error loading file."));
    } else {
      doc->setTitle(diag.title());
      doc->setAuthor(diag.author());
      setActiveDocument(doc);
    }
  }
}

void MainWindow::closeFile()
{
  if (!confirmClose())
    return;

  if (state_->activeDocument()) {
    Document *doc = state_->activeDocument();
    setActiveDocument(NULL);
    delete doc;
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

void MainWindow::about()
{
  QMessageBox::about(this, tr("About Stitchy"),
		     tr("<qt><h2>Stitchy - A free cross-stitch pattern designer for Windows, Mac OS X and Unix.</h2>Version %1<br>Copyright (c) 2012 Park Joon-Kyu &lt;segfault87@gmail.com&gt;<br><br>This software is a free software. Visit <a href=\"https://github.com/segfault87/stitchy\">GitHub page</a> to find out more.</qt>").arg(VERSION));
}

void MainWindow::viewModeAction(QAction *action)
{
  RenderingMode rm;
  
  if (action == actionViewSimplified_)
    rm = RenderingMode_Simple;
  else if (action == actionViewFull_)
    rm = RenderingMode_Full;
  else if (action == actionViewSymbols_)
    rm = RenderingMode_Symbol;
  else
    return;

  state_->setRenderingMode(rm);
  canvas_->scale(2.0, 2.0);
  canvas_->scale(0.5, 0.5);
}

void MainWindow::showColorEditor()
{
  QDialog *dialog = new ColorEditor(GlobalState::self()->colorManager(), this);
  dialog->show();
}

void MainWindow::toolModeAction(QAction *action)
{
  ToolMode t;

  if (state_->toolMode() == ToolMode_Move && action != actionModeMove_)
    canvas_->commitPaste();

  if (action == actionModeSelect_)
    t = ToolMode_Select;
  else if (action == actionModeMove_)
    t = ToolMode_Move;
  else if (action == actionModeErase_)
    t = ToolMode_Erase;
  else if (action == actionModeRectangle_)
    t = ToolMode_Rectangle;
  else if (action == actionModeDrawFull_)
    t = ToolMode_Full;
  else if (action == actionModeDrawHalf_)
    t = ToolMode_Half;
  else if (action == actionModeDrawPetite_)
    t = ToolMode_Petite;
  else if (action == actionModeDrawQuarter_)
    t = ToolMode_Quarter;
  else
    return;

  state_->setToolMode(t);
}

void MainWindow::updateTitle()
{
  Document *d = state_->activeDocument();

  if (!d) {
    setWindowTitle(tr("Stitchy"));
  } else {
    QString title;
    QString edited;

    if (d->changed())
      edited = tr(" [Modified]");
    
    if (d->title().isEmpty())
      title = tr("Untitled document");
    else
      title = d->title();
    
    if (d->name().isEmpty()) {
      setWindowTitle(tr("%1 - Stitchy%2").arg(title).arg(edited));
    } else {
      QFileInfo path(d->name());
      setWindowTitle(tr("%1 - %2 - Stitchy%3").arg(path.fileName())
                     .arg(title).arg(edited));
    }
  }
}

void MainWindow::setActiveDocument(Document *document)
{
  if (state_->activeDocument())
    disconnectConnections(state_->activeDocument());

  state_->setActiveDocument(document);

  if (state_->activeDocument()) {
    setEnabled(documentActions_, true);
    state_->undoGroup()->setActiveStack(document->editor());
    canvas_->setDocument(state_->activeDocument());
    initConnections(document);
  } else {
    setEnabled(documentActions_, false);
    state_->undoGroup()->setActiveStack(NULL);
    canvas_->setDocument(NULL);
  }

  emit needsUpdate();
  emit documentChanged(document);
}

void MainWindow::documentChangeAction(Document *document)
{
  Q_UNUSED(document);

  updateTitle();
}

void MainWindow::selectionChanged(const QRect &rect)
{
  actionModeMove_->setChecked(true);
  toolModeAction(actionModeMove_);
  setEnabled(selectionActions_, true);
}

void MainWindow::selectionCleared()
{
  actionModeSelect_->setChecked(true);
  toolModeAction(actionModeSelect_);
  setEnabled(selectionActions_, false);
}

void MainWindow::clipboardChanged()
{
  const QMimeData *data = clipboard_->mimeData();

  if (state_->activeDocument() && data &&
      data->hasFormat(SelectionGroup::mimeType()))
    actionPaste_->setEnabled(true);
  else
    actionPaste_->setEnabled(false);
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

QAction* MainWindow::createAction(const QString &name, const QKeySequence &shortcut,
                                  const QIcon &icon)
{
  QAction *a = createAction(name, NULL, NULL, shortcut, icon);
  a->setCheckable(true);
  return a;
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
          .arg(activeDocument->title().isEmpty() ? "Untitled" : activeDocument->title()),
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
    filename = QFileDialog::getSaveFileName(
        this, tr("Save as"), QString(),
        tr("Stitchy Document (*.stitchy)"));
    if (filename.isEmpty())
      return false;
  } else {
    filename = activeDocument->name();
  }

  QString error;
  if (!DocumentFactory::save(activeDocument, filename, error)) {
    QMessageBox::critical(this, tr("Error"), tr("Error saving file: %1").arg(error));
    return false;
  }

  activeDocument->setName(filename);
  activeDocument->setChanged(false);

  return true;
}

void MainWindow::setEnabled(QList<QAction *> &actions, bool enabled)
{
  foreach (QAction *a, actions) {
    a->setEnabled(enabled);
  }
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
  actionImportFile_ = createAction(tr("&Import Image..."),
				   this,
				   SLOT(importFile()),
				   QKeySequence(),
				   Utils::icon("document-import"));
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

  actionCut_ = createAction(tr("&Cut"),
                            canvas_,
                            SLOT(cut()),
                            QKeySequence::Cut,
                            Utils::icon("edit-cut"));
  actionCopy_ = createAction(tr("C&opy"),
                             canvas_,
                             SLOT(copy()),
                             QKeySequence::Copy,
                             Utils::icon("edit-copy"));
  actionPaste_ = createAction(tr("&Paste"),
                              canvas_,
                              SLOT(paste()),
                              QKeySequence::Paste,
                              Utils::icon("edit-paste"));
  clipboardChanged();
  actionDeleteSelected_ = createAction(tr("&Delete Selected"),
                                       canvas_,
                                       SLOT(deleteSelected()),
                                       QKeySequence("Delete"),
                                       Utils::icon("edit-delete"));
                                       
  
  actionZoomIn_ = createAction(tr("Zoom &In"),
                               canvas_,
                               SLOT(zoomIn()),
                               QKeySequence("Ctrl++"),
                               Utils::icon("zoom-in"));
  actionZoomOut_ = createAction(tr("Zoom &Out"),
                                canvas_,
                                SLOT(zoomOut()),
                                QKeySequence("Ctrl+-"),
                                Utils::icon("zoom-out"));
  actionZoomReset_ = createAction(tr("&Reset Zoom"),
                                  canvas_,
                                  SLOT(zoomReset()),
                                  QKeySequence("Ctrl+0"),
                                  Utils::icon("zoom-original"));
  actionViewGrids_ = createAction(tr("Show &Grids"),
                                  QKeySequence(),
                                  Utils::icon("view-grid"));
  actionViewGrids_->setChecked(true);

  actionViewFull_ = createAction(tr("&Detailed View"),
                                 QKeySequence(),
                                 Utils::icon("view-stitch-full"));
  actionViewSimplified_ = createAction(tr("&Color View"),
                                       QKeySequence(),
                                       Utils::icon("view-stitch-color"));
  actionViewSymbols_ = createAction(tr("&Code View"),
                                   QKeySequence(),
                                   Utils::icon("view-stitch-symbols"));
  actionViewFull_->setChecked(true);

  actionViewMode_ = new QActionGroup(this);
  actionViewMode_->addAction(actionViewFull_);
  actionViewMode_->addAction(actionViewSimplified_);
  actionViewMode_->addAction(actionViewSymbols_);
  actionViewMode_->setExclusive(true);

  actionModeSelect_ = createAction(tr("&Select"),
                                   QKeySequence("F3"),
                                   Utils::icon("select-rectangular"));
  actionModeMove_ = createAction(tr("&Move"),
                                 QKeySequence("F4"),
                                 Utils::icon("transform-move"));
  actionModeErase_ = createAction(tr("&Erase"),
                                  QKeySequence("F5"),
                                  Utils::icon("draw-eraser"));
  actionModeRectangle_ = createAction(tr("&Rectangle"),
                                      QKeySequence("F6"),
                                      Utils::icon("draw-rectangle"));
  actionModeDrawFull_ = createAction(tr("&Full Stitch"),
                                     QKeySequence("F7"),
                                     Utils::icon("stitch-full"));
  actionModeDrawHalf_ = createAction(tr("&Half Stitch"),
                                     QKeySequence("F8"),
                                     Utils::icon("stitch-half"));
  actionModeDrawPetite_ = createAction(tr("&Petite Stitch"),
                                       QKeySequence("F9"),
                                       Utils::icon("stitch-petite"));
  actionModeDrawQuarter_ = createAction(tr("&Quarter Stitch"),
                                        QKeySequence("F10"),
                                        Utils::icon("stitch-quarter"));
  actionModeDrawFull_->setChecked(true);

  actionGroupMode_ = new QActionGroup(this);
  actionGroupMode_->addAction(actionModeSelect_);
  actionGroupMode_->addAction(actionModeMove_);
  actionGroupMode_->addAction(actionModeErase_);
  actionGroupMode_->addAction(actionModeRectangle_);
  actionGroupMode_->addAction(actionModeDrawFull_);
  actionGroupMode_->addAction(actionModeDrawHalf_);
  actionGroupMode_->addAction(actionModeDrawPetite_);
  actionGroupMode_->addAction(actionModeDrawQuarter_);
  actionGroupMode_->setExclusive(true);
  
  actionColorEditor_ = createAction(tr("&Colors..."),
                                    this,
                                    SLOT(showColorEditor()),
                                    QKeySequence("F12"),
                                    QIcon());

  actionAbout_ = createAction(tr("&About..."),
			      this,
			      SLOT(about()),
			      QKeySequence(),
			      QIcon());
  actionAboutQt_ = createAction(tr("About &Qt..."),
				qApp,
				SLOT(aboutQt()),
				QKeySequence(),
				QIcon());

  documentActions_ << actionCloseFile_ << actionSaveFile_ <<
      actionSaveFileAs_ << actionZoomIn_ << actionZoomOut_ <<
      actionZoomReset_;
  selectionActions_ << actionCut_ << actionCopy_ <<
      actionDeleteSelected_;
}

void MainWindow::initMenus()
{
  menuFile_ = menuBar()->addMenu(tr("&File"));
  menuFile_->addAction(actionNewFile_);
  menuFile_->addAction(actionOpenFile_);
  menuFile_->addAction(actionImportFile_);
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
  menuEdit_->addSeparator();
  menuEdit_->addAction(actionCut_);
  menuEdit_->addAction(actionCopy_);
  menuEdit_->addAction(actionPaste_);
  menuEdit_->addSeparator();;
  menuEdit_->addAction(actionDeleteSelected_);

  menuView_ = menuBar()->addMenu(tr("&View"));
  menuView_->addAction(actionZoomOut_);
  menuView_->addAction(actionZoomIn_);
  menuView_->addAction(actionZoomReset_);
  menuView_->addSeparator();
  menuView_->addAction(actionViewFull_);
  menuView_->addAction(actionViewSimplified_);
  menuView_->addAction(actionViewSymbols_);
  menuView_->addSeparator();
  menuView_->addAction(actionViewGrids_);

  menuTool_ = menuBar()->addMenu(tr("&Tools"));
  menuTool_->addAction(actionModeSelect_);
  menuTool_->addAction(actionModeMove_);
  menuTool_->addAction(actionModeErase_);
  menuTool_->addAction(actionModeRectangle_);
  menuTool_->addAction(actionModeDrawFull_);
  menuTool_->addAction(actionModeDrawHalf_);
  menuTool_->addAction(actionModeDrawPetite_);
  menuTool_->addAction(actionModeDrawQuarter_);

  menuWindow_ = menuBar()->addMenu(tr("&Window"));
  menuWindow_->addAction(actionColorEditor_);

  menuHelp_ = menuBar()->addMenu(tr("&Help"));
  menuHelp_->addAction(actionAbout_);
  menuHelp_->addAction(actionAboutQt_);
}

void MainWindow::initToolbars()
{
  QToolBar *toolBarFile = addToolBar(tr("File"));
  toolBarFile->setObjectName("toolbar_file");
  toolBarFile->addAction(actionNewFile_);
  toolBarFile->addSeparator();
  toolBarFile->addAction(actionOpenFile_);
  toolBarFile->addSeparator();
  toolBarFile->addAction(actionSaveFile_);
  toolBarFile->addAction(actionSaveFileAs_);
  toolBarFile->addSeparator();
  toolBarFile->addAction(actionCloseFile_);

  QToolBar *toolBarEdit = addToolBar(tr("Edit"));
  toolBarEdit->setObjectName("toolbar_edit");
  toolBarEdit->addAction(actionUndo_);
  toolBarEdit->addAction(actionRedo_);
  toolBarEdit->addSeparator();
  toolBarEdit->addAction(actionCut_);
  toolBarEdit->addAction(actionCopy_);
  toolBarEdit->addAction(actionPaste_);
  toolBarEdit->addSeparator();
  toolBarEdit->addAction(actionDeleteSelected_);

  QToolBar *toolBarView = addToolBar(tr("View"));
  toolBarView->setObjectName("toolbar_view");
  toolBarView->addAction(actionZoomOut_);
  toolBarView->addAction(actionZoomIn_);
  toolBarView->addAction(actionZoomReset_);
  toolBarView->addSeparator();
  toolBarView->addAction(actionViewFull_);
  toolBarView->addAction(actionViewSimplified_);
  toolBarView->addAction(actionViewSymbols_);

  QToolBar *toolBarTool = addToolBar(tr("Tools"));
  toolBarTool->setObjectName("toolbar_tool");
  toolBarTool->addAction(actionModeSelect_);
  toolBarTool->addAction(actionModeMove_);
  toolBarTool->addSeparator();
  toolBarTool->addAction(actionModeErase_);
  toolBarTool->addAction(actionModeRectangle_);
  toolBarTool->addAction(actionModeDrawFull_);
  toolBarTool->addAction(actionModeDrawHalf_);
  toolBarTool->addAction(actionModeDrawPetite_);
  toolBarTool->addAction(actionModeDrawQuarter_);
}

void MainWindow::initWidgets()
{
  palette_ = new PaletteWidget(GlobalState::self()->colorManager());
  QDockWidget *paletteDock = new QDockWidget(tr("Color Swatches"));
  paletteDock->setObjectName("palette");
  paletteDock->setWidget(palette_);
  paletteDock->setFeatures(QDockWidget::DockWidgetMovable |
                           QDockWidget::DockWidgetFloatable);
  addDockWidget(Qt::LeftDockWidgetArea, paletteDock);

  canvas_ = new Canvas(this);
  setCentralWidget(canvas_);
}

void MainWindow::initConnections()
{
  connect(this, SIGNAL(needsUpdate()),
          canvas_, SLOT(update()));
  connect(palette_, SIGNAL(colorSelected(const Color *)),
          state_, SLOT(setColor(const Color *)));
  connect(palette_, SIGNAL(userColorSetIsEmpty()),
          this, SLOT(showColorEditor()));
  connect(this, SIGNAL(documentChanged(Document *)),
          palette_, SLOT(documentChanged(Document *)));
  connect(this, SIGNAL(documentChanged(Document *)),
          this, SLOT(documentChangeAction(Document*)));
  connect(actionGroupMode_, SIGNAL(triggered(QAction *)),
          this, SLOT(toolModeAction(QAction *)));
  connect(actionViewMode_, SIGNAL(triggered(QAction *)),
          this, SLOT(viewModeAction(QAction *)));
  connect(actionViewGrids_, SIGNAL(toggled(bool)),
          canvas_, SLOT(toggleGrid(bool)));
  connect(canvas_, SIGNAL(madeSelection(const QRect &)),
          this, SLOT(selectionChanged(const QRect &)));
  connect(canvas_, SIGNAL(clearedSelection()),
          this, SLOT(selectionCleared()));
  connect(clipboard_, SIGNAL(dataChanged()),
          this, SLOT(clipboardChanged()));
}

void MainWindow::initConnections(Document *doc)
{
  connect(doc, SIGNAL(documentChanged()), this, SLOT(updateTitle()));
  connect(doc, SIGNAL(documentSaved()), this, SLOT(updateTitle()));
  connect(doc, SIGNAL(madeSelection(const QRect &)),
	  canvas_, SIGNAL(madeSelection(const QRect &)));
}

void MainWindow::disconnectConnections(Document *doc)
{
  disconnect(doc, SIGNAL(documentChanged()));
  disconnect(doc, SIGNAL(documentSaved()));
  disconnect(doc, SIGNAL(madeSelection(const QRect &)));
}
