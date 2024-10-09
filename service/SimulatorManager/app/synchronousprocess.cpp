#include "synchronousprocess.h"

#include <QDebug>
#include <QTimer>
#include <QTextCodec>
#include <QDir>
#include <QThread>

#include <limits.h>
#include <memory>

#ifdef Q_OS_UNIX
#    include <unistd.h>
#endif

enum { defaultMaxHangTimerCount = 10 };

namespace Utils {

class TerminalControllingProcess : public QProcess {
public:
    TerminalControllingProcess() : m_flags(0) {}

    unsigned flags() const { return m_flags; }
    void setFlags(unsigned tc) { m_flags = tc; }

protected:
    void setupChildProcess() override;

private:
    unsigned m_flags;
};

void TerminalControllingProcess::setupChildProcess()
{
#ifdef Q_OS_UNIX
    // Disable terminal by becoming a session leader.
    if (m_flags & SynchronousProcess::UnixTerminalDisabled)
        setsid();
#endif
}

void SynchronousProcessResponse::clear()
{
    result = StartFailed;
    exitCode = -1;
    rawStdOut.clear();
    rawStdErr.clear();
}

QString SynchronousProcessResponse::exitMessage(const QString &binary, int timeoutS) const
{
    switch (result) {
    case Finished:
        return SynchronousProcess::tr("The command \"%1\" finished successfully.").arg(QDir::toNativeSeparators(binary));
    case FinishedError:
        return SynchronousProcess::tr("The command \"%1\" terminated with exit code %2.").arg(QDir::toNativeSeparators(binary)).arg(exitCode);
    case TerminatedAbnormally:
        return SynchronousProcess::tr("The command \"%1\" terminated abnormally.").arg(QDir::toNativeSeparators(binary));
    case StartFailed:
        return SynchronousProcess::tr("The command \"%1\" could not be started.").arg(QDir::toNativeSeparators(binary));
    case Hang:
        return SynchronousProcess::tr("The command \"%1\" did not respond within the timeout limit (%2 s).")
                .arg(QDir::toNativeSeparators(binary)).arg(timeoutS);
    }
    return QString();
}

QByteArray SynchronousProcessResponse::allRawOutput() const
{
    if (!rawStdOut.isEmpty() && !rawStdErr.isEmpty()) {
        QByteArray result = rawStdOut;
        if (!result.endsWith('\n'))
            result += '\n';
        result += rawStdErr;
        return result;
    }
    return !rawStdOut.isEmpty() ? rawStdOut : rawStdErr;
}

QString SynchronousProcessResponse::allOutput() const
{
    const QString out = stdOut();
    const QString err = stdErr();

    if (!out.isEmpty() && !err.isEmpty()) {
        QString result = out;
        if (!result.endsWith('\n'))
            result += '\n';
        result += err;
        return result;
    }
    return !out.isEmpty() ? out : err;
}

QString SynchronousProcessResponse::stdOut() const
{
    return SynchronousProcess::normalizeNewlines(codec->toUnicode(rawStdOut));
}

QString SynchronousProcessResponse::stdErr() const
{
    return SynchronousProcess::normalizeNewlines(codec->toUnicode(rawStdErr));
}

QDebug operator<<(QDebug str, const SynchronousProcessResponse& r)
{
    QDebug nsp = str.nospace();
    nsp << "SynchronousProcessResponse: result=" << r.result << " ex=" << r.exitCode << '\n'
        << r.rawStdOut.size() << " bytes stdout, stderr=" << r.rawStdErr << '\n';
    return str;
}

SynchronousProcessResponse::Result defaultExitCodeInterpreter(int code)
{
    return code ? SynchronousProcessResponse::FinishedError
                : SynchronousProcessResponse::Finished;
}

class ChannelBuffer : public QObject
{
    Q_OBJECT

public:
    void clearForRun();

    QString linesRead();
    void append(const QByteArray &text, bool emitSignals);

