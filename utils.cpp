#include "utils.h"

QPointF Utils::mapToCoord(const QPoint &point)
{
  return QPointF(point.x() * 10.0f, point.y() * 10.0f);
}
