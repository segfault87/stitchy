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
