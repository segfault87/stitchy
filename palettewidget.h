#ifndef _PALETTEWIDGET_H_
#define _PALETTEWIDGET_H_

#include <QFrame>
#include <QTreeView>
#include <QVector>
#include <QWidget>

class QComboBox;
class QScrollArea;
class QMouseEvent;
class QPaintEvent;

class Color;
class ColorManager;
class MetaColorManager;
class PaletteModel;

#define SWATCH_MINE -1
#define SWATCH_DOCUMENT -2

class SwatchWidget : public QWidget
{
  Q_OBJECT;

 public:
  SwatchWidget(QWidget *parent = NULL);
  ~SwatchWidget();

  void setColorManager(ColorManager *cm);

  QPoint itemPosition(int idx) const;
  int itemIndex(const QPoint &point) const;

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

 signals:
  void indexSelected(int);
  void indexHovered(int);

 private:
  QPoint mapToTable(const QPoint &pos) const;
  const Color* mapToElem(const QPoint &pos) const;

  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void paintEvent(QPaintEvent *event);

 private:
  bool picking_;
  const QVector<const Color *> *list_;
  int cols_;
};

class PaletteListView : public QTreeView
{
  Q_OBJECT;
  
 public:
  PaletteListView(QWidget *parent = NULL);
  ~PaletteListView();

 signals:
  void itemSelected(int);
  void itemHovered(int);

 public slots:
  void selectItem(int row);

 private:
  void mouseMoveEvent(QMouseEvent *event);
  void selectionChanged(const QItemSelection &selected,
                        const QItemSelection &deselected);
};

class Palette : public QFrame
{
  Q_OBJECT;

 public:
  Palette(QWidget *parent = NULL);
  ~Palette();

  QSize sizeHint() const;
  QSize minimumSizeHint() const;

 public slots:
  void setColor(const Color *c);
  
 private:
  void paintEvent(QPaintEvent *event);

 private:
  const Color *hovered_;
};

class PaletteWidget : public QWidget
{
  Q_OBJECT;

 public:
  PaletteWidget(MetaColorManager *meta, QWidget *parent = NULL);
  ~PaletteWidget();

  void setColorManager(ColorManager *cm);

 signals:
  void colorSelected(const Color *);
  void userColorSetIsEmpty();

 private slots:
  void itemSelected(int row);
  void itemHovered(int row);
  void initializeColorSet();
  void changeColorSet(int index);

 private:
  const QVector<const Color *> *list_;

  QComboBox *colorSet_;
  SwatchWidget *swatchWidget_;
  QScrollArea *swatchScrollArea_;
  PaletteListView *listWidget_;
  PaletteModel *model_;
  Palette *palette_;
  MetaColorManager *meta_;
};


#endif
