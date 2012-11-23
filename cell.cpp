#include <QGraphicsScene>

#include "document.h"
#include "stitch.h"
#include "utils.h"

#include "cell.h"

const int featureMaskList[] = {
  MASK_CELL_FULL,
  MASK_CELL_HALF_S,
  MASK_CELL_HALF_BS,
  MASK_CELL_PETITE_TL,
  MASK_CELL_PETITE_TR,
  MASK_CELL_PETITE_BL,
  MASK_CELL_PETITE_BR,
  MASK_CELL_QUARTER_TL_S,
  MASK_CELL_QUARTER_TL_BS,
  MASK_CELL_QUARTER_TR_S,
  MASK_CELL_QUARTER_TR_BS,
  MASK_CELL_QUARTER_BL_S,
  MASK_CELL_QUARTER_BL_BS,
  MASK_CELL_QUARTER_BR_S,
  MASK_CELL_QUARTER_BR_BS
};

Cell::Cell()
    : document_(NULL)
{
  featureMask_ = 0;
  for (int i = 0; i < CELL_COUNT; ++i) {
    features_[i] = NULL;
    colors_[i] = NULL;
  }
}

Cell::Cell(const QPoint &pos, Document *document)
    : pos_(pos), document_(document)
{
  featureMask_ = 0;
  for (int i = 0; i < CELL_COUNT; ++i) {
    features_[i] = NULL;
    colors_[i] = NULL;
  }
}

Cell::Cell(const Cell &other)
{
  featureMask_ = other.featureMask_;
  pos_ = other.pos_;
  document_ = other.document_;
  for (int i = 0; i < CELL_COUNT; ++i) {
    features_[i] = NULL;
    colors_[i] = other.colors_[i];
  }
}

Cell::~Cell()
{
  if (!featureMask_)
    return;

  for (int i = 0; i < CELL_COUNT; ++i)
    remove(i);
}

void Cell::move(const QPoint &pos)
{
  pos_ = pos;

  for (int i = 0; i < CELL_COUNT; ++i) {
    if (features_[i])
      features_[i]->setPos(Utils::mapToCoord(pos_) + subareaOffset(i));
  }
}

void Cell::addFullStitch(const Color *color)
{
  addFeature(CELL_FULL, color);
}

void Cell::addHalfStitch(Orientation orientation,
                                    const Color *color)
{
  int subcellId;

  if (orientation == Orientation_Slash)
    subcellId = CELL_HALF_S;
  else
    subcellId = CELL_HALF_BS;

  addFeature(subcellId, color);
}

void Cell::addPetiteStitch(Subarea subarea,
                                        const Color *color)
{
  int subcellId;

  if (subarea == Subarea_TopLeft)
    subcellId = CELL_PETITE_TL;
  else if (subarea == Subarea_TopRight)
    subcellId = CELL_PETITE_TR;
  else if (subarea == Subarea_BottomLeft)
    subcellId = CELL_PETITE_BL;
  else if (subarea == Subarea_BottomRight)
    subcellId = CELL_PETITE_BR;
  else
    return;

  addFeature(subcellId, color);
}

void Cell::addQuarterStitch(Orientation orientation,
                            Subarea subarea,
                            const Color *color)
{
  int subcellId;

  if (orientation == Orientation_Slash) {
    if (subarea == Subarea_TopLeft)
      subcellId = CELL_QUARTER_TL_S;
    else if (subarea == Subarea_TopRight)
      subcellId = CELL_QUARTER_TR_S;
    else if (subarea == Subarea_BottomLeft)
      subcellId = CELL_QUARTER_BL_S;
    else if (subarea == Subarea_BottomRight)
      subcellId = CELL_QUARTER_BR_S;
    else
      return;
  } else {
    if (subarea == Subarea_TopLeft)
      subcellId = CELL_QUARTER_TL_BS;
    else if (subarea == Subarea_TopRight)
      subcellId = CELL_QUARTER_TR_BS;
    else if (subarea == Subarea_BottomLeft)
      subcellId = CELL_QUARTER_BL_BS;
    else if (subarea == Subarea_BottomRight)
      subcellId = CELL_QUARTER_BR_BS;
    else
      return;
  }
  
  addFeature(subcellId, color);
}

void Cell::merge(const Cell &other)
{
  for (int i = 0; i < CELL_COUNT; ++i) {
    if (other.contains(i))
      addFeature(i, other.color(i));
  }
}

bool Cell::contains(int feature) const
{
  return featureMask_ & featureMaskList[feature];
}

const Color* Cell::color(int feature) const
{
  return colors_[feature];
}

bool Cell::isEmpty() const
{
  return featureMask_ == 0;
}

void Cell::remove(int feature)
{
  if (!contains(feature))
    return;

  if (features_[feature]) {
    StitchItem *it = features_[feature];
    it->release();
    delete it;
    features_[feature] = NULL;
  }
  colors_[feature] = NULL;

  featureMask_ = featureMask_ & ~featureMaskList[feature];
}

