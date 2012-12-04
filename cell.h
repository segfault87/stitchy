#ifndef _CELL_H_
#define _CELL_H_

#include <QPoint>

#include "stitch.h"

class QGraphicsItem;

class Color;
class Document;
class DocumentIo;

/* 0: Full
 * 1: Half (Slash)
 * 2: Half (Backslash)
 * 3: Petite (TL)
 * 4: Petite (TR)
 * 5: Petite (BL)
 * 6: Petite (BR)
 * 7: Quarter (TL; Slash)
 * 8: Quarter (TL; Backslash)
 * 9: Quarter (TR; Slash)
 *10: Quarter (TR; Backslash)
 *11: Quarter (BL; Slash)
 *12: Quarter (BL; Backslash)
 *13: Quarter (BR; Slash)
 *14: Quarter (BR; Backslash)
 */

#define CELL_FULL           0
#define CELL_HALF_S         1
#define CELL_HALF_BS        2
#define CELL_PETITE_TL      3
#define CELL_PETITE_TR      4
#define CELL_PETITE_BL      5
#define CELL_PETITE_BR      6
#define CELL_QUARTER_TL_S   7
#define CELL_QUARTER_TL_BS  8
#define CELL_QUARTER_TR_S   9
#define CELL_QUARTER_TR_BS 10
#define CELL_QUARTER_BL_S  11
#define CELL_QUARTER_BL_BS 12
#define CELL_QUARTER_BR_S  13
#define CELL_QUARTER_BR_BS 14
#define CELL_COUNT         15

#define MASK_CELL_FULL          0x0001
#define MASK_CELL_HALF_S        0x0002
#define MASK_CELL_HALF_BS       0x0004
#define MASK_CELL_HALF          0x0006
#define MASK_CELL_PETITE_TL     0x0008
#define MASK_CELL_PETITE_TR     0x0010
#define MASK_CELL_PETITE_BL     0x0020
#define MASK_CELL_PETITE_BR     0x0040
#define MASK_CELL_PETITE        0x0078
#define MASK_CELL_QUARTER_TL_S  0x0080
#define MASK_CELL_QUARTER_TL_BS 0x0100
#define MASK_CELL_QUARTER_TR_S  0x0200
#define MASK_CELL_QUARTER_TR_BS 0x0400
#define MASK_CELL_QUARTER_BL_S  0x0800
#define MASK_CELL_QUARTER_BL_BS 0x1000
#define MASK_CELL_QUARTER_BR_S  0x2000
#define MASK_CELL_QUARTER_BR_BS 0x4000
#define MASK_CELL_QUARTER       0x7f80

enum Subarea
{
  Subarea_TopLeft,
  Subarea_TopRight,
  Subarea_BottomLeft,
  Subarea_BottomRight
};

bool FeatureMaskTest(int mask, int feature);

class Cell
{
 public:
  Cell();
  Cell(const QPoint &pos, Document *document);
  Cell(const Cell &other);
  ~Cell();

  const QPoint& pos() const { return pos_; }

  void move(const QPoint &pos);

  Cell& operator=(const Cell &other);

  void addFullStitch(const Color *color);
  void addHalfStitch(Orientation orientation, const Color *color);
  void addPetiteStitch(Subarea subarea, const Color *color);
  void addQuarterStitch(Orientation orientation,
                        Subarea subarea,
                        const Color *color);
  void merge(const Cell &other);

  void addFeature(int feature, const Color *color);

  int featureMask() const { return featureMask_; }
  bool contains(int feature) const;
  const Color* color(int feature) const;
  bool isEmpty() const;
  void remove(int feature);
  
  void createGraphicsItems(QGraphicsItem *parent = NULL);
  void clearGraphicsItems();
  
 private:
  QPointF subareaOffset(int feature);
  void resetPosition();
  int affectedFeatures(int feature);
  void removeFeatures(int mask);

 private:
  QPoint pos_;
  PositionedStitchItem *features_[CELL_COUNT];
  const Color *colors_[CELL_COUNT];
  int featureMask_;

  Document *document_;

  friend class DocumentIoV1;
};

#endif
