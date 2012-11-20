#ifndef _UTILS_H_
#define _UTILS_H_

#include <QIcon>
#include <QPoint>
#include <QPointF>

class Utils
{
 public:
  static QPointF mapToCoord(const QPoint &point);
  static QIcon icon(const QString &name);
};

#endif
