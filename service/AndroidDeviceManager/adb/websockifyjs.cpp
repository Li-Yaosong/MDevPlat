#include "websockifyjs.h"
#include "processhandler.h"
#include "androiddebugbridge.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>

WebsockifyJS *WebsockifyJS::m_instance = nullptr;
WebsockifyJS::WebsockifyJS(QObject *parent)
    : QObject{parent}
{
    QDir dir(QCoreApplication::applicationDirPath());
    // 判断dir是否存在android文件夹，如果存在则设置m_path为android文件夹的绝对路径
    if (dir.exists("nodejs")) {
#ifdef Q_OS_WIN
        m_path = dir.absoluteFilePath("nodejs/node.exe");
#else
        m_path = dir.absoluteFilePath("nodejs/bin/node");
#endif
    }
    if (dir.exists("websockify-js")) {
        m_websockifyPath = dir.absoluteFilePath("websockify-js/websockify/websockify.js");
    }
}

WebsockifyJS *WebsockifyJS::instance()
{
    if (!m_instance) {
        m_instance = new WebsockifyJS();
    }
    return m_instance;
}

QString WebsockifyJS::path() const
{
    return m_path;
}

QString WebsockifyJS::websockifyPath() const
{
    return m_websockifyPath;
}

QString WebsockifyJS::start(const QString &target)
{
    QString wsPort = "15555";
    // 如果m_path为空，则返回
    if (m_path.isEmpty()) {
        return "";
    }

    // 如果m_websockifyPath为空，则返回
    if (m_websockifyPath.isEmpty()) {
        return "";
    }
    // 创建ProcessHandler对象
    if (m_process == nullptr) {
        m_process = new ProcessHandler(this);
    }
    if (m_process->isRunning()) {
        m_process->stop();
    }
    QString deviceIp = target.startsWith("emulator-") ? "localhost" : adb->wlanInfo(target).inetAddr;
    adb->startTcpIp(target);
    m_process->start(path(), {websockifyPath(), wsPort, deviceIp +":5555"});
    adb->killAdbServer();
    return wsPort;
}