    QByteArray rawData;
    QString incompleteLineBuffer; // lines not yet signaled
    QTextCodec *codec = nullptr; // Not owner
    std::unique_ptr<QTextCodec::ConverterState> codecState;
    int rawDataPos = 0;
    bool bufferedSignalsEnabled = false;
    bool firstBuffer = true;

signals:
    void outputBuffered(const QString &text, bool firstTime);
};

void ChannelBuffer::clearForRun()
{
    firstBuffer = true;
    rawDataPos = 0;
    rawData.clear();
    codecState.reset(new QTextCodec::ConverterState);
    incompleteLineBuffer.clear();
}

QString ChannelBuffer::linesRead()
{
    // Convert and append the new input to the buffer of incomplete lines
    const char *start = rawData.constData() + rawDataPos;
    const int len = rawData.size() - rawDataPos;
    incompleteLineBuffer.append(codec->toUnicode(start, len, codecState.get()));
    rawDataPos = rawData.size();

    // Any completed lines in the incompleteLineBuffer?
    const int lastLineIndex = qMax(incompleteLineBuffer.lastIndexOf('\n'),
                                   incompleteLineBuffer.lastIndexOf('\r'));
    if (lastLineIndex == -1)
        return QString();

    // Get completed lines and remove them from the incompleteLinesBuffer:
    const QString lines = SynchronousProcess::normalizeNewlines(incompleteLineBuffer.left(lastLineIndex + 1));
    incompleteLineBuffer = incompleteLineBuffer.mid(lastLineIndex + 1);

    return lines;
}

void ChannelBuffer::append(const QByteArray &text, bool emitSignals)
{
    if (text.isEmpty())
        return;
    rawData += text;
    if (!emitSignals)
        return;

    // Buffered. Emit complete lines?
    if (bufferedSignalsEnabled) {
        const QString lines = linesRead();
        if (!lines.isEmpty()) {
            emit outputBuffered(lines, firstBuffer);
            firstBuffer = false;
        }
    }
}

// ----------- SynchronousProcessPrivate
class SynchronousProcessPrivate {
public:
    void clearForRun();

    QTextCodec *m_codec = QTextCodec::codecForLocale();
    TerminalControllingProcess m_process;
    SynchronousProcessResponse m_result;
    QString m_binary;
    ChannelBuffer m_stdOut;
    ChannelBuffer m_stdErr;
    ExitCodeInterpreter m_exitCodeInterpreter = defaultExitCodeInterpreter;

