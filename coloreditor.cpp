#include "colormanager.h"
#include "palettemodel.h"
#include "settings.h"
#include "utils.h"

#include "coloreditor.h"

ColorEditor::ColorEditor(MetaColorManager *cm, QWidget *parent)
    : QDialog(parent), colorManager_(cm)
{
  setAttribute(Qt::WA_DeleteOnClose);

  myColors_ = cm->localSwatches();
  myModel_ = new PaletteModel(this);
  foreignModel_ = new PaletteModel(this);

  myModel_->setColorManager(myColors_);

  ui_.setupUi(this);

  ui_.buttonAdd->setIcon(Utils::icon("arrow-right"));
  ui_.buttonRemove->setIcon(Utils::icon("edit-delete"));
  ui_.buttonLower->setIcon(Utils::icon("arrow-down"));
  ui_.buttonUpper->setIcon(Utils::icon("arrow-up"));

  ui_.swatchesView->setModel(foreignModel_);
  ui_.mySwatchesView->setModel(myModel_);

  QString initialCategory = Settings::self()->defaultPalette();
  QList<ColorManager *> &list = cm->colorManagers();
  int idx = 0;
  foreach (ColorManager *cm, list) {
    ui_.colorSet->addItem(cm->name(), cm->id());
    if (!initialCategory.isEmpty() && cm->id() == initialCategory) {
      selectColorSet(idx);
      break;
    }
    ++idx;
  }
  if (initialCategory.isEmpty() && list.size() > 0)
    selectColorSet(0);

  connect(ui_.colorSet, SIGNAL(activated(int)),
          this, SLOT(selectColorSet(int)));
  connect(ui_.buttonAdd, SIGNAL(clicked()),
          this, SLOT(addColor()));
  connect(ui_.buttonRemove, SIGNAL(clicked()),
          this, SLOT(removeColor()));
  connect(ui_.buttonUpper, SIGNAL(clicked()),
          this, SLOT(moveUp()));
  connect(ui_.buttonLower, SIGNAL(clicked()),
          this, SLOT(moveDown()));
  connect(ui_.swatchesView->selectionModel(),
          SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
          this,
          SLOT(leftPaneSelected(const QItemSelection &, const QItemSelection &)));
  connect(ui_.mySwatchesView->selectionModel(),
          SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
          this,
          SLOT(rightPaneSelected(const QItemSelection &, const QItemSelection &)));
}

ColorEditor::~ColorEditor()
{
  commit();
}

void ColorEditor::commit()
{
  QStringList sl;

  const QVector<const Color *> &cl = myColors_->colorList();
  for (QVector<const Color *>::ConstIterator it = cl.begin();
       it != cl.end();
       ++it) {
    sl << (*it)->parent()->id() + "|" + (*it)->id();
  }

  Settings::self()->setMyColors(sl);
}

void ColorEditor::addColor()
{
  const QItemSelection &selection = ui_.swatchesView->selectionModel()->selection();
  const QItemSelection &rs = ui_.mySwatchesView->selectionModel()->selection();

  if (selection.indexes().size() == 0)
    return;

  const Color *c = foreignColors_->itemAt(selection.indexes()[0].row());

  if (rs.indexes().size() == 0) {
    myColors_->add(Color(*c));
  } else {
    int row = rs.indexes()[0].row();
    if (row == myColors_->count() - 1)
      myColors_->add(Color(*c));
    else
      myColors_->insert(Color(*c), row + 1);
  }
}

void ColorEditor::removeColor()
{
  const QItemSelection &rs = ui_.mySwatchesView->selectionModel()->selection();
  if (rs.indexes().size() == 0)
    return;
  int row = rs.indexes()[0].row();
  
  myColors_->remove(myColors_->itemAt(row)->id());
  ui_.mySwatchesView->clearSelection();
}

void ColorEditor::moveUp()
{
  const QItemSelection &rs = ui_.mySwatchesView->selectionModel()->selection();
  if (rs.indexes().size() == 0)
    return;
  int row = rs.indexes()[0].row();
  if (row == 0)
    return;

  myColors_->swap(row, row - 1);

  ui_.mySwatchesView->selectionModel()->select(
      QItemSelection(
          myModel_->index(row-1, 0),
          myModel_->index(row-1, myModel_->columnCount() - 1)),
      QItemSelectionModel::ClearAndSelect);
}

void ColorEditor::moveDown()
{
  const QItemSelection &rs = ui_.mySwatchesView->selectionModel()->selection();
  if (rs.indexes().size() == 0)
    return;
  int row = rs.indexes()[0].row();
  if (row == myColors_->count() - 1)
    return;

  myColors_->swap(row, row + 1);

  ui_.mySwatchesView->selectionModel()->select(
      QItemSelection(
          myModel_->index(row+1, 0),
          myModel_->index(row+1, myModel_->columnCount() - 1)),
      QItemSelectionModel::ClearAndSelect);
}

void ColorEditor::selectColorSet(int index)
{
  if (index == -1)
    return;

  QVariant data = ui_.colorSet->itemData(index);
  if (data.type() == QVariant::String) {
    foreignColors_ = colorManager_->colorManager(data.toString());
    foreignModel_->setColorManager(foreignColors_);
    leftPaneDeselected();
  }
}

void ColorEditor::leftPaneDeselected()
{
  ui_.buttonAdd->setEnabled(false);
}

void ColorEditor::rightPaneDeselected()
{
  ui_.buttonRemove->setEnabled(false);
  ui_.buttonUpper->setEnabled(false);
  ui_.buttonLower->setEnabled(false);
}

void ColorEditor::leftPaneSelected(const QItemSelection &selected,
                                   const QItemSelection &deselected)
{
  Q_UNUSED(deselected);
  
  int row = selected.indexes()[0].row();
  const Color *c = foreignColors_->itemAt(row);
  bool enable = false;

  if (c) {
    if (!myColors_->get(c->id()))
      enable = true;
  }

  ui_.buttonAdd->setEnabled(enable);
}

void ColorEditor::rightPaneSelected(const QItemSelection &selected,
                                    const QItemSelection &deselected)
{
  Q_UNUSED(deselected);
  
  if (selected.indexes().size() > 0) {
    int row = selected.indexes()[0].row();
    
    bool remove = false;
    if (myColors_->itemAt(row))
      remove = true;
    ui_.buttonRemove->setEnabled(remove);
    
    bool upper = true;
    if (row == 0)
      upper = false;
    ui_.buttonUpper->setEnabled(upper);
    
    bool lower = true;
    if (row == myColors_->count() -1)
      lower = false;
    ui_.buttonLower->setEnabled(lower);
  } else {
    rightPaneDeselected();
  }
}

