#include "document.h"

#include "documentpropertiesdialog.h"

DocumentPropertiesDialog::DocumentPropertiesDialog(Document *doc,
						   QWidget *parent)
    : QDialog(parent), Ui::DocumentPropertiesDialog()
{
  setupUi(this);

  columns->setValue(doc->size().width());
  rows->setValue(doc->size().height());
  Ui::DocumentPropertiesDialog::title->setText(doc->title());
  Ui::DocumentPropertiesDialog::author->setText(doc->author());
}

DocumentPropertiesDialog::~DocumentPropertiesDialog()
{

}

QSize DocumentPropertiesDialog::documentSize() const
{
  return QSize(columns->value(), rows->value());
}

QString DocumentPropertiesDialog::title() const
{
  return Ui::DocumentPropertiesDialog::title->text();
}

QString DocumentPropertiesDialog::author() const
{
  return Ui::DocumentPropertiesDialog::author->text();
}

