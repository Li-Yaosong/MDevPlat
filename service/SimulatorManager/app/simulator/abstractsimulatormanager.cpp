#include "abstractsimulatormanager.h"
#include "simulator.h"

namespace HandleldTerminal {
namespace Internal {

AbstractSimulatorManager::AbstractSimulatorManager(QObject *parent) : QObject(parent)
{

}

AbstractSimulatorManager::~AbstractSimulatorManager()
{
    qDeleteAll(m_simulators);
}

bool AbstractSimulatorManager::start(const QString &name)
{
    Simulator *sim = find(name);
    if (sim == nullptr)
        return false;
    return sim->start();
}

Simulator *AbstractSimulatorManager::find(const QString &name) const
{
    auto it = std::find_if(m_simulators.constBegin(), m_simulators.constEnd(), [name](Simulator *sim){
        return sim->name() == name;
    });
    return it != m_simulators.constEnd() ? *it : nullptr;
}

bool AbstractSimulatorManager::exists(const QString &name)
{
    return (find(name) != nullptr);
}
}
}
