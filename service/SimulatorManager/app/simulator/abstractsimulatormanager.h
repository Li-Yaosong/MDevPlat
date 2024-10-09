#ifndef ABSTRACTSIMULATORMANAGER_H
#define ABSTRACTSIMULATORMANAGER_H

#include <QObject>
#include <QList>

namespace HandleldTerminal {
namespace Internal {

class Simulator;

class AbstractSimulatorManager : public QObject
{
    Q_OBJECT
public:
    explicit AbstractSimulatorManager(QObject *parent = nullptr);
    ~AbstractSimulatorManager();

    virtual void create(const QVariantMap &params) = 0;

    virtual void remove(Simulator *sim) = 0;

    virtual bool start(const QString &name);

    virtual QList<Simulator *> list() = 0;

    Simulator *find(const QString &name) const;

    bool exists(const QString &name);

signals:
    void simulatorAdded(Simulator *sim);
    void simulatorAboutToRemoved(Simulator *sim);
public slots:

protected:
    QList<Simulator *> m_simulators;
};
}
}
#endif // ABSTRACTSIMULATORMANAGER_H
