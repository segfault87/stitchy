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

void Settings::setDefaultPalette(const QString &data)
{
  settings_->setValue("colors/default_palette", data);
}

QByteArray Settings::state() const
{
  return settings_->value("general/state", QByteArray()).toByteArray();
}

QByteArray Settings::geometry() const
{
  return settings_->value("general/geometry", QByteArray()).toByteArray();
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
