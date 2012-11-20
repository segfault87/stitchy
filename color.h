#ifndef _COLOR_H_
#define _COLOR_H_

#include <QBrush>
#include <QColor>
#include <QString>

#include "common.h"

class Color
{
 public:
  static Color defaultColor;

  Color();
  Color(const QString &name, const QString &id, byte red, byte green, byte blue);
  Color(const Color &other);
  ~Color();

  const QString& name() const { return name_; }
  const QString& id() const { return id_; }
  
  const QColor& color() const { return color_; }
  const QBrush& brush() const { return brush_; }
  byte red() const { return color_.red(); }
  byte green() const { return color_.green(); }
  byte blue() const { return color_.blue(); }

  bool operator==(const Color &other) const { return id_ == other.id(); }
  Color& operator=(const Color &other);

 private:
  QString id_;
  QString name_;
  QColor color_;
  QBrush brush_;
};

#endif
