#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <QSettings>
#include <QStringList>

class Settings
{
 public:
  Settings();
  ~Settings();

  static Settings* self();

  /* colors */
  QString defaultPalette() const;
  QStringList myColors() const;

  void setDefaultPalette(const QString &data);
  void setMyColors(const QStringList &list);
  
  /* general */
  QByteArray state() const;
  QByteArray geometry() const;

  void setState(const QByteArray &data);
  void setGeometry(const QByteArray &data);

 private:
  void writeConfig();

 private:
  static Settings *instance_;

  QSettings *settings_;
};

#endif
