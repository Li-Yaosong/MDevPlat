#include "processhandler.h"

#include <QProcess>
#include <QDebug>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
ProcessHandler::ProcessHandler(QObject *parent)
    : QObject(parent)
    , m_process(new QProcess(this))
{
    // 连接信号到槽函数，用于实时读取标准输出
    QObject::connect(m_process, &QProcess::readyReadStandardOutput, this, &ProcessHandler::standardOutput);

    // 连接信号到槽函数，用于实时读取标准错误
    QObject::connect(m_process, &QProcess::readyReadStandardError, this, &ProcessHandler::standardError);

    // 连接 aboutToQuit 信号以在程序退出时终止子进程
    QObject::connect(qApp, &QCoreApplication::aboutToQuit, m_process, &QProcess::terminate);
    QDir dir(QCoreApplication::applicationDirPath() + "/log");
    if (!dir.exists()) {
        dir.mkpath(".");  // 创建目录，包括必要的父目录
    }
}

ProcessHandler::~ProcessHandler()
{
    qDebug() << "ProcessHandler::~ProcessHandler()";
    if (m_process && m_process->state() == QProcess::Running) {
        m_process->terminate();
        m_process->waitForFinished();
    }
    delete m_process;
    m_process = nullptr;
}

void ProcessHandler::start(const QString &program, const QStringList &arguments)
{
    m_process->start(program, arguments);
    if (!m_process->waitForStarted()) {
        writeLog("Failed to start the process."+ program, LogLevel::Critical);
    }
    else{
        writeLog("start the process."+ program, LogLevel::Info);
    }

    m_programName = QFileInfo(m_process->program()).fileName();
}

void ProcessHandler::stop()
{
    if (m_process->state() == QProcess::Running) {
        m_process->terminate();
        if (!m_process->waitForFinished()) {
            writeLog("Failed to stop the process.", LogLevel::Critical);
        }
        else{
            writeLog("stop the process." + m_process->program(), LogLevel::Info);
        }
    }
}

bool ProcessHandler::isRunning() const
{
    return m_process->state() == QProcess::Running;
}

void ProcessHandler::writeLog(const QString &log, LogLevel logLevel)
{
    QFile file(QCoreApplication::applicationDirPath() + "/log/" + programName()+"_"
               + QString::number(QCoreApplication::applicationPid())+"_"
               + QString::number(m_process->processId()) + ".log");
    if (file.open(QIODevice::Append | QIODevice::Text | QIODevice::ReadWrite)) {
        QTextStream out(&file);
        out << log << endl;
        switch (logLevel) {
        case Info:
            qInfo() << log;
            break;
        case Warning:
            qWarning() << log;
            break;
        case Critical:
            qCritical() << log;
            break;
        default:
            break;
        }
        file.close();
    } else {
        qCritical() << "Failed to open log file.";
    }
}

QString ProcessHandler::programName() const
{
    return m_programName;
}

void ProcessHandler::standardOutput()
{
    QByteArray output = m_process->readAllStandardOutput();
    writeLog(output);
}

void ProcessHandler::standardError()
{
    QByteArray errorOutput = m_process->readAllStandardError();
    writeLog(errorOutput, LogLevel::Warning);
}
