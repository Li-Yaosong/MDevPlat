#ifndef ANDROIDSIMULATOR_H
#define ANDROIDSIMULATOR_H

#include "simulator.h"

class QProcess;
namespace HandleldTerminal {
namespace Internal {

class AndroidSettings;
class AndroidSimulator : public Simulator
{
    Q_OBJECT
public:
    AndroidSimulator(QObject *parent = nullptr);
    ~AndroidSimulator();

    bool start();

    virtual bool stop();

    virtual bool isRunning() const;

    virtual SimulatorType type() const;

    virtual QVariantMap settings() const;

    virtual void updateSettings(const QVariantMap &map);

    QString emulatorId() const;

protected:
    virtual void loadSettings();
    void obtainId();
    QString nameForEmulatorId(const QString &id) const;
private:
    // sdcard大小,单位为G,默认为4G
    quint8 m_memory;
    // cpu的个数，默认为4个
    quint8 m_cpuNumber;
    // 分辨率x
    quint16 m_xRes;
    // 分辨率y
    quint16 m_yRes;
    mutable QString m_emulatorId;
    AndroidSettings *m_androidSettings;
};
}
}
#endif // ANDROIDSIMULATOR_H
