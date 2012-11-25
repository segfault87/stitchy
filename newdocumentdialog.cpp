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
