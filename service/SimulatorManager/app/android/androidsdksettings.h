#ifndef ANDROIDSDKSETTINGS_H
#define ANDROIDSDKSETTINGS_H

#include <QString>
#include <QHash>

#define qAndroid HandleldTerminal::Internal::AndroidSdkSettings::instance()
class QProcessEnvironment;

namespace HandleldTerminal {
namespace Internal {
class Simulator;

class AndroidSdkSettings
{

public:
    enum MarkerTag
    {
        None                        = 0x001,
        InstalledPackagesMarker     = 0x002,
        AvailablePackagesMarkers    = 0x004,
        AvailableUpdatesMarker      = 0x008,
        EmptyMarker                 = 0x010,
        PlatformMarker              = 0x020,
        SystemImageMarker           = 0x040,
        BuildToolsMarker            = 0x080,
        SdkToolsMarker              = 0x100,
        PlatformToolsMarker         = 0x200,
        EmulatorToolsMarker         = 0x400,
        SectionMarkers = InstalledPackagesMarker | AvailablePackagesMarkers | AvailableUpdatesMarker
    };
    AndroidSdkSettings();

    static AndroidSdkSettings *instance();

    QString path() const;

    QString sdkPath() const;

    QString avdManager() const;

    QString sdkManager() const;

    QString emulator() const;

    QString adb() const;

    QList<Simulator *> simulators();

    QStringList supported() const;

    QString targetSdkPath(const QString &abi) const;

    QProcessEnvironment toolsEnvironment();
private:
    void load();
    void checkLoaded() const;
    QString filePath(const QString &dir, const QString &file, const QString &suffix = ".exe") const;
    void parsePackageData(MarkerTag packageMarker, const QStringList &data);
    AndroidSdkSettings::MarkerTag parseMarkers(const QString &line);
    QPair<QString, QString> parseSystemImage(const QStringList &data) const;
    bool parseAvd(const QStringList &deviceInfo, Simulator *sim);
    QList<Simulator *> parseAvdList(const QString &output);
private:
    bool m_loaded = false;
    QString m_path;
    MarkerTag m_currentSection;
    QHash<QString, QString> m_systemImages;
};

}
}
#endif // ANDROIDSDKSETTINGS_H
