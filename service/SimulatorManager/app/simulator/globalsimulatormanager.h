#ifndef GLOBALSIMULATORMANAGER_H
#define GLOBALSIMULATORMANAGER_H

#include <QObject>
#include <QHash>

#include "handleldterminaldefines.h"

#define qSimulatorManager HandleldTerminal::Internal::GlobalSimulatorManager::instance()

namespace HandleldTerminal {
namespace Internal {

class AbstractSimulatorManager;
class Simulator;

class GlobalSimulatorManager : public QObject
{
    Q_OBJECT
public:

    explicit GlobalSimulatorManager(QObject *parent = nullptr);
    ~GlobalSimulatorManager();

    static GlobalSimulatorManager *instance();

    void create(SimulatorType type, const QVariantMap &params);

    void remove(Simulator *sim);

    Simulator *find(SimulatorType type, const QString &name);

    bool exists(SimulatorType type, const QString &name);

    bool start(SimulatorType type, const QString &name);

    QList<Simulator *> list();

signals:
    void simulatorAdded(Simulator *sim);
    void simulatorAboutToRemoved(Simulator *sim);
private:
    static GlobalSimulatorManager *m_self;
    QHash<SimulatorType, AbstractSimulatorManager *> m_managers;
};
}
}
#endif // GLOBALSIMULATORMANAGER_H
