#include <stdio.h>

#include "color.h"
#include "colormanager.h"

#include "palettemodel.h"

#define COLUMN_ID 0
#define COLUMN_NAME 1

PaletteModel::PaletteModel(QObject *parent)
    : QAbstractItemModel(parent)
{
  list_ = NULL;
}

PaletteModel::~PaletteModel()
{

}

void PaletteModel::setColorManager(ColorManager *cm)
{
  list_ = &cm->colorList();

  disconnect(this, SLOT(colorAppended()));
  disconnect(this, SLOT(colorInserted(int)));
  disconnect(this, SLOT(colorDeleted(int)));
  disconnect(this, SLOT(colorSwapped(int, int)));

  connect(cm, SIGNAL(colorAppended()),
          this, SLOT(colorAppended()));
  connect(cm, SIGNAL(colorInserted(int)),
          this, SLOT(colorInserted(int)));
  connect(cm, SIGNAL(colorDeleted(int)), this,
          SLOT(colorDeleted(int)));
  connect(cm, SIGNAL(colorSwapped(int, int)), this,
          SLOT(colorSwapped(int, int)));

  reset();
}

int PaletteModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);

  return 2;
}

int PaletteModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;

  if (!list_)
    return 0;

  return list_->size();
}

QModelIndex PaletteModel::parent(const QModelIndex &index) const
{
  Q_UNUSED(index);

  return QModelIndex();
}

QModelIndex PaletteModel::index(int row, int column,
                                const QModelIndex &parent) const
{
  /* there are no children */
  if (parent.isValid())
    return QModelIndex();

  return createIndex(row, column);
}

QVariant PaletteModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const
{
  Q_UNUSED(orientation);

  if (role == Qt::DisplayRole) {
    if (section == 0)
      return tr("Color No.");
    else if (section == 1)
      return tr("Name");
  }

  return QVariant();
}

QVariant PaletteModel::data(const QModelIndex &index, int role) const
{
  /* there are no children */
  if (index.parent().isValid())
    return QVariant();

  if (!list_)
    return QVariant();

  const Color *c = (*list_)[index.row()];

  if (!c)
    return QVariant();

  if (role == Qt::DisplayRole) {
    if (index.column() == COLUMN_ID)
      return c->id();
    else if (index.column() == COLUMN_NAME)
      return c->name();
  } else if (role == Qt::TextAlignmentRole) {
    if (index.column() == COLUMN_ID)
      return Qt::AlignHCenter;
  } else if (role == Qt::DecorationRole) {
    if (index.column() == COLUMN_NAME)
      return c->color();
  }

  return QVariant();
}

void PaletteModel::resetModel()
{
  reset();
}

void PaletteModel::colorAppended()
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  endInsertRows();
}

void PaletteModel::colorInserted(int before)
{
  beginInsertRows(QModelIndex(), before, before);
  endInsertRows();
}

void PaletteModel::colorDeleted(int index)
{
  beginRemoveRows(QModelIndex(), index, index);
  endRemoveRows();
}

void PaletteModel::colorSwapped(int index1, int index2)
{
  emit dataChanged(index(index1, 0), index(index1, columnCount()));
  emit dataChanged(index(index2, 0), index(index2, columnCount()));
}
