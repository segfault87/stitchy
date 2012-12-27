#ifndef _DOCUMENTPROPERTIESDIALOG_H_
#define _DOCUMENTPROPERTIESDIALOG_H_

#include <QDialog>
#include <QSize>

#include "ui_documentpropertiesdialog.h"

class Document;

class DocumentPropertiesDialog : public QDialog, public Ui::DocumentPropertiesDialog
{
 public:
  DocumentPropertiesDialog(Document *doc, QWidget *parent = NULL);
  ~DocumentPropertiesDialog();

  QSize documentSize() const;
  QString title() const;
  QString author() const;
};

#endif
