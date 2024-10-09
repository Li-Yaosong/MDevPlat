#include "simulator.h"

#include <QDebug>

namespace HandleldTerminal {
namespace Internal {

Simulator::Simulator(QObject *parent)
    : QObject (parent)
    , m_process(nullptr)
{

}

Simulator::~Simulator()
{

}

QString Simulator::name() const
{
    return m_name;
}

void Simulator::setName(const QString &name)
{
    m_name = name;
}

bool Simulator::isValid() const
{
    return (!m_name.isEmpty() && !m_abi.isEmpty());
}

void Simulator::setSettingsFile(const QString &file)
{
    m_settings = file;
    loadSettings();
}

bool Simulator::stop()
{
    if (m_process != nullptr) {
        if (m_process->state() == QProcess::Running) {
            m_process->close();
            m_process->waitForFinished();
            delete m_process;
        }
    }
    return true;
}

bool Simulator::isRunning() const
{
    if (m_process == nullptr) {
        return false;
    }
    return m_process->state() == QProcess::Running;
}

QString Simulator::abi() const
{
    return m_abi;
}

void Simulator::setAbi(const QString &abi)
{
    m_abi = abi;
}

CpuType Simulator::cpu() const
{
    if (m_abi == "x86_64")
        return HandleldTerminal::x86_64;
    else if (m_abi == "aarch64")
        return HandleldTerminal::aarch64;
    return HandleldTerminal::unkonwn;
}

void Simulator::setError(const QString &error)
{
    m_errorString = error;
    emit errorOuccured(m_errorString);
}

QString Simulator::errorString() const
{
    return m_errorString;
}

static void processFinished(int exitCode, QProcess *p, Simulator *sim)
{
    if (p == nullptr || sim == nullptr)
        return;
    if (exitCode) {
        emit sim->setError(QString::fromLatin1(p->readAll()));
    }
}


bool Simulator::startProcess(const QString &exe, const QStringList &environments, const QStringList &arguments)
{
    if (m_process == nullptr) {
        m_process = new QProcess;
        m_process->setReadChannelMode(QProcess::MergedChannels);
        m_process->setProgram(exe);
        m_process->setEnvironment(environments);
        connect(m_process, &QProcess::errorOccurred, this, &Simulator::onProcessErrorOccurred);
        QObject::connect(m_process,
                         static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                         this,
                         std::bind(&processFinished, std::placeholders::_1, m_process, this));
    }
    m_process->setArguments(arguments);
    qDebug() << "start simulator with args" << m_process->arguments();
    m_process->start();
    return m_process->waitForStarted();
}

void Simulator::onProcessErrorOccurred(QProcess::ProcessError error)
{
    switch (error) {
    case QProcess::Crashed:
        qWarning() << "The process crashed some time after starting successfully.";
        break;
    case QProcess::FailedToStart:
        qWarning() << "The process failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program.";
        break;
    case QProcess::Timedout:
        qWarning() << "The last waitFor...() function timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again.";
        break;
    case QProcess::WriteError:
        qWarning() << "An error occurred when attempting to write to the process. For example, the process may not be running, or it may have closed its input channel.";
        break;
    case QProcess::ReadError:
        qWarning() << "An error occurred when attempting to read from the process. For example, the process may not be running.";
        break;
    default:
        qWarning() << "An unknown error occurred. This is the default return value of error().";
    }
}

}
}
