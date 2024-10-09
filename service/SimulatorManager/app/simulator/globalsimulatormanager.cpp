#include "globalsimulatormanager.h"
#include "androidsimulatormanager.h"
#include "linuxsimulatormanager.h"
#include "simulator.h"

#include <QDebug>

namespace HandleldTerminal {
namespace Internal {

GlobalSimulatorManager *GlobalSimulatorManager::m_self = nullptr;

GlobalSimulatorManager::GlobalSimulatorManager(QObject *parent) : QObject(parent)
{
    Q_ASSERT(m_self == nullptr);
    m_self = this;
    AbstractSimulatorManager *manager = new AndroidSimulatorManager(this);
    connect(manager, &AbstractSimulatorManager::simulatorAdded, this, &GlobalSimulatorManager::simulatorAdded);
    connect(manager, &AbstractSimulatorManager::simulatorAboutToRemoved, this, &GlobalSimulatorManager::simulatorAboutToRemoved);
    m_managers.insert(Android, manager);
    manager = new LinuxSimulatorManager(this);
    connect(manager, &AbstractSimulatorManager::simulatorAdded, this, &GlobalSimulatorManager::simulatorAdded);
    connect(manager, &AbstractSimulatorManager::simulatorAboutToRemoved, this, &GlobalSimulatorManager::simulatorAboutToRemoved);
    m_managers.insert(Linux, manager);
}

GlobalSimulatorManager::~GlobalSimulatorManager()
{
    m_self = nullptr;
}

GlobalSimulatorManager *GlobalSimulatorManager::instance()
{
    return m_self;
}

void GlobalSimulatorManager::create(SimulatorType type, const QVariantMap &params)
{
    AbstractSimulatorManager *manager = m_managers.value(type);
    if (manager == nullptr) {
        qWarning() << "unsupported simulator type";
        return;
    }
    manager->create(params);
}

void GlobalSimulatorManager::remove(Simulator *sim)
{
    if (sim == nullptr)
        return;
    AbstractSimulatorManager *manager = m_managers.value(sim->type());
    manager->remove(sim);
}

Simulator *GlobalSimulatorManager::find(SimulatorType type, const QString &name)
{
    AbstractSimulatorManager *manager = m_managers.value(type);
    if (manager == nullptr) {
        qWarning() << "unsupported simulator type";
        return nullptr;
    }
    return manager->find(name);
}

bool GlobalSimulatorManager::exists(SimulatorType type, const QString &name)
{
    AbstractSimulatorManager *manager = m_managers.value(type);
    if (manager == nullptr) {
        qWarning() << "unsupported simulator type";
        return false;
    }
    return manager->exists(name);
}

bool GlobalSimulatorManager::start(SimulatorType type, const QString &name)
{
    AbstractSimulatorManager *manager = m_managers.value(type);
    if (manager == nullptr) {
        qWarning() << "unsupported simulator type";
        return false;
    }
    return manager->start(name);
}

QList<Simulator *> GlobalSimulatorManager::list()
{
    QList<Simulator *> result;
    QList<AbstractSimulatorManager *> managers = m_managers.values();
    for (int i = 0; i < managers.count(); ++i) {
        result << managers.at(i)->list();
    }

    return result;
}

}
}
