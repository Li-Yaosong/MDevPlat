#include "androidsdksettings.h"
#include "androidsimulator.h"
// #include "handleldterminalconstants.h"

#include <QSettings>
#include <QGlobalStatic>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

#include "synchronousprocess.h"

#include <QRegularExpression>

Q_GLOBAL_STATIC(HandleldTerminal::Internal::AndroidSdkSettings, settings)

namespace HandleldTerminal {
namespace Internal {

//const char installLocationKey[] = "Installed Location:";
//const char revisionKey[] = "Version:";
//const char descriptionKey[] = "Description:";
//const char commonArgsKey[] = "Common Arguments:";

const char avdInfoNameKey[] = "Name:";
const char avdInfoPathKey[] = "Path:";
const char avdInfoAbiKey[] = "abi.type";
const char avdInfoErrorKey[] = "Error:";
const char avdInfoTargetKey[] = "target";

const std::map<AndroidSdkSettings::MarkerTag, const char *> markerTags {
    {AndroidSdkSettings::MarkerTag::InstalledPackagesMarker,    "Installed packages:"},
    {AndroidSdkSettings::MarkerTag::AvailablePackagesMarkers,   "Available Packages:"},
    {AndroidSdkSettings::MarkerTag::AvailablePackagesMarkers,   "Available Updates:"},
    {AndroidSdkSettings::MarkerTag::PlatformMarker,             "platforms"},
    {AndroidSdkSettings::MarkerTag::SystemImageMarker,          "system-images"},
    {AndroidSdkSettings::MarkerTag::BuildToolsMarker,           "build-tools"},
    {AndroidSdkSettings::MarkerTag::SdkToolsMarker,             "tools"},
    {AndroidSdkSettings::MarkerTag::PlatformToolsMarker,        "platform-tools"},
    {AndroidSdkSettings::MarkerTag::EmulatorToolsMarker,        "emulator"}
};

QChar pathListSeparator()
{
#ifdef Q_OS_WIN
    return QLatin1Char(';');
#else
    return QLatin1Char(':');
#endif
}

QProcessEnvironment AndroidSdkSettings::toolsEnvironment()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    // QString jdkLocation = QString(QCoreApplication::applicationDirPath()+"/Java/jdk-17.0.12");

