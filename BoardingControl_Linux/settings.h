#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class LocalSettings
{
public:
    LocalSettings();
    ~LocalSettings();
    static LocalSettings* instance();
public:
    static QSettings *config;
};

inline LocalSettings* LocalSettings::instance(){
  static LocalSettings lst;
  return &lst;
}


#endif // SETTINGSINFO_H
