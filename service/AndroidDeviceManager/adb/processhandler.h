#ifndef PROCESSHANDLER_H
#define PROCESSHANDLER_H

#include <QObject>
#include <QStringList>

class QProcess;
class ProcessHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString programName READ programName CONSTANT)
    enum LogLevel
    {
        Info,
        Warning,
        Critical
    };

public:
    ProcessHandler(QObject *parent = nullptr);
    ~ProcessHandler();
    void start(const QString &program, const QStringList &arguments = QStringList());
    void stop();

    bool isRunning() const;
    void writeLog(const QString &log, LogLevel logLevel = LogLevel::Info);

    QString programName() const;
private slots:
    void standardOutput();
    void standardError();

private:
    QProcess *m_process;
    QString m_programName;
};

#endif // PROCESSHANDLER_H
