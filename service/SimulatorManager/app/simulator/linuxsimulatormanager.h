#ifndef LINUXSIMULATORMANAGER_H
#define LINUXSIMULATORMANAGER_H

#include "abstractsimulatormanager.h"

namespace HandleldTerminal {
namespace Internal {

class LinuxSimulatorManager : public AbstractSimulatorManager
{
    Q_OBJECT
public:
    LinuxSimulatorManager(QObject *parent = nullptr);

    virtual void create(const QVariantMap &params);

    virtual void remove(Simulator *sim);

    virtual QString savePath();

    virtual QList<Simulator *> list();
};
}
}
#endif // LINUXSIMULATORMANAGER_H
