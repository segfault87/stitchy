#include "colormanager.h"
#include "palettemodel.h"
#include "settings.h"
#include "utils.h"

#include "coloreditor.h"

ColorEditor::ColorEditor(MetaColorManager *cm, QWidget *parent)
    : QDialog(parent), Ui::ColorEditor(), colorManager_(cm)
{
  setAttribute(Qt::WA_DeleteOnClose);

  myColors_ = cm->localSwatches();
  myModel_ = new PaletteModel(this);
  foreignModel_ = new PaletteModel(this);

  myModel_->setColorManager(myColors_);

  setupUi(this);

  buttonAdd->setIcon(Utils::icon("arrow-right"));
  buttonRemove->setIcon(Utils::icon("edit-delete"));
  buttonLower->setIcon(Utils::icon("arrow-down"));
  buttonUpper->setIcon(Utils::icon("arrow-up"));

  swatchesView->setModel(foreignModel_);
  mySwatchesView->setModel(myModel_);

  QString initialCategory = Settings::self()->defaultPalette();
  QList<ColorManager *> &list = cm->colorManagers();
  int idx = 0;
  foreach (ColorManager *cm, list) {
    colorSet->addItem(cm->name(), cm->id());
    if (!initialCategory.isEmpty() && cm->id() == initialCategory) {
      selectColorSet(idx);
      break;
    }
    ++idx;
  }
  if (initialCategory.isEmpty() && list.size() > 0)
    selectColorSet(0);

  connect(colorSet, SIGNAL(activated(int)),
          this, SLOT(selectColorSet(int)));
  connect(buttonAdd, SIGNAL(clicked()),
          this, SLOT(addColor()));
  connect(buttonRemove, SIGNAL(clicked()),
          this, SLOT(removeColor()));
  connect(buttonUpper, SIGNAL(clicked()),
          this, SLOT(moveUp()));
  connect(buttonLower, SIGNAL(clicked()),
          this, SLOT(moveDown()));
  connect(swatchesView->selectionModel(),
          SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
          this,
          SLOT(leftPaneSelected(const QItemSelection &, const QItemSelection &)));
  connect(mySwatchesView->selectionModel(),
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
  const QItemSelection &selection = swatchesView->selectionModel()->selection();
  const QItemSelection &rs = mySwatchesView->selectionModel()->selection();

  if (selection.indexes().size() == 0)
    return;

  const Color *c = foreignColors_->itemAt(selection.indexes()[0].row());

  if (rs.indexes().size() == 0) {
    myColors_->add(c);
  } else {
    int row = rs.indexes()[0].row();
    if (row == myColors_->count() - 1)
      myColors_->add(c);
    else
      myColors_->insert(c, row + 1);
  }
}

void ColorEditor::removeColor()
{
  const QItemSelection &rs = mySwatchesView->selectionModel()->selection();
  if (rs.indexes().size() == 0)
    return;
  int row = rs.indexes()[0].row();
  
  myColors_->remove(myColors_->itemAt(row)->id());
  mySwatchesView->clearSelection();
}

void ColorEditor::moveUp()
{
  const QItemSelection &rs = mySwatchesView->selectionModel()->selection();
  if (rs.indexes().size() == 0)
    return;
  int row = rs.indexes()[0].row();
  if (row == 0)
    return;

  myColors_->swap(row, row - 1);

  mySwatchesView->selectionModel()->select(
      QItemSelection(
          myModel_->index(row-1, 0),
          myModel_->index(row-1, myModel_->columnCount() - 1)),
      QItemSelectionModel::ClearAndSelect);
}

void ColorEditor::moveDown()
{
  const QItemSelection &rs = mySwatchesView->selectionModel()->selection();
  if (rs.indexes().size() == 0)
    return;
  int row = rs.indexes()[0].row();
  if (row == myColors_->count() - 1)
    return;

  myColors_->swap(row, row + 1);

  mySwatchesView->selectionModel()->select(
      QItemSelection(
          myModel_->index(row+1, 0),
          myModel_->index(row+1, myModel_->columnCount() - 1)),
      QItemSelectionModel::ClearAndSelect);
}

void ColorEditor::selectColorSet(int index)
{
  if (index == -1)
    return;

  QVariant data = colorSet->itemData(index);
  if (data.type() == QVariant::String) {
    foreignColors_ = colorManager_->colorManager(data.toString());
    foreignModel_->setColorManager(foreignColors_);
    leftPaneDeselected();
  }
}

void ColorEditor::leftPaneDeselected()
{
  buttonAdd->setEnabled(false);
}

void ColorEditor::rightPaneDeselected()
{
  buttonRemove->setEnabled(false);
  buttonUpper->setEnabled(false);
  buttonLower->setEnabled(false);
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

  buttonAdd->setEnabled(enable);
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
    buttonRemove->setEnabled(remove);
    
    bool upper = true;
    if (row == 0)
      upper = false;
    buttonUpper->setEnabled(upper);
    
    bool lower = true;
    if (row == myColors_->count() -1)
      lower = false;
    buttonLower->setEnabled(lower);
  } else {
    rightPaneDeselected();
  }
}

