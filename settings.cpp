#include "settings.h"

Settings* Settings::instance_ = NULL;

Settings* Settings::self()
{
  return instance_;
}

Settings::Settings()
{
  settings_ = new QSettings();

  instance_ = this;
}

Settings::~Settings()
{
  writeConfig();
  delete settings_;
}

QString Settings::defaultPalette() const
{
  return settings_->value("colors/default_palette", QString()).toString();
}

QStringList Settings::myColors() const
{
  return settings_->value("colors/my_colors", QStringList()).toStringList();
}

void Settings::setDefaultPalette(const QString &data)
{
  settings_->setValue("colors/default_palette", data);
}

void Settings::setMyColors(const QStringList &data)
{
  settings_->setValue("colors/my_colors", data);
}

QString Settings::colorFile() const
{
  return settings_->value("general/color_file", QString()).toString();
}

QByteArray Settings::state() const
{
  return settings_->value("general/state", QByteArray()).toByteArray();
}

QByteArray Settings::geometry() const
{
  return settings_->value("general/geometry", QByteArray()).toByteArray();
}

void Settings::setColorFile(const QString &data)
{
  settings_->setValue("general/color_file", data);
}

void Settings::setState(const QByteArray &data)
{
  settings_->setValue("general/state", data);
}

void Settings::setGeometry(const QByteArray &data)
{
  settings_->setValue("general/geometry", data);
}

void Settings::writeConfig()
{
  settings_->sync();
}
