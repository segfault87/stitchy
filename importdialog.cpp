#include <QColorDialog>

#include "colormanager.h"
#include "globalstate.h"

#include "importdialog.h"

ImportDialog::ImportDialog(const QImage &image,
			   QWidget *parent)
    : QDialog(parent), Ui::ImportDialog()
{
  setupUi(this);

  connect(Ui::ImportDialog::width, SIGNAL(valueChanged(int)),
	  this, SLOT(setWidth(int)));
  connect(Ui::ImportDialog::height, SIGNAL(valueChanged(int)),
	  this, SLOT(setHeight(int)));
  connect(Ui::ImportDialog::transparentColor, SIGNAL(released()),
	  this, SLOT(selectColor()));

  transparentColor_ = QColor(255, 255, 255);
  origsize_ = image.size();

  if (origsize_.height() > origsize_.width())
    setHeight(60);
  else
    setWidth(60);

  MetaColorManager *mcm = GlobalState::self()->colorManager();
  foreach (ColorManager *cm, mcm->colorManagers())
    colorSet->addItem(cm->name(), cm->id());
  if (mcm->localSwatches()->count() > 0) {
    colorSet->insertSeparator(colorSet->count());
    colorSet->addItem(mcm->localSwatches()->name(), "");
  }
}

ImportDialog::~ImportDialog()
{

}

int ImportDialog::documentWidth() const
{
  return Ui::ImportDialog::width->value();
}

QString ImportDialog::title() const
{
  return Ui::ImportDialog::title->text();
}

QString ImportDialog::author() const
{
  return Ui::ImportDialog::author->text();
}

ColorManager* ImportDialog::colorManager() const
{
  if (colorSet->currentIndex() < 0)
    return NULL;

  MetaColorManager *mcm = GlobalState::self()->colorManager();
  QString id = colorSet->itemData(colorSet->currentIndex()).toString();
  if (id == "")
    return mcm->localSwatches();
  else
    return mcm->colorManager(id);
}

bool ImportDialog::hasTransparent() const
{
  return transparent->isChecked();
}

const QColor& ImportDialog::transparentColor() const
{
  return transparentColor_;
}

void ImportDialog::setWidth(int v)
{
  int adjustedh = origsize_.height() / (origsize_.width() / (float) v);
  if (adjustedh > 1000) {
    setHeight(1000);
    return;
  }

  Ui::ImportDialog::width->blockSignals(true);
  Ui::ImportDialog::height->blockSignals(true);

  Ui::ImportDialog::width->setValue(v);
  Ui::ImportDialog::height->setValue(adjustedh);

  Ui::ImportDialog::width->blockSignals(false);
  Ui::ImportDialog::height->blockSignals(false);
}

void ImportDialog::setHeight(int v)
{
  int adjustedw = origsize_.width() / (origsize_.height() / (float) v);
  if (adjustedw > 1000) {
    setWidth(1000);
    return;
  }

  Ui::ImportDialog::width->blockSignals(true);
  Ui::ImportDialog::height->blockSignals(true);

  Ui::ImportDialog::width->setValue(adjustedw);
  Ui::ImportDialog::height->setValue(v);

  Ui::ImportDialog::width->blockSignals(false);
  Ui::ImportDialog::height->blockSignals(false);
}

void ImportDialog::selectColor()
{
  QColorDialog cd(transparentColor_, this);

  cd.exec();
  if (cd.result() != QDialog::Accepted)
    return;

  transparentColor_ = cd.selectedColor();
  Ui::ImportDialog::transparentColor->setText(transparentColor_.name());
}
