#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "handleldterminaldefines.h"

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QProcess>

namespace HandleldTerminal {
namespace Internal {

class Simulator : public QObject
{
    Q_OBJECT
public:
    Simulator(QObject *parent = nullptr);
    virtual ~Simulator();

    QString name() const;
    void setName(const QString &name);

    bool isValid() const;

    void setSettingsFile(const QString &file);

    virtual bool start() = 0;

    virtual bool stop();

    virtual bool isRunning() const;

    QString abi() const;
    void setAbi(const QString &abi);

    CpuType cpu() const;

    virtual SimulatorType type() const = 0;

    virtual QVariantMap settings() const = 0;

    virtual void updateSettings(const QVariantMap &map) = 0;

    void setError(const QString &error);
    QString errorString() const;
signals:
    void errorOuccured(const QString &errorString);
protected:
    virtual void loadSettings() = 0;
    virtual bool startProcess(const QString &exe, const QStringList &environments, const QStringList &arguments = QStringList());
    QString m_settings;
private:
    void onProcessErrorOccurred(QProcess::ProcessError error);
private:
    QString m_name;
    QString m_abi;
    QString m_errorString;
    QProcess *m_process;
};
}
}
#endif // SIMULATOR_H