    int m_hangTimerCount = 0;
    int m_maxHangTimerCount = defaultMaxHangTimerCount;
    bool m_startFailure = false;
    bool m_timeOutMessageBoxEnabled = false;
    bool m_waitingForUser = false;
};

void SynchronousProcessPrivate::clearForRun()
{
    m_hangTimerCount = 0;
    m_stdOut.clearForRun();
    m_stdOut.codec = m_codec;
    m_stdErr.clearForRun();
    m_stdErr.codec = m_codec;
    m_result.clear();
    m_result.codec = m_codec;
    m_startFailure = false;
    m_binary.clear();
}

SynchronousProcess::SynchronousProcess() :
    d(new SynchronousProcessPrivate)
{
    connect(&d->m_process,
            static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &SynchronousProcess::finished);
    connect(&d->m_process, &QProcess::errorOccurred, this, &SynchronousProcess::error);
    connect(&d->m_process, &QProcess::readyReadStandardOutput,
            this, [this]() {
                d->m_hangTimerCount = 0;
                processStdOut(true);
            });
    connect(&d->m_process, &QProcess::readyReadStandardError,
            this, [this]() {
                d->m_hangTimerCount = 0;
                processStdErr(true);
            });
    connect(&d->m_stdOut, &ChannelBuffer::outputBuffered, this, &SynchronousProcess::stdOutBuffered);
    connect(&d->m_stdErr, &ChannelBuffer::outputBuffered, this, &SynchronousProcess::stdErrBuffered);
}

SynchronousProcess::~SynchronousProcess()
{
    disconnect(&d->m_process, 0, this, 0);
    delete d;
}

void SynchronousProcess::setCodec(QTextCodec *c)
{
    if(c == nullptr)
        return;
    d->m_codec = c;
}

QTextCodec *SynchronousProcess::codec() const
{
    return d->m_codec;
}

bool SynchronousProcess::stdOutBufferedSignalsEnabled() const
{
    return d->m_stdOut.bufferedSignalsEnabled;
}

void SynchronousProcess::setStdOutBufferedSignalsEnabled(bool v)
{
    d->m_stdOut.bufferedSignalsEnabled = v;
}

bool SynchronousProcess::stdErrBufferedSignalsEnabled() const
{
    return d->m_stdErr.bufferedSignalsEnabled;
}

void SynchronousProcess::setStdErrBufferedSignalsEnabled(bool v)
{
    d->m_stdErr.bufferedSignalsEnabled = v;
}

QStringList SynchronousProcess::environment() const
{
    return d->m_process.environment();
}

bool SynchronousProcess::timeOutMessageBoxEnabled() const
{
    return d->m_timeOutMessageBoxEnabled;
}

void SynchronousProcess::setTimeOutMessageBoxEnabled(bool v)
{
    d->m_timeOutMessageBoxEnabled = v;
}

void SynchronousProcess::setEnvironment(const QStringList &e)
{
    d->m_process.setEnvironment(e);
}

void SynchronousProcess::setProcessEnvironment(const QProcessEnvironment &environment)
{
    d->m_process.setProcessEnvironment(environment);
}

QProcessEnvironment SynchronousProcess::processEnvironment() const
{
    return d->m_process.processEnvironment();
}

unsigned SynchronousProcess::flags() const
{
    return d->m_process.flags();
}

void SynchronousProcess::setFlags(unsigned tc)
{
    d->m_process.setFlags(tc);
}

void SynchronousProcess::setExitCodeInterpreter(const ExitCodeInterpreter &interpreter)
{
    d->m_exitCodeInterpreter = interpreter;
}

ExitCodeInterpreter SynchronousProcess::exitCodeInterpreter() const
{
    return d->m_exitCodeInterpreter;
}

void SynchronousProcess::setWorkingDirectory(const QString &workingDirectory)
{
    d->m_process.setWorkingDirectory(workingDirectory);
}

QString SynchronousProcess::workingDirectory() const
{
    return d->m_process.workingDirectory();
}

QProcess::ProcessChannelMode SynchronousProcess::processChannelMode () const
{
    return d->m_process.processChannelMode();
}

void SynchronousProcess::setProcessChannelMode(QProcess::ProcessChannelMode m)
{
    d->m_process.setProcessChannelMode(m);
}

SynchronousProcessResponse SynchronousProcess::runBlocking(const QString &binary, const QStringList &args)
{
    d->clearForRun();

    d->m_binary = binary;
    d->m_process.start(binary, args, QIODevice::ReadOnly);
    if (!d->m_process.waitForStarted(d->m_maxHangTimerCount * 1000)) {
        d->m_result.result = SynchronousProcessResponse::StartFailed;
        return d->m_result;
    }
    d->m_process.closeWriteChannel();
    if (d->m_process.waitForFinished(d->m_maxHangTimerCount * 1000)) {
        if (d->m_process.state() == QProcess::Running) {
            d->m_result.result = SynchronousProcessResponse::Hang;
            d->m_process.terminate();
            if (d->m_process.waitForFinished(1000) && d->m_process.state() == QProcess::Running) {
                d->m_process.kill();
                d->m_process.waitForFinished(1000);
            }
        }
    }

    if (d->m_process.state() != QProcess::NotRunning)
        return d->m_result;

    d->m_result.exitCode = d->m_process.exitCode();
    if (d->m_result.result == SynchronousProcessResponse::StartFailed) {
        if (d->m_process.exitStatus() != QProcess::NormalExit)
            d->m_result.result = SynchronousProcessResponse::TerminatedAbnormally;
        else
            d->m_result.result = (exitCodeInterpreter())(d->m_result.exitCode);
    }
    processStdOut(false);
    processStdErr(false);

    d->m_result.rawStdOut = d->m_stdOut.rawData;
    d->m_result.rawStdErr = d->m_stdErr.rawData;

    return d->m_result;
}

bool SynchronousProcess::stopProcess(QProcess &p)
{
    if (p.state() == QProcess::NotRunning)
        return true;
    p.terminate();
    if (p.waitForFinished(300) && p.state() == QProcess::Running)
        return true;
    p.kill();
    return p.waitForFinished(300) || p.state() == QProcess::NotRunning;
}

bool SynchronousProcess::terminate()
{
    return stopProcess(d->m_process);
}

void SynchronousProcess::finished(int exitCode, QProcess::ExitStatus e)
{
    d->m_hangTimerCount = 0;

    switch (e) {
    case QProcess::NormalExit:
        d->m_result.result = d->m_exitCodeInterpreter(exitCode);
        d->m_result.exitCode = exitCode;
        break;
    case QProcess::CrashExit:
        // Was hang detected before and killed?
        if (d->m_result.result != SynchronousProcessResponse::Hang)
            d->m_result.result = SynchronousProcessResponse::TerminatedAbnormally;
        d->m_result.exitCode = -1;
        break;
    }
}

void SynchronousProcess::error(QProcess::ProcessError e)
{
    Q_UNUSED(e);
    d->m_hangTimerCount = 0;
    // Was hang detected before and killed?
    if (d->m_result.result != SynchronousProcessResponse::Hang)
        d->m_result.result = SynchronousProcessResponse::StartFailed;
    d->m_startFailure = true;
}

void SynchronousProcess::processStdOut(bool emitSignals)
{
    // Handle binary data
    d->m_stdOut.append(d->m_process.readAllStandardOutput(), emitSignals);
}

void SynchronousProcess::processStdErr(bool emitSignals)
{
    // Handle binary data
    d->m_stdErr.append(d->m_process.readAllStandardError(), emitSignals);
}

QString SynchronousProcess::normalizeNewlines(const QString &text)
{
    QString res = text;
    res.replace(QLatin1String("\r\n"), QLatin1String("\n"));
    return res;
}

} // namespace Utils

#include "synchronousprocess.moc"
