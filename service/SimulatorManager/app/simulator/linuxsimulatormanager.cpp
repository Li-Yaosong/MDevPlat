#include "linuxsimulatormanager.h"
#include "handleldterminalconstants.h"
#include "linuxsimulator.h"
#include "qemusettings.h"

#include <QSettings>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDirIterator>
#include <QRunnable>
#include <QThreadPool>

namespace HandleldTerminal {
namespace Internal {

LinuxSimulatorManager::LinuxSimulatorManager(QObject *parent)
    : AbstractSimulatorManager (parent)
{

}

class Copy : public QRunnable
{
public:
    Copy(const QString &src, const QString &dest) : _src(src), _dest(dest) {}
private:
    QString _src;
    QString _dest;

    // QRunnable interface
public:
    virtual void run() override
    {
        QFile::copy(_src, _dest);
    }
};

void LinuxSimulatorManager::create(const QVariantMap &params)
{
    QDir dir(qQemu->path());
    if (dir.cd("vm")) {
        QString abi = params.value(HandleldTerminal::Constants::Simulator::ABI).toString();
        QString templateFile = dir.filePath(QString("%1.qcow2").arg(abi));
        QString name = params.value(HandleldTerminal::Constants::Simulator::NAME).toString();
        if (name.isEmpty())
            return;
        QString targetPath = savePath() + "/" + name;
        if (!QDir().mkpath(targetPath))
            return;
        QString targetFile = targetPath + "/" + name + ".qcow2";
        QThreadPool::globalInstance()->start(new Copy(templateFile, targetFile));
        QSettings settings(targetPath + "/config.ini", QSettings::IniFormat);
        settings.setValue(HandleldTerminal::Constants::Simulator::Linux::NAME, name);
        settings.setValue(HandleldTerminal::Constants::Simulator::Linux::ABI, abi);
        LinuxSimulator *sim = new LinuxSimulator;
        sim->setAbi(abi);
        sim->setName(name);
        sim->setSettingsFile(targetPath + "/config.ini");
        m_simulators << sim;
        simulatorAdded(sim);
    }
}

class DeleteDirTask : public QRunnable
{
public:
    DeleteDirTask(const QString &dir) : _dir(dir) {}
private:
    QString _dir;

    // QRunnable interface
public:
    virtual void run() override
    {
        if (_dir.isEmpty())
            return ;
        QDir dir(_dir);
        if (!dir.exists())
            return;
        dir.removeRecursively();
    }
};

void LinuxSimulatorManager::remove(Simulator *sim)
{
    if (sim == nullptr)
        return;
    if (m_simulators.contains(sim)) {
        m_simulators.removeOne(sim);
        // 删除文件
        QString path = savePath();
        QString name = sim->name();
        QThreadPool::globalInstance()->start(new DeleteDirTask(path + "/" + name));
        simulatorAboutToRemoved(sim);
        sim->deleteLater();
    }
}

QString LinuxSimulatorManager::savePath()
{
    QDir home = QDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    static const char handledTerminal[] = ".handledterminal";
    if (!home.exists(handledTerminal)) {
        home.mkdir(handledTerminal);
    }
    if (!home.exists(handledTerminal))
        return QString();
    return home.filePath(handledTerminal);
}

QList<Simulator *> LinuxSimulatorManager::list()
{
    static bool m_read = false;
    if (!m_read) {
        QDirIterator it(savePath(), QStringList() << "*.ini", QDir::NoFilter, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString settingsFile = it.next();
            QSettings settings(settingsFile, QSettings::IniFormat);
            LinuxSimulator *sim = new LinuxSimulator;
            sim->setName(settings.value(HandleldTerminal::Constants::Simulator::Linux::NAME).toString());
            sim->setAbi(settings.value(HandleldTerminal::Constants::Simulator::Linux::ABI).toString());
            sim->setSettingsFile(settingsFile);
            m_simulators << sim;
        }
        m_read = true;
    }
    return m_simulators;
}

}
}
