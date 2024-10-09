#ifndef LINUXSIMULATOR_H
#define LINUXSIMULATOR_H

#include "simulator.h"

#include <QProcess>

namespace QSsh {
class SshConnection;
}

namespace HandleldTerminal {
namespace Internal {

class LinuxSimulator : public Simulator
{
    Q_OBJECT
public:
    LinuxSimulator(QObject *parent = nullptr);
    ~LinuxSimulator();

    bool start();

    virtual bool stop();

    virtual bool isRunning() const;

    virtual quint16 sshPort() const;

    virtual SimulatorType type() const;

    virtual QVariantMap settings() const;
    virtual void updateSettings(const QVariantMap &map);

    QSsh::SshConnection *sshConnection();

protected:
    virtual void loadSettings();

private:
    bool connectUseSsh();
private slots:
    void onSshConnectionError();
private:
    // 虚拟机所在的位置
    QString m_path;
    // 内存大小,单位未G,默认为4G
    quint8 m_memory;
    // cpu的个数，默认为4个
    quint8 m_cpuNumber;
    // 分辨率x
    quint16 m_xRes;
    // 分辨率y
    quint16 m_yRes;
    // ssh 端口
    quint16 m_sshPort;
    // ssh登录用户名
    QString m_userName;
    // ssh登录密码
    QString m_password;
    //
    QSsh::SshConnection *m_sshConnection = nullptr;
    //
    bool m_inStop = false;

};
}
}
#endif // LINUXSIMULATOR_H
