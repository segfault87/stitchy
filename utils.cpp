#include "utils.h"

QPointF Utils::mapToCoord(const QPoint &point)
{
  return QPointF(point.x() * 10.0f, point.y() * 10.0f);
}

QIcon Utils::icon(const QString &name)
{
  return QIcon::fromTheme(name, QIcon(":/icons/fallback/" + name + ".png"));
}