void Cell::createGraphicsItems()
{
  clearGraphicsItems();

  for (int i = 0; i < CELL_COUNT; ++i) {
    if ((featureMask_ & featureMaskList[i]) == 0)
      continue;

    PositionedStitchItem *it;
    Orientation o = Orientation_Slash;

    switch (i) {
      case CELL_HALF_S:
      case CELL_QUARTER_TL_S:
      case CELL_QUARTER_TR_S:
      case CELL_QUARTER_BL_S:
      case CELL_QUARTER_BR_S:
        o = Orientation_Slash;
        break;
      case CELL_HALF_BS:
      case CELL_QUARTER_TL_BS:
      case CELL_QUARTER_TR_BS:
      case CELL_QUARTER_BL_BS:
      case CELL_QUARTER_BR_BS:
        o = Orientation_Backslash;
        break;
      default:
        ;
    }

    if (i == CELL_FULL) {
      it = new FullStitchItem(Utils::mapToCoord(pos_),
                              colors_[i],
                              document_);
    } else if (i >= CELL_HALF_S && i <= CELL_HALF_BS) {
      it = new HalfStitchItem(o,
                              Utils::mapToCoord(pos_),
                              colors_[i],
                              document_);
    } else if (i >= CELL_PETITE_TL && i <= CELL_PETITE_BR) {
      it = new PetiteStitchItem(
          Utils::mapToCoord(pos_) + subareaOffset(i),
          colors_[i],
          document_);
    } else if (i >= CELL_QUARTER_TL_S && i <= CELL_QUARTER_BR_BS) {
      it = new QuarterStitchItem(
          o,
          Utils::mapToCoord(pos_) + subareaOffset(i),
          colors_[i],
          document_);
    } else {
      it = NULL;
    }

    if (it) {
      it->acquire();
      features_[i] = it;
      document_->scene()->addItem(it);
    }
  }
}

void Cell::clearGraphicsItems()
{
  for (int i = 0; i < CELL_COUNT; ++i) {
    if (features_[i]) {
      features_[i]->release();
      delete features_[i];
      features_[i] = NULL;
    }
  }
}

QPointF Cell::subareaOffset(int feature)
{
  switch (feature) {
    case CELL_PETITE_TR:
    case CELL_QUARTER_TR_S:
    case CELL_QUARTER_TR_BS:
      return QPointF(5.0f, 0.0f);
    case CELL_PETITE_BL:
    case CELL_QUARTER_BL_S:
    case CELL_QUARTER_BL_BS:
      return QPointF(0.0f, 5.0f);
    case CELL_PETITE_BR:
    case CELL_QUARTER_BR_S:
    case CELL_QUARTER_BR_BS:
      return QPointF(5.0f, 5.0f);
    default:
      return QPointF();
  }
}

void Cell::resetPosition()
{
  for (int i = 0; i < CELL_COUNT; ++i) {
    if (features_[i])
      features_[i]->setPos(Utils::mapToCoord(pos_));
  }
}

void Cell::addFeature(int feature, const Color *color)
{
  remove(feature);
  removeFeatures(affectedFeatures(feature));

  colors_[feature] = color;
  featureMask_ |= featureMaskList[feature];
}

int Cell::affectedFeatures(int feature)
{
  if (feature == CELL_FULL)
    return ~MASK_CELL_FULL;

  int affects = MASK_CELL_FULL | featureMaskList[feature];

  switch (feature) {
    case CELL_HALF_S:
      affects |= MASK_CELL_PETITE_TR | MASK_CELL_PETITE_BL |
          MASK_CELL_QUARTER_TR_S | MASK_CELL_QUARTER_TR_BS |
          MASK_CELL_QUARTER_BL_S | MASK_CELL_QUARTER_BL_BS; 
      break;
    case CELL_HALF_BS:
      affects |= MASK_CELL_PETITE_TL | MASK_CELL_PETITE_BR |
          MASK_CELL_QUARTER_TL_S | MASK_CELL_QUARTER_TL_BS |
          MASK_CELL_QUARTER_BR_S | MASK_CELL_QUARTER_BR_BS; 
      break;
    case CELL_PETITE_TL:
      affects |= MASK_CELL_HALF_BS |
          MASK_CELL_QUARTER_TL_S | MASK_CELL_QUARTER_TL_BS;
      break;
    case CELL_PETITE_TR:
      affects |= MASK_CELL_HALF_S |
          MASK_CELL_QUARTER_TR_S | MASK_CELL_QUARTER_TR_BS;
      break;
    case CELL_PETITE_BL:
      affects |= MASK_CELL_HALF_S |
          MASK_CELL_QUARTER_BL_S | MASK_CELL_QUARTER_BL_BS;
      break;
    case CELL_PETITE_BR:
      affects |= MASK_CELL_HALF_BS |
          MASK_CELL_QUARTER_BR_S | MASK_CELL_QUARTER_BR_BS;
      break;
    case CELL_QUARTER_TL_S:
    case CELL_QUARTER_TL_BS:
    case CELL_QUARTER_BR_S:
    case CELL_QUARTER_BR_BS:
      affects |= MASK_CELL_HALF_BS;
      break;
    case CELL_QUARTER_TR_S:
    case CELL_QUARTER_TR_BS:
    case CELL_QUARTER_BL_S:
    case CELL_QUARTER_BL_BS:
      affects |= MASK_CELL_HALF_S;
      break;
    default:
      ;
  }

  return affects;
}

void Cell::removeFeatures(int mask)
{
  for (int i = 0; i < CELL_COUNT; ++i) {
    if (featureMaskList[i] & featureMask_ & mask)
      remove(i);
  }
}

