#include "linuxsimulator.h"
#include "qemusettings.h"
#include "handleldterminalconstants.h"

#include <ssh/sshconnection.h>
#include <ssh/sshconnectionmanager.h>

#include <QSettings>
#include <QEventLoop>
#include <QFileInfo>
#include <QDebug>
#include <QDataStream>

namespace HandleldTerminal {
namespace Internal {

LinuxSimulator::LinuxSimulator(QObject *parent)
    : Simulator (parent)
    , m_memory(4)
    , m_cpuNumber(4)
    , m_xRes(1920)
    , m_yRes(1080)
{
    static quint16 port = 30000;
    static quint16 index = 0;
    m_sshPort = port + index++;
}

LinuxSimulator::~LinuxSimulator()
{
    stop();
}

bool LinuxSimulator::start()
{
    HandleldTerminal::CpuType t = cpu();
    if (t == HandleldTerminal::unkonwn)
        return false;
    QString startExe = qQemu->path(t);
    if (startExe.isEmpty())
        return false;

    if (isRunning())
        return true;

    QStringList arguments({"-m"});
    arguments << QString("%1G").arg(m_memory);
    arguments << QString("-smp") << QString::number(m_cpuNumber);
    arguments << "-boot" << "d" << "-hda" << QString("%1.qcow2").arg(m_path + "/" + name());
    arguments << "-device" << QString("VGA,vgamem_mb=32,xres=%1,yres=%2").arg(m_xRes).arg(m_yRes);
    arguments << QString("-net") << QString("user,hostfwd=tcp::%1-:22,hostfwd=tcp::%2-:3000,hostfwd=tcp::%3-:6080").arg(m_sshPort).arg(m_sshPort+10).arg(m_sshPort+20) << QString("-net") << QString("nic");
    return startProcess(startExe, QProcessEnvironment::systemEnvironment().toStringList(), arguments);
}

bool LinuxSimulator::stop()
{
    m_inStop = true;
    if (isRunning()) {
        if (m_sshConnection != nullptr) {
            if (m_sshConnection->state() == QSsh::SshConnection::Connected) {
                m_sshConnection->disconnectFromHost();
            }
            QSsh::releaseConnection(m_sshConnection);
            m_sshConnection = nullptr;
        }
        Simulator::stop();
    }
    m_inStop = false;
    return true;
}

bool LinuxSimulator::isRunning() const
{
    if (m_sshConnection != nullptr) {
        if (m_sshConnection->state() == QSsh::SshConnection::Connected)
            return true;
    }
    if (!m_inStop) {
        // 能否使用ssh链接
        LinuxSimulator *that = const_cast<LinuxSimulator *>(this);
        if (that->connectUseSsh())
            return true;
    }
    return Simulator::isRunning();
}

quint16 LinuxSimulator::sshPort() const
{
    return m_sshPort;
}

SimulatorType LinuxSimulator::type() const
{
    return Linux;
}

QVariantMap LinuxSimulator::settings() const
{
    if (m_settings.isEmpty())
        return QVariantMap();

    QVariantMap result;
    result.insert(HandleldTerminal::Constants::Simulator::MEMORY, m_memory);
    result.insert(HandleldTerminal::Constants::Simulator::CPUNUMBER, m_cpuNumber);
    result.insert(HandleldTerminal::Constants::Simulator::X_RESOLUTION, m_xRes);
    result.insert(HandleldTerminal::Constants::Simulator::Y_RESOLUTION, m_yRes);
    result.insert(HandleldTerminal::Constants::Simulator::Linux::SSH_PORT, m_sshPort);
    result.insert(HandleldTerminal::Constants::Simulator::Linux::USERNAME, m_userName);
    result.insert(HandleldTerminal::Constants::Simulator::Linux::PASSWORD, m_password);
    return result;
}

void LinuxSimulator::updateSettings(const QVariantMap &map)
{
    if (map.isEmpty())
        return;
    quint8 memory = static_cast<quint8>(map.value(HandleldTerminal::Constants::Simulator::MEMORY).toInt());
    m_memory = qMax(quint8(2), qMin(quint8(32), memory));
    quint8 cpuNumber = static_cast<quint8>(map.value(HandleldTerminal::Constants::Simulator::CPUNUMBER).toInt());
    m_cpuNumber = qMax(quint8(2), qMin(quint8(32), cpuNumber));

    m_xRes = static_cast<quint16>(map.value(HandleldTerminal::Constants::Simulator::X_RESOLUTION).toUInt());
    m_yRes = static_cast<quint16>(map.value(HandleldTerminal::Constants::Simulator::Y_RESOLUTION).toUInt());
    m_sshPort = static_cast<quint16>(map.value(HandleldTerminal::Constants::Simulator::Linux::SSH_PORT, m_sshPort).toUInt());
    m_userName = map.value(HandleldTerminal::Constants::Simulator::Linux::USERNAME).toString();
    m_password = map.value(HandleldTerminal::Constants::Simulator::Linux::PASSWORD).toString();
    if (m_settings.isEmpty())
        return;
    QSettings settings(m_settings, QSettings::IniFormat);
    settings.setValue(HandleldTerminal::Constants::Simulator::Linux::MEMORY, m_memory);
    settings.setValue(HandleldTerminal::Constants::Simulator::Linux::CPUNUMBER, m_cpuNumber);
    settings.setValue(HandleldTerminal::Constants::Simulator::Linux::X_RESOLUTION, m_xRes);
    settings.setValue(HandleldTerminal::Constants::Simulator::Linux::Y_RESOLUTION, m_yRes);
    settings.setValue(HandleldTerminal::Constants::Simulator::Linux::SSH_PORT, m_sshPort);
    settings.setValue(HandleldTerminal::Constants::Simulator::Linux::USERNAME, m_userName);
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << m_password;
    settings.setValue(HandleldTerminal::Constants::Simulator::Linux::PASSWORD, data);
}

QSsh::SshConnection *LinuxSimulator::sshConnection()
{
    if (m_sshConnection == nullptr) {
        QSsh::SshConnectionParameters sshParams;
        sshParams.timeout = 3;
        sshParams.setHost("localhost");
        sshParams.setPort(m_sshPort);
        sshParams.setUserName(m_userName);
        sshParams.authenticationType = QSsh::SshConnectionParameters::AuthenticationTypePassword;
        sshParams.setPassword(m_password);
        QSsh::SshConnection *connection = QSsh::acquireConnection(sshParams);
        if (connection != nullptr) {
            connect(connection, &QSsh::SshConnection::error, this, &LinuxSimulator::onSshConnectionError);
            m_sshConnection = connection;
        }
    }
    return m_sshConnection;
}

void LinuxSimulator::onSshConnectionError()
{
    QSsh::SshConnection *c = qobject_cast<QSsh::SshConnection *>(sender());
    if (c != nullptr)
        qDebug() << c->errorString();
}

void LinuxSimulator::loadSettings()
{
    if (m_settings.isEmpty())
        return;
    QFileInfo info(m_settings);
    m_path = info.absolutePath();
    QSettings settings(m_settings, QSettings::IniFormat);
    m_memory = static_cast<quint8>(settings.value(HandleldTerminal::Constants::Simulator::Linux::MEMORY, m_memory).toUInt());
    m_cpuNumber = static_cast<quint8>(settings.value(HandleldTerminal::Constants::Simulator::Linux::CPUNUMBER, m_cpuNumber).toUInt());
    m_xRes = static_cast<quint16>(settings.value(HandleldTerminal::Constants::Simulator::Linux::X_RESOLUTION, m_xRes).toInt());
    m_yRes = static_cast<quint16>(settings.value(HandleldTerminal::Constants::Simulator::Linux::Y_RESOLUTION, m_yRes).toInt());
    m_sshPort = static_cast<quint16>(settings.value(HandleldTerminal::Constants::Simulator::Linux::SSH_PORT, m_sshPort).toUInt());
    m_userName = settings.value(HandleldTerminal::Constants::Simulator::Linux::USERNAME).toString();
    QByteArray data = settings.value(HandleldTerminal::Constants::Simulator::Linux::PASSWORD).toByteArray();
    QDataStream stream(data);
    stream >> m_password;
}

bool LinuxSimulator::connectUseSsh()
{
    sshConnection();
    if (m_sshConnection == nullptr)
        return false;
    if (m_sshConnection->state() == QSsh::SshConnection::Connected)
        return true;
    QEventLoop loop;
    connect(m_sshConnection, &QSsh::SshConnection::connected, &loop, &QEventLoop::quit);
    connect(m_sshConnection, &QSsh::SshConnection::error, &loop, &QEventLoop::quit);
    m_sshConnection->connectToHost();
    loop.exec();
    return m_sshConnection->state() == QSsh::SshConnection::Connected;
}

}
}
