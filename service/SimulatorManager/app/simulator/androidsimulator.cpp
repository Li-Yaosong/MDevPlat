#include "androidsimulator.h"
#include "androidsettings.h"
#include "androidsdksettings.h"
#include "handleldterminalconstants.h"

#include "synchronousprocess.h"

#include <QFileInfo>
#include <QSettings>
#include <QRegularExpression>
#include <QDebug>
#include <QProcess>

namespace HandleldTerminal {
namespace Internal {

AndroidSimulator::AndroidSimulator(QObject *parent)
    : Simulator(parent)
    , m_memory(4)
    , m_cpuNumber(6)
    , m_xRes(1080)
    , m_yRes(1920)
    , m_androidSettings(new AndroidSettings)
{

}

AndroidSimulator::~AndroidSimulator()
{
    delete m_androidSettings;
    stop();
}

bool AndroidSimulator::start()
{
    if (isRunning())
        return true;
    QString emulator = qAndroid->emulator();
    QFileInfo info(emulator);
    if (emulator.isEmpty() || !info.exists()) {
        setError(tr("Install the missing emulator tool to the"
                                 " installed Android SDK."));
        return false;
    }


    // start the emulator
    QStringList arguments;

//    arguments << "-partition-size" << QString::number(m_config.partitionSize())
//              << "-avd" << name();
    arguments << "-avd" << name();
#ifdef Q_PROCESSOR_X86_64
    // 在x86_64上启动arm64-v8a会出现PCI bus not available for hda导致启动失败
    // 添加qemu参数禁止
    QString target = abi();
    if (target.contains("arm")) {
        arguments << "-qemu" << "-machine" << "virt,accel=tcg";
    }
#else
#endif
    bool result = startProcess(emulator, qAndroid->toolsEnvironment().toStringList(), arguments);
    if (result) {
        obtainId();
    }
    return result;
}

bool AndroidSimulator::stop()
{
    Simulator::stop();
    if (isRunning() && !m_emulatorId.isEmpty()) {
        QProcess killProcess;
        killProcess.setProgram(qAndroid->adb());
        killProcess.setProcessEnvironment(qAndroid->toolsEnvironment());
        killProcess.setArguments({"-s", m_emulatorId, "emu", "kill"});
        killProcess.start();
        killProcess.waitForFinished();
    }
    return true;
}

bool AndroidSimulator::isRunning() const
{
    if (m_emulatorId.isEmpty()) {
        AndroidSimulator *that = const_cast<AndroidSimulator *>(this);
        that->obtainId();
    }
    return !m_emulatorId.isEmpty();
//    return Simulator::isRunning();
}

SimulatorType AndroidSimulator::type() const
{
    return Android;
}

QVariantMap AndroidSimulator::settings() const
{
    if (m_settings.isEmpty())
        return QVariantMap();

    QVariantMap result;
    result.insert(HandleldTerminal::Constants::Simulator::MEMORY, m_memory);
    result.insert(HandleldTerminal::Constants::Simulator::CPUNUMBER, m_cpuNumber);
    result.insert(HandleldTerminal::Constants::Simulator::X_RESOLUTION, m_xRes);
    result.insert(HandleldTerminal::Constants::Simulator::Y_RESOLUTION, m_yRes);
    return result;
}

void AndroidSimulator::updateSettings(const QVariantMap &map)
{
    quint8 memory = static_cast<quint8>(map.value(HandleldTerminal::Constants::Simulator::MEMORY).toInt());
    m_memory = qMax(quint8(2), qMin(quint8(32), memory));
    quint8 cpuNumber = static_cast<quint8>(map.value(HandleldTerminal::Constants::Simulator::CPUNUMBER).toInt());
    m_cpuNumber = qMax(quint8(2), qMin(quint8(32), cpuNumber));

    m_xRes = static_cast<quint16>(map.value(HandleldTerminal::Constants::Simulator::X_RESOLUTION).toUInt());
    m_yRes = static_cast<quint16>(map.value(HandleldTerminal::Constants::Simulator::Y_RESOLUTION).toUInt());

    m_androidSettings->insert(HandleldTerminal::Constants::Simulator::Android::MEMORY, QString("%1M").arg(m_memory * 1024));
    m_androidSettings->insert(HandleldTerminal::Constants::Simulator::Android::CPUNUMBER, QString::number(m_cpuNumber));
    m_androidSettings->insert(HandleldTerminal::Constants::Simulator::Android::X_RESOLUTION, QString::number(m_xRes));
    m_androidSettings->insert(HandleldTerminal::Constants::Simulator::Android::Y_RESOLUTION, QString::number(m_yRes));
    m_androidSettings->saveSettings();
}

static quint8 convertSizeStringToInt(const QString &sizeString) {
    QRegularExpression regex("(\\d+)([M]?)");
    QRegularExpressionMatch match = regex.match(sizeString);

    if (!match.hasMatch()) {
        return 0;
    }

    int number = match.captured(1).toInt();
    number = number / 1024;
    return static_cast<quint8>(number);
}

void AndroidSimulator::loadSettings()
{
    if (m_settings.isEmpty())
        return;
    // 配置文件中有以\结束的value，使用QSettings解析出错
    m_androidSettings->setSettingsFile(m_settings);

    bool needToUpdate = false;
    if (m_androidSettings->contains(HandleldTerminal::Constants::Simulator::Android::MEMORY)) {
        QString temp = m_androidSettings->value(HandleldTerminal::Constants::Simulator::Android::MEMORY);
        m_memory = static_cast<quint8>(convertSizeStringToInt(temp));
    } else {
        needToUpdate = true;
        m_androidSettings->insert(HandleldTerminal::Constants::Simulator::Android::MEMORY, QString("%1M").arg(m_memory * 1024));
    }
    if (m_androidSettings->contains(HandleldTerminal::Constants::Simulator::Android::CPUNUMBER)) {
        m_cpuNumber = static_cast<quint8>(m_androidSettings->value(HandleldTerminal::Constants::Simulator::Android::CPUNUMBER).toUInt());
    } else {
        needToUpdate = true;
        m_androidSettings->insert(HandleldTerminal::Constants::Simulator::Android::CPUNUMBER, QString::number(m_cpuNumber));
    }
    if (m_androidSettings->contains(HandleldTerminal::Constants::Simulator::Android::X_RESOLUTION)) {
        m_xRes = static_cast<quint16>(m_androidSettings->value(HandleldTerminal::Constants::Simulator::Android::X_RESOLUTION).toInt());
    } else {
        needToUpdate = true;
        m_androidSettings->insert(HandleldTerminal::Constants::Simulator::Android::X_RESOLUTION, QString::number(m_xRes));
    }
    if (m_androidSettings->contains(HandleldTerminal::Constants::Simulator::Android::Y_RESOLUTION)) {
        m_yRes = static_cast<quint16>(m_androidSettings->value(HandleldTerminal::Constants::Simulator::Android::Y_RESOLUTION).toInt());
    } else {
        needToUpdate = true;
        m_androidSettings->insert(HandleldTerminal::Constants::Simulator::Android::Y_RESOLUTION, QString::number(m_yRes));
    }
    if (needToUpdate) {
        m_androidSettings->saveSettings();
    }
}

void AndroidSimulator::obtainId()
{
    QString adb = qAndroid->adb();
    QFileInfo info(adb);
    if (adb.isEmpty() || !info.exists()) {
        return;
    }

    Utils::SynchronousProcess proc;
    proc.setProcessEnvironment(qAndroid->toolsEnvironment());
    proc.setTimeOutMessageBoxEnabled(true);

    QStringList args = {"devices"};
    Utils::SynchronousProcessResponse response = proc.runBlocking(adb, args);
    if (response.result != Utils::SynchronousProcessResponse::Finished) {
        qWarning() << response.stdErr();
        return;
    }
    QString output = response.allOutput();
    QRegularExpression delimiters("[\\n\\r]");
    QStringList lines = output.split(delimiters);
    foreach (QString outputLine, lines) {
        if (!outputLine.startsWith("emulator-")) {
            continue;
        }
        QStringList parts = outputLine.split(QRegExp("\\s+"));
        if (parts.size() > 1 && parts[1] == "device") {
            if (name() == nameForEmulatorId(parts[0])) {
                m_emulatorId = parts[0];
                return;
            }
        }
    }
}

QString AndroidSimulator::nameForEmulatorId(const QString &id) const
{
    QString adb = qAndroid->adb();

    Utils::SynchronousProcess proc;
    proc.setProcessEnvironment(qAndroid->toolsEnvironment());
    proc.setTimeOutMessageBoxEnabled(true);

    QStringList args = {"-s", id, "emu", "avd", "name"};
    Utils::SynchronousProcessResponse response = proc.runBlocking(adb, args);
    if (response.result != Utils::SynchronousProcessResponse::Finished) {
        qWarning() << response.stdErr();
        return QString();
    }
    QString output = response.allOutput();
//    Test-Android-Arm\r\nOK

    QRegularExpression delimiters("[\\n\\r]");
    QStringList parts = output.split(delimiters);
    QString result = parts.count() > 1 ? parts.first() : QString();

    return result;
}

QString AndroidSimulator::emulatorId() const
{
    return m_emulatorId;
}

}
}
