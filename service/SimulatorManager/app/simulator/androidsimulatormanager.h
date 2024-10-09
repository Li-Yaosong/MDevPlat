#ifndef ANDROIDSIMULATORMANAGER_H
#define ANDROIDSIMULATORMANAGER_H

#include "abstractsimulatormanager.h"

namespace HandleldTerminal {
namespace Internal {

class AndroidSimulatorManager : public AbstractSimulatorManager
{
    Q_OBJECT
public:
    AndroidSimulatorManager(QObject *parent = nullptr);
    ~AndroidSimulatorManager();

    virtual void create(const QVariantMap &params);
    virtual void remove(Simulator *sim);
    virtual QList<Simulator *> list();
};

}
}

#endif // ANDROIDSIMULATORMANAGER_H