    // if (!jdkLocation.isEmpty()) {
    //     env.insert("JAVA_HOME", jdkLocation);
    //     QString binPath = jdkLocation;
    //     binPath.append("/bin");
    //     QString path = env.value("PATH");
    //     const QString toPrepend = binPath + QString(pathListSeparator());
    //     if (!path.startsWith(toPrepend))
    //         path.prepend(toPrepend);
    // }
    return env;
}

static bool valueForKey(QString key, const QString &line, QString *value = nullptr)
{
    auto trimmedInput = line.trimmed();
    if (trimmedInput.startsWith(key)) {
        if (value)
            *value = trimmedInput.section(key, 1, 1).trimmed();
        return true;
    }
    return false;
}

// static int platformNameToApiLevel(const QString &platformName)
// {
//     int apiLevel = -1;
//     QRegularExpression re("(android-)(?<apiLevel>[0-9]{1,})",
//                           QRegularExpression::CaseInsensitiveOption);
//     QRegularExpressionMatch match = re.match(platformName);
//     if (match.hasMatch()) {
//         QString apiLevelStr = match.captured("apiLevel");
//         apiLevel = apiLevelStr.toInt();
//     }
//     return apiLevel;
// }

static bool avdManagerCommand(const QStringList &args, QString *output)
{
    QString avdManagerToolPath = qAndroid->avdManager();
    Utils::SynchronousProcess proc;
    auto env = qAndroid->toolsEnvironment().toStringList();
    proc.setEnvironment(env);
    Utils::SynchronousProcessResponse response = proc.runBlocking(avdManagerToolPath, args);
    if (response.result == Utils::SynchronousProcessResponse::Finished) {
        if (output)
            *output = response.allOutput();
        return true;
    }
    return false;
}

AndroidSdkSettings::AndroidSdkSettings()
{
    QDir dir(QCoreApplication::applicationDirPath());
    if (dir.cd("Android")) {
        m_path = dir.absolutePath();
    }
}

AndroidSdkSettings *AndroidSdkSettings::instance()
{
    return settings();
}

QString AndroidSdkSettings::path() const
{
    return m_path;
}

QString AndroidSdkSettings::sdkPath() const
{
    return filePath(m_path, "Sdk", QString());
}

QString AndroidSdkSettings::avdManager() const
{
#ifdef Q_OS_WIN
    QString suffix = ".bat";
#else
    QString suffix = "";
#endif

    return filePath(sdkPath() + "/cmdline-tools/latest/bin", "avdmanager", suffix);
}

QString AndroidSdkSettings::sdkManager() const
{
#ifdef Q_OS_WIN
    QString suffix = ".bat";
#else
    QString suffix = ".sh";
#endif
    return filePath(sdkPath() + "/cmdline-tools/latest/bin", "sdkmanager", suffix);
}

QString AndroidSdkSettings::emulator() const
{
    return filePath(sdkPath() + "/emulator", "emulator");
}

QString AndroidSdkSettings::adb() const
{
    return filePath(sdkPath() + "/platform-tools", "adb");
}

bool AndroidSdkSettings::parseAvd(const QStringList &deviceInfo, Simulator *sim)
{
    if(sim == nullptr)
        return false;
    foreach (const QString &line, deviceInfo) {
        QString value;
        if (valueForKey(avdInfoErrorKey, line)) {
            qDebug() << "Avd Parsing: Skip avd device. Error key found:" << line;
            return false;
        } else if (valueForKey(avdInfoNameKey, line, &value)) {
            sim->setName(value);
        } else if (valueForKey(avdInfoPathKey, line, &value)) {
            const QString avdPath = value;
            if (QDir(avdPath).exists())
            {
                // Get ABI.
                QString configFile = avdPath;
                configFile.append("/config.ini");
                QSettings config(configFile, QSettings::IniFormat);
                value = config.value(avdInfoAbiKey).toString();
                if (!value.isEmpty())
                    sim->setAbi(value);
                else
                   qDebug() << "Avd Parsing: Cannot find ABI:" << configFile;

                sim->setSettingsFile(configFile);
//                // Get Target
//                Utils::FileName avdInfoFile = avdPath.parentDir();
//                QString avdInfoFileName = avdPath.toFileInfo().baseName() + ".ini";
//                avdInfoFile.appendPath(avdInfoFileName);
//                QSettings avdInfo(avdInfoFile.toString(), QSettings::IniFormat);
//                value = avdInfo.value(avdInfoTargetKey).toString();
//                if (!value.isEmpty())
//                    sim->setSdk(value.section('-', -1).toInt());
//                else
//                   qDebug() << "Avd Parsing: Cannot find sdk API:" << avdInfoFile.toString();
            }
        }
    }
    return sim->isValid();
}

QList<Simulator *> AndroidSdkSettings::parseAvdList(const QString &output)
{
    QList<Simulator *> avdList;
    QStringList avdInfo;
    auto parseAvdInfo = [&avdInfo, &avdList, this] () {
        Simulator *avd = new AndroidSimulator;
        if (parseAvd(avdInfo, avd)) {
             avdList << avd;
        } else {
            qDebug() << "Avd Parsing: Parsing failed: " << avdInfo;
            delete avd;
        }
        avdInfo.clear();
    };

    foreach (QString line,  output.split('\n')) {
        if (line.startsWith("---------") || line.isEmpty()) {
            parseAvdInfo();
        } else {
            avdInfo << line;
        }
    }

    if (!avdInfo.isEmpty())
        parseAvdInfo();

    return avdList;
}

QList<Simulator *> AndroidSdkSettings::simulators()
{
    QString output;
    if (!avdManagerCommand(QStringList({"list", "avd"}), &output)) {
        qWarning() << "Avd list command failed" << output;
        return QList<Simulator *>();
    }
    return parseAvdList(output);
}

static bool sdkManagerCommand(const QStringList &args, QString *output)
{
    Utils::SynchronousProcess proc;
    proc.setProcessEnvironment(qAndroid->toolsEnvironment());
    proc.setTimeOutMessageBoxEnabled(true);
    Utils::SynchronousProcessResponse response = proc.runBlocking(qAndroid->sdkManager(), args);
    if (output)
        *output = response.allOutput();
    return response.result == Utils::SynchronousProcessResponse::Finished;
}

QPair<QString, QString> AndroidSdkSettings::parseSystemImage(const QStringList &data) const
{
    if (data.isEmpty()) {
        qDebug() << ": Empty input";
        return QPair<QString, QString>();
    }

    QStringList headerParts = data.at(0).split(';');
    if (headerParts.count() < 4) {
        qDebug() << "%1: Unexpected header:" << data;
        return QPair<QString, QString>();
    }

//    int apiLevel = platformNameToApiLevel(headerParts.at(1));
//    if (apiLevel == -1) {
//        qDebug() << "System-image: Can not parse api level:"<< data;
//        return QPair<QString, QString>();
//    }

//    QStringList extraKeys {installLocationKey, revisionKey, descriptionKey};
//    foreach (QString line, data) {
//        QString value;
//        for (auto key: extraKeys) {
//            if (valueForKey(key, line, &value)) {
//                if (key == installLocationKey)
//                    output.installedLocation = Utils::FileName::fromString(value);
//                else if (key == revisionKey)
//                    output.revision = QVersionNumber::fromString(value);
//                else if (key == descriptionKey)
//                    output.description = value;
//                else
//                    output.extraData[key] = value;
//                break;
//            }
//        }
//    }
    return qMakePair(headerParts.at(1) + ":" + headerParts.at(3), data.at(0));
}

void AndroidSdkSettings::parsePackageData(MarkerTag packageMarker, const QStringList &data)
{
    if(!(!data.isEmpty() && packageMarker != None))
        return;

    switch (packageMarker) {
    case MarkerTag::BuildToolsMarker:
        break;

    case MarkerTag::SdkToolsMarker:
        break;

    case MarkerTag::PlatformToolsMarker:
        break;

    case MarkerTag::EmulatorToolsMarker:
        break;

    case MarkerTag::PlatformMarker:
        break;

    case MarkerTag::SystemImageMarker:
    {
        QPair<QString, QString> result = parseSystemImage(data);
        if (m_currentSection == InstalledPackagesMarker) {
            m_systemImages.insert(result.first, result.second);
        }
    }
        break;

    default:
        qWarning() << "Unhandled package: " << markerTags.at(packageMarker);
        break;
    }
}

AndroidSdkSettings::MarkerTag AndroidSdkSettings::parseMarkers(const QString &line)
{
    if (line.isEmpty())
        return EmptyMarker;

    for (auto pair: markerTags) {
        if (line.startsWith(QLatin1String(pair.second)))
            return pair.first;
    }

    return None;
}

QStringList AndroidSdkSettings::supported() const
{
    checkLoaded();
    return m_systemImages.keys();
}

QString AndroidSdkSettings::targetSdkPath(const QString &abi) const
{
    checkLoaded();
    return m_systemImages.value("android-27:" + abi);
}

void AndroidSdkSettings::load()
{
    m_systemImages.clear();
    QStringList args({"--list", "--verbose"});
    QString output;
    if (sdkManagerCommand(args, &output)) {
        QStringList packageData;
        bool collectingPackageData = false;
        MarkerTag currentPackageMarker = MarkerTag::None;

        auto processCurrentPackage = [&]() {
            if (collectingPackageData) {
                collectingPackageData = false;
                parsePackageData(currentPackageMarker, packageData);
                packageData.clear();
            }
        };

        QRegularExpression delimiters("[\\n\\r]");
        foreach (QString outputLine, output.split(delimiters)) {
            MarkerTag marker = parseMarkers(outputLine.trimmed());
            if (marker & SectionMarkers) {
                // Section marker found. Update the current section being parsed.
                m_currentSection = marker;
                processCurrentPackage();
                continue;
            }

            if (m_currentSection == None)
                continue; // Continue with the verbose output until a valid section starts.

            if (marker == EmptyMarker) {
                // Empty marker. Occurs at the end of a package details.
                // Process the collected package data, if any.
                processCurrentPackage();
                continue;
            }

            if (marker == None) {
                if (collectingPackageData)
                    packageData << outputLine; // Collect data until next marker.
                else
                    continue;
            } else {
                // Package marker found.
                processCurrentPackage(); // New package starts. Process the collected package data, if any.
                currentPackageMarker = marker;
                collectingPackageData = true;
                packageData << outputLine;
            }
        }
    }
}

void AndroidSdkSettings::checkLoaded() const
{
    if (m_loaded)
        return;
    AndroidSdkSettings *s = const_cast<AndroidSdkSettings *>(this);
    s->load();
    s->m_loaded = true;
}

QString AndroidSdkSettings::filePath(const QString &dir, const QString &file, const QString &suffix) const
{
    QString name = file;
#ifdef Q_OS_WIN
    name.append(suffix);
#else
    Q_UNUSED(suffix)
#endif

    QDir _dir(dir);
    if (_dir.exists(name))
        return _dir.filePath(name);
    return QString();
}

}
}
