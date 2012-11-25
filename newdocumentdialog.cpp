#include "newdocumentdialog.h"

NewDocumentDialog::NewDocumentDialog(QWidget *parent)
    : QDialog(parent), Ui::NewDocumentDialog()
{
  setupUi(this);
}

NewDocumentDialog::~NewDocumentDialog()
{

}

QSize NewDocumentDialog::documentSize() const
{
  return QSize(columns->value(), rows->value());
}

QString NewDocumentDialog::title() const
{
  return Ui::NewDocumentDialog::title->text();
}

QString NewDocumentDialog::author() const
{
  return Ui::NewDocumentDialog::author->text();
}

