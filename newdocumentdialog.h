#ifndef _NEWDOCUMENTDIALOG_H_
#define _NEWDOCUMENTDIALOG_H_

#include <QDialog>
#include <QSize>

#include "ui_newdocumentdialog.h"

class NewDocumentDialog : public QDialog, public Ui::NewDocumentDialog
{
 public:
  NewDocumentDialog(QWidget *parent = NULL);
  ~NewDocumentDialog();

  QSize documentSize() const;
};

#endif
