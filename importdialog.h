#ifndef _IMPORTDIALOG_H_
#define _IMPORTDIALOG_H_

#include <QDialog>
#include <QImage>

#include "ui_importdialog.h"

class ColorManager;

class ImportDialog : public QDialog, public Ui::ImportDialog
{
  Q_OBJECT;

 public:
  ImportDialog(const QImage &image, QWidget *parent = NULL);
  ~ImportDialog();

  int documentWidth() const;
  QString title() const;
  QString author() const;
  ColorManager* colorManager() const;
  bool hasTransparent() const;
  const QColor& transparentColor() const;

 public slots:
  void setWidth(int v);
  void setHeight(int v);

 private slots:
  void selectColor();

 private:
  QSize origsize_;
  QColor transparentColor_;
};

#endif
