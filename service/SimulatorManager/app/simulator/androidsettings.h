#ifndef ANDROIDSETTINGS_H
#define ANDROIDSETTINGS_H

#include <QString>
#include <QVariantMap>
#include <QPair>

namespace HandleldTerminal {
namespace Internal {

class AndroidSettings
{
public:
    AndroidSettings(const QString &settingsFile = QString());

    void setSettingsFile(const QString &file);

    bool contains(const QString &key);
    QString value(const QString &key) const;
    void insert(const QString &key, const QString &value);

    void saveSettings();
private:
    void loadSettings();
private:
    QString m_settingsFile;
    QList<QPair<QString, QString>> m_allSettings;
};

}
}
#endif // ANDROIDSETTINGS_H
