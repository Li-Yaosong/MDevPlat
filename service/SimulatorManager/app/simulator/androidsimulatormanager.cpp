#include "androidsimulatormanager.h"
#include "androidsimulator.h"
#include "androidsettings.h"
#include "handleldterminalconstants.h"
#include "androidsdksettings.h"

#include "synchronousprocess.h"

#include <QSettings>
#include <QDebug>
#include <QFileInfo>

namespace HandleldTerminal {
namespace Internal {

AndroidSimulatorManager::AndroidSimulatorManager(QObject *parent)
    : AbstractSimulatorManager (parent)
{

}

AndroidSimulatorManager::~AndroidSimulatorManager()
{

}

static bool createAvdCommand(const QVariantMap &info)
{
    if (info.isEmpty()) {
        qWarning() << "Cannot create AVD. Invalid input.";
        return false;
    }

    QString name = info.value(HandleldTerminal::Constants::Simulator::NAME).toString();
    QString abi = info.value(HandleldTerminal::Constants::Simulator::ABI).toString();

    QStringList arguments({"create", "avd", "-n", name});

    if (!abi.isEmpty()) {
        QString target = qAndroid->targetSdkPath(abi);
        if (!target.isEmpty()) {
            arguments << "-k" << qAndroid->targetSdkPath(abi);
        }
        else {
            return false;
        }
    } else {
        return false;
    }

    quint64 sdcardSize = info.value(HandleldTerminal::Constants::Simulator::MEMORY).toUInt();
    if (sdcardSize > 0)
        arguments << "-c" << QString::fromLatin1("%1G").arg(sdcardSize);

    QProcess proc;
    proc.start(qAndroid->avdManager(), arguments);
    if (!proc.waitForStarted()) {
        qWarning() << QString("Could not start process \"%1 %2\"")
                .arg(qAndroid->avdManager(), arguments.join(' '));
        return false;
    }

    proc.write(QByteArray("yes\n")); // yes to "Do you wish to create a custom hardware profile"

    QString errorOutput;
    QByteArray question;
    while (errorOutput.isEmpty()) {
        proc.waitForReadyRead(500);
        question += proc.readAllStandardOutput();
        if (question.endsWith(QByteArray("]:"))) {
            // truncate to last line
            int index = question.lastIndexOf(QByteArray("\n"));
            if (index != -1)
                question = question.mid(index);
            if (question.contains("hw.gpu.enabled"))
                proc.write(QByteArray("yes\n"));
            else
                proc.write(QByteArray("\n"));
            question.clear();
        }

        errorOutput = QString::fromLocal8Bit(proc.readAllStandardError());
        if (proc.state() != QProcess::Running)
            break;
    }

    // Kill the running process.
    if (proc.state() != QProcess::NotRunning) {
        proc.terminate();
        if (!proc.waitForFinished(3000))
            proc.kill();
    }

    return true;
}

void AndroidSimulatorManager::create(const QVariantMap &params)
{
    if (createAvdCommand(params)) {
        QList<Simulator *> list = qAndroid->simulators();
        for (int i = 0; i < list.count(); i++) {
            Simulator *sim = list.at(i);
            if (find(sim->name()))
                continue;
            emit simulatorAdded(sim);
            m_simulators << sim;
        }
    }
}

void AndroidSimulatorManager::remove(Simulator *sim)
{
    if (sim == nullptr)
        return;
    if (m_simulators.contains(sim)) {
        m_simulators.removeOne(sim);
        // 删除文件
        QProcess proc;
        QStringList arguments = {"delete", "avd", "-n", sim->name()};
        proc.start(qAndroid->avdManager(), arguments);
        proc.waitForStarted();
        proc.waitForFinished();
        simulatorAboutToRemoved(sim);
        sim->deleteLater();
    }
}

QList<Simulator *> AndroidSimulatorManager::list()
{
    static bool m_read = false;
    if (!m_read) {
        m_simulators = qAndroid->simulators();
        m_read = true;
    }
    return m_simulators;
}

}
}
