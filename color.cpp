#include "color.h"

Color Color::defaultColor = Color("Default Color", "nil", 0, 0, 0);

Color::Color()
{
  
}

Color::Color(const QString &name, const QString &id, byte red, byte green, byte blue)
    : id_(id), name_(name), color_(red, green, blue)
{
  brush_ = QBrush(color_);
}

Color::Color(const Color &other)
{
  id_ = other.id();
  name_ = other.name();
  color_ = other.color();
  brush_ = QBrush(color_);
}

Color::~Color()
{
  
}

Color& Color::operator=(const Color &other)
{
  id_ = other.id();
  name_ = other.name();
  color_ = other.color();
  brush_ = QBrush(color_);

  return *this;
}
