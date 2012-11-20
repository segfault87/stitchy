#ifndef _COLOR_H_
#define _COLOR_H_

#include <QBrush>
#include <QColor>
#include <QString>

#include "common.h"

class ColorManager;

class Color
{
 public:
  static Color defaultColor;

  Color();
  Color(const QString &name, const QString &id, const QColor &color);
  Color(const Color &other);
  ~Color();

  const ColorManager* parent() const { return parent_; }
  const QString& name() const { return name_; }
  const QString& id() const { return id_; }
  
  const QColor& color() const { return color_; }
  const QBrush& brush() const { return brush_; }
  byte red() const { return color_.red(); }
  byte green() const { return color_.green(); }
  byte blue() const { return color_.blue(); }

  void setParent(const ColorManager *cm) { parent_ = cm; }

  bool operator==(const Color &other) const { return id_ == other.id(); }
  Color& operator=(const Color &other);

 private:
  const ColorManager *parent_;
  QString id_;
  QString name_;
  QColor color_;
  QBrush brush_;
};

#endif
