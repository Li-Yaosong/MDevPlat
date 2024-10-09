#pragma once

#include <QProcess>
#include <QSharedPointer>
#include <QTextCodec>

#include <functional>

QT_FORWARD_DECLARE_CLASS(QDebug)

namespace Utils {

class SynchronousProcessPrivate;

class SynchronousProcessResponse
{
public:
    enum Result {
        Finished,
        FinishedError,
        TerminatedAbnormally,
        StartFailed,
        Hang
    };

    void clear();

    QString exitMessage(const QString &binary, int timeoutS) const;

    QByteArray allRawOutput() const;
    QString allOutput() const;

    QString stdOut() const;
    QString stdErr() const;

    Result result = StartFailed;
    int exitCode = -1;

    QByteArray rawStdOut;
    QByteArray rawStdErr;
    QTextCodec *codec = QTextCodec::codecForLocale();
};

QDebug operator<<(QDebug str, const SynchronousProcessResponse &);

using ExitCodeInterpreter = std::function<SynchronousProcessResponse::Result(int /*exitCode*/)>;
SynchronousProcessResponse::Result defaultExitCodeInterpreter(int code);

class SynchronousProcess : public QObject
{
    Q_OBJECT
public:
    enum Flags {
        UnixTerminalDisabled = 0x1
    };

    SynchronousProcess();
    ~SynchronousProcess() override;

    void setCodec(QTextCodec *c);
    QTextCodec *codec() const;

    QProcess::ProcessChannelMode processChannelMode () const;
    void setProcessChannelMode(QProcess::ProcessChannelMode m);

    bool stdOutBufferedSignalsEnabled() const;
    void setStdOutBufferedSignalsEnabled(bool);

    bool stdErrBufferedSignalsEnabled() const;
    void setStdErrBufferedSignalsEnabled(bool);

    bool timeOutMessageBoxEnabled() const;
    void setTimeOutMessageBoxEnabled(bool);

    QStringList environment() const;
    void setEnvironment(const QStringList &);

    void setProcessEnvironment(const QProcessEnvironment &environment);
    QProcessEnvironment processEnvironment() const;

    void setWorkingDirectory(const QString &workingDirectory);
    QString workingDirectory() const;

    unsigned flags() const;
    void setFlags(unsigned);

    void setExitCodeInterpreter(const ExitCodeInterpreter &interpreter);
    ExitCodeInterpreter exitCodeInterpreter() const;

    SynchronousProcessResponse runBlocking(const QString &binary, const QStringList &args);

    static bool stopProcess(QProcess &p);

    static QString normalizeNewlines(const QString &text);

signals:
    void stdOutBuffered(const QString &lines, bool firstTime);
    void stdErrBuffered(const QString &lines, bool firstTime);

public slots:
    bool terminate();

private:
    void slotTimeout();
    void finished(int exitCode, QProcess::ExitStatus e);
    void error(QProcess::ProcessError);
    void processStdOut(bool emitSignals);
    void processStdErr(bool emitSignals);

    SynchronousProcessPrivate *d;
};

} // namespace Utils
