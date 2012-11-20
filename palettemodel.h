#ifndef _PALETTEMODEL_H_
#define _PALETTEMODEL_H_

#include <QAbstractItemModel>
#include <QVector>

class Color;
class ColorManager;

class PaletteModel : public QAbstractItemModel
{
  Q_OBJECT;

 public:
  PaletteModel(QObject *parent = NULL);
  ~PaletteModel();

  void setColorManager(ColorManager *cm);

  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  
  QModelIndex parent(const QModelIndex &index) const;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  QVariant data(const QModelIndex &index, int role) const;

 public slots:
  void resetModel();
  
 private:
  const QVector<const Color *> *list_;
};

#endif
