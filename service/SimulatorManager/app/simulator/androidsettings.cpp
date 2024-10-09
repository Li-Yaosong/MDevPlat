#include "androidsettings.h"
#include <QFile>
#include <QDebug>

namespace HandleldTerminal {
namespace Internal {

AndroidSettings::AndroidSettings(const QString &settingsFile)
    : m_settingsFile(settingsFile)
{
    if (m_settingsFile.isEmpty())
        return;
    loadSettings();
}

void AndroidSettings::setSettingsFile(const QString &file)
{
    m_settingsFile = file;
    loadSettings();
}

bool AndroidSettings::contains(const QString &key)
{
    auto it = std::find_if(m_allSettings.constBegin(), m_allSettings.constEnd(), [key](const QPair<QString, QString> &keyValue){
        return key == keyValue.first;
    });
    return it != m_allSettings.constEnd();
}

QString AndroidSettings::value(const QString &key) const
{
    auto it = std::find_if(m_allSettings.constBegin(), m_allSettings.constEnd(), [key](const QPair<QString, QString> &keyValue){
        return key == keyValue.first;
    });
    return (*it).second;
}

void AndroidSettings::insert(const QString &key, const QString &value)
{
    auto it = std::find_if(m_allSettings.begin(), m_allSettings.end(), [key](const QPair<QString, QString> &keyValue){
        return key == keyValue.first;
    });
    if (it == m_allSettings.end()) {
        m_allSettings << qMakePair(key, value);
        return;
    }
    (*it).second = value;
}

void AndroidSettings::loadSettings()
{
    m_allSettings.clear();

    // 配置文件中有以\结束的value，使用QSettings解析出错
    QFile file(m_settingsFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Unable to open file:" << m_settingsFile;
        return;
    }

    QTextStream in(&file);
    QString key;
    QString value;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#') || line.startsWith(';')) {
            // Skip empty lines and comments
            continue;
        }

        int equalsIndex = line.indexOf('=');
        if (equalsIndex != -1) {
            key = line.left(equalsIndex).trimmed();
            value = line.mid(equalsIndex + 1).trimmed();
            m_allSettings.append(qMakePair(key, value));
        }
    }
    file.close();
}

void AndroidSettings::saveSettings()
{
    QFile file(m_settingsFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qWarning() << "Unable to open file:" << m_settingsFile;
    }
    QString contents;

    for (int i = 0; i < m_allSettings.count(); ++i) {
        const QPair<QString, QString> &keyValue = m_allSettings.at(i);
        contents += QString("%1=%2\n").arg(keyValue.first).arg(keyValue.second);
    }

    file.write(contents.toUtf8());
    file.close();
}

}
}
