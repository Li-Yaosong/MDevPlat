#include "androiddebugbridge.h"

#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>
#include <QProcess>
#include "synchronousprocess.h"
#include <QDebug>
#include <QRegularExpression>
#include <QSize>
AndroidDebugBridge *AndroidDebugBridge::m_instance = nullptr;
AndroidDebugBridge::AndroidDebugBridge(QObject *parent)
    : QObject{parent}
{
    QDir dir(QCoreApplication::applicationDirPath());
    // 判断dir是否存在android文件夹，如果存在则设置m_path为android文件夹的绝对路径
    if (dir.exists("Android")) {
        m_path = dir.absoluteFilePath("Android/Sdk/platform-tools");
    } else {
        //获取Local路径
        QString genericDataLocation = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
        m_path = genericDataLocation + "/Android/Sdk/platform-tools";
    }
}
QString filePath(const QString &dir, const QString &file, const QString &suffix)
{
    QString name = file;
#ifdef Q_OS_WIN
    name.append(suffix);
#else
    Q_UNUSED(suffix)
#endif

    QDir _dir(dir);
    if (_dir.exists(name))
        return _dir.filePath(name);
    return QString();
}
const QRegularExpression& regex(const QString &pattern)
{
    static const QRegularExpression regex(pattern);
    return regex;
}
AndroidDebugBridge *AndroidDebugBridge::instance()
{
    if (!m_instance)
    {
        m_instance = new AndroidDebugBridge;
    }
    return m_instance;
}

QString AndroidDebugBridge::path() const
{
    return m_path;
}

QString AndroidDebugBridge::adbPath() const
{
#ifdef Q_OS_WIN
    QString suffix = ".exe";
#else
    QString suffix = "";
#endif
    return filePath(m_path, "adb", suffix);
}

void AndroidDebugBridge::startAdbServer() const
{
    killAdbServer();
    NetworkCardInfo info;
    Utils::SynchronousProcess proc;
    proc.setTimeOutMessageBoxEnabled(true);
    Utils::SynchronousProcessResponse response = proc.runBlocking(adbPath(),
                                                                  {"start-server"});
    if (response.result != Utils::SynchronousProcessResponse::Finished) {
        qWarning() << response.stdErr();
    }
}

void AndroidDebugBridge::killAdbServer() const
{
    NetworkCardInfo info;
    Utils::SynchronousProcess proc;
    proc.setTimeOutMessageBoxEnabled(true);
    Utils::SynchronousProcessResponse response = proc.runBlocking(adbPath(),
                                                                  {"kill-server"});
    if (response.result != Utils::SynchronousProcessResponse::Finished) {
        qWarning() << response.stdErr();
    }
}

NetworkCardInfo AndroidDebugBridge::parseNetworkCardInfo(const QString &serialNum, const QString &name) const
{
    // startAdbServer();
    NetworkCardInfo info;
    Utils::SynchronousProcess proc;
    proc.setTimeOutMessageBoxEnabled(true);
    Utils::SynchronousProcessResponse response = proc.runBlocking(adbPath(),
                                                                  {"-s", serialNum, "shell", "ip", "addr", "show", name});
    if (response.result != Utils::SynchronousProcessResponse::Finished) {
        qWarning() << response.stdErr();
        return info;
    }
    info.name = name;
    QString output = response.allOutput();

    // 正则表达式：用于匹配 IPv4 和 IPv6 地址
    static const QRegularExpression ipv4Regex(R"(inet\s+(\d+\.\d+\.\d+\.\d+))");
    // 匹配并提取 IPv4 地址
    QRegularExpressionMatch ipv4Match = ipv4Regex.match(output);
    if (ipv4Match.hasMatch()) {
        QString ipv4Address = ipv4Match.captured(1);
        info.inetAddr = ipv4Address;
        // qDebug() << "IPv4 Address:" << ipv4Address;
    } else {
        qDebug() << "No IPv4 address found!";
    }

    // 正则表达式：用于匹配 IPv6 地址和作用域
    static const QRegularExpression ipv6Regex(R"(inet6\s+([a-fA-F0-9:]+)\s+scope\s+(\w+))");

    // 匹配并提取所有 IPv6 地址及其作用域
    QRegularExpressionMatchIterator ipv6Iterator = ipv6Regex.globalMatch(output);

    while (ipv6Iterator.hasNext()) {
        QRegularExpressionMatch match = ipv6Iterator.next();
        QString ipv6Address = match.captured(1);
        QString scope = match.captured(2);

        // 根据作用域分类
        if (scope == "global") {
            info.inet6AddrGlobal<< ipv6Address;
        } else if (scope == "site") {
            info.inet6AddrSite << ipv6Address;
        } else if (scope == "link") {
            info.inet6AddrLink << ipv6Address;
        }
    }


    // qDebug() << output;
    // QStringList lines = output.split("\n", QString::SkipEmptyParts);
    // // 遍历每一行并进行解析
    // QRegularExpressionMatch match;
    // for (const QString &line : qAsConst(lines)) {

        // if (line.startsWith("eth0") || line.startsWith("wlan0")) {
        //     info.name = line.split(" ").first();
        // } else if (line.contains("inet addr")) {
        //     // 解析IPv4地址、广播地址和子网掩码
        //     QRegularExpressionMatch match = regex(R"(inet addr:(\S+)\s+Bcast:(\S+)\s+Mask:(\S+))").match(line);
        //     if (match.hasMatch()) {
        //         info.inetAddr = match.captured(1);
        //         info.bcast = match.captured(2);
        //         info.mask = match.captured(3);
        //     }
        // } else if (line.contains("inet6 addr") && line.contains("Scope: Site")) {
        //     // 解析IPv6站点范围地址
        //     static const QRegularExpression regex(R"(inet6 addr:\s*(\S+)\s+Scope: Site)");
        //     QRegularExpressionMatch match = regex.match(line);
        //     if (match.hasMatch()) {
        //         info.inet6AddrSite.append(match.captured(1));
        //     }
        // } else if (line.contains("inet6 addr") && line.contains("Scope: Link")) {
        //     // 解析IPv6链路范围地址
        //     static const QRegularExpression regex(R"(inet6 addr:\s*(\S+)\s+Scope: Link)");
        //     QRegularExpressionMatch match = regex.match(line);
        //     if (match.hasMatch()) {
        //         info.inet6AddrLink.append(match.captured(1));
        //     }
        // } else if (line.contains("inet6 addr") && line.contains("Scope: Global")) {
        //     // 解析IPv6链路范围地址
        //     static const QRegularExpression regex(R"(inet6 addr:\s*(\S+)\s+Scope: Global)");
        //     QRegularExpressionMatch match = regex.match(line);
        //     if (match.hasMatch()) {
        //         info.inet6AddrGlobal.append(match.captured(1));
        //     }
        // } else if (line.contains("UP") && line.contains("MTU")) {
        //     // 解析接口状态和MTU值
        //     QRegularExpressionMatch match = regex(R"((UP BROADCAST RUNNING MULTICAST)\s+MTU:(\d+))").match(line);
        //     if (match.hasMatch()) {
        //         info.status = match.captured(1);
        //         info.mtu = match.captured(2).toInt();
        //     }
        // } else if (line.contains("RX packets")) {
        //     // 解析接收的数据包和字节数
        //     QRegularExpressionMatch match = regex(R"(RX packets:(\d+).+RX bytes:(\d+))").match(line);
        //     if (match.hasMatch()) {
        //         info.rxPackets = match.captured(1).toInt();
        //         info.rxBytes = match.captured(2).toLongLong();
        //     }
        // } else if (line.contains("TX packets")) {
        //     // 解析发送的数据包和字节数
        //     QRegularExpressionMatch match = regex(R"(TX packets:(\d+).+TX bytes:(\d+))").match(line);
        //     if (match.hasMatch()) {
        //         info.txPackets = match.captured(1).toInt();
        //         info.txBytes = match.captured(2).toLongLong();
        //     }
        // }
    // }
    // killAdbServer();
    return info;
}

DeviceList AndroidDebugBridge::devices() const
{
    startAdbServer();
    DeviceList devices;
    Utils::SynchronousProcess proc;
    proc.setTimeOutMessageBoxEnabled(true);
    Utils::SynchronousProcessResponse response = proc.runBlocking(adbPath(), {"devices"});
    if (response.result != Utils::SynchronousProcessResponse::Finished) {
        qWarning() << response.stdErr();
        return DeviceList();
    }
    QString output = response.allOutput();
    // qDebug() << output;
    QStringList lines = output.split("\n", QString::SkipEmptyParts);
    // qDebug() << lines;
    for (int i = 1; i < lines.size(); ++i) {
        // qDebug() << lines.at(i);
        Device device;
        device.serialNum = lines.at(i).split("\t").first();
        device.status = lines.at(i).split("\t").last();
        if (!device.serialNum.contains(":")) {
            devices.append(device);
        }
    }
    // killAdbServer();
    return devices;
}

QString AndroidDebugBridge::deviceModel(const QString &serialNum) const
{
    // startAdbServer();
    Utils::SynchronousProcess proc;
    proc.setTimeOutMessageBoxEnabled(true);
    Utils::SynchronousProcessResponse response = proc.runBlocking(adbPath(),
                                                                  {"-s", serialNum, "shell", "getprop", "ro.product.model"});
    if (response.result != Utils::SynchronousProcessResponse::Finished) {
        qWarning() << response.stdErr();
        return QString();
    }
    // killAdbServer();
    return response.stdOut().trimmed();
}

QString AndroidDebugBridge::androidVersion(const QString &serialNum) const
{
    // startAdbServer();
    Utils::SynchronousProcess proc;
    proc.setTimeOutMessageBoxEnabled(true);
    Utils::SynchronousProcessResponse response = proc.runBlocking(adbPath(),
                                                                  {"-s", serialNum, "shell", "getprop", "ro.build.version.release"});
    if (response.result != Utils::SynchronousProcessResponse::Finished) {
        qWarning() << response.stdErr();
        return "null";
    }
    // killAdbServer();
    return response.stdOut().trimmed();
}

QString AndroidDebugBridge::cpuAbi(const QString &serialNum) const
{
    // startAdbServer();
    Utils::SynchronousProcess proc;
    proc.setTimeOutMessageBoxEnabled(true);
    Utils::SynchronousProcessResponse response = proc.runBlocking(adbPath(),
                                                                  {"-s", serialNum, "shell", "getprop", "ro.product.cpu.abi"});
    if (response.result != Utils::SynchronousProcessResponse::Finished) {
        qWarning() << response.stdErr();
        return QString();
    }
    // killAdbServer();
    return response.stdOut().trimmed();
}

BatteryStatus AndroidDebugBridge::batteryStatus(const QString &serialNum) const
{
    // startAdbServer();
    BatteryStatus status;
    Utils::SynchronousProcess proc;
    proc.setTimeOutMessageBoxEnabled(true);
    Utils::SynchronousProcessResponse response = proc.runBlocking(adbPath(),
                                                                  {"-s", serialNum, "shell", "dumpsys", "battery"});
    if (response.result != Utils::SynchronousProcessResponse::Finished) {
        qWarning() << response.stdErr();
        return status;
    }
    QString output = response.allOutput();
    QStringList lines = output.split("\n", QString::SkipEmptyParts);
    for (const QString &line : lines) {
        if (line.contains("AC powered")) {
            status.acPowered = line.contains("true");
        } else if (line.contains("USB powered")) {
            status.usbPowered = line.contains("true");
        } else if (line.contains("Wireless powered")) {
            status.wirelessPowered = line.contains("true");
        } else if (line.contains("Max charging current")) {
            status.maxChargingCurrent = line.split(": ").last().toInt();
        } else if (line.contains("Max charging voltage")) {
            status.maxChargingVoltage = line.split(": ").last().toInt();
        } else if (line.contains("Charge counter")) {
            status.chargeCounter = line.split(": ").last().toInt();
        } else if (line.contains("status")) {
            status.status = line.split(": ").last().toInt();
        } else if (line.contains("health")) {
            status.health = line.split(": ").last().toInt();
        } else if (line.contains("present")) {
            status.present = line.contains("true");
        } else if (line.contains("level")) {
            status.level = line.split(": ").last().toInt();
        } else if (line.contains("scale")) {
            status.scale = line.split(": ").last().toInt();
        } else if (line.contains("voltage")) {
            status.voltage = line.split(": ").last().toInt();
        } else if (line.contains("temperature")) {
            status.temperature = line.split(": ").last().toInt();
        } else if (line.contains("technology")) {
            status.technology = line.split(": ").last().trimmed();
        }
    }
    // killAdbServer();
    return status;
}

Storage AndroidDebugBridge::storage(const QString &serialNum) const
{
    // startAdbServer();
    Storage storage;
    Utils::SynchronousProcess proc;
    proc.setTimeOutMessageBoxEnabled(true);
    Utils::SynchronousProcessResponse response = proc.runBlocking(adbPath(),
                                                                  {"-s", serialNum, "shell", "df", "/data"});
    if (response.result != Utils::SynchronousProcessResponse::Finished) {
        qWarning() << response.stdErr();
        return storage;
    }
    QString output = response.allOutput();
    QStringList lines = output.split("\n", QString::SkipEmptyParts);
    if (lines.size() < 2) {
        return storage;
    }
    QStringList parts = lines.at(1).split(QRegExp("\\s+"), QString::SkipEmptyParts);
    if (parts.size() < 4) {
        return storage;
    }
    storage.size = parts.at(1).toInt();
    storage.used = parts.at(2).toInt();
    storage.avail = parts.at(3).toInt();
    QString use = parts.at(4);
    use.remove('%');
    storage.use = use.toInt();
    // killAdbServer();
    return storage;
}

Storage AndroidDebugBridge::memory(const QString &serialNum) const
{
    // startAdbServer();
    Storage storage;
    Utils::SynchronousProcess proc;
    proc.setTimeOutMessageBoxEnabled(true);
    Utils::SynchronousProcessResponse response = proc.runBlocking(adbPath(),
                                                                  {"-s", serialNum, "shell", "cat", "/proc/meminfo"});
    if (response.result != Utils::SynchronousProcessResponse::Finished) {
        qWarning() << response.stdErr();
        return storage;
    }
    QString output = response.allOutput();
    QStringList lines = output.split("\n", QString::SkipEmptyParts);

    MemoryInfo memInfo;
    for (const QString &line : lines) {
        QStringList parts = line.split(":");
        if (parts.size() < 2) continue;
        QString key = parts.first().trimmed();
        int value = parts.last().trimmed().split(" ").first().toInt();

        if (key == "MemTotal") {
            memInfo.total = value;
        } else if (key == "MemFree") {
            memInfo.free = value;
        } else if (key == "MemAvailable") {
            memInfo.available = value;
        } else if (key == "Buffers") {
            memInfo.buffers = value;
        } else if (key == "Cached") {
            memInfo.cached = value;
        } else if (key == "SwapTotal") {
            memInfo.swapTotal = value;
        } else if (key == "SwapFree") {
            memInfo.swapFree = value;
        } else if (key == "Active") {
            memInfo.active = value;
        } else if (key == "Inactive") {
            memInfo.inactive = value;
        } else if (key == "Active(anon)") {
            memInfo.activeAnon = value;
        } else if (key == "Inactive(anon)") {
            memInfo.inactiveAnon = value;
        } else if (key == "Active(file)") {
            memInfo.activeFile = value;
        } else if (key == "Inactive(file)") {
            memInfo.inactiveFile = value;
        } else if (key == "Unevictable") {
            memInfo.unevictable = value;
        } else if (key == "Mlocked") {
            memInfo.mlocked = value;
        } else if (key == "Dirty") {
            memInfo.dirty = value;
        } else if (key == "Writeback") {
            memInfo.writeback = value;
        } else if (key == "AnonPages") {
            memInfo.anonPages = value;
        } else if (key == "Mapped") {
            memInfo.mapped = value;
        } else if (key == "Shmem") {
            memInfo.shmem = value;
        } else if (key == "Slab") {
            memInfo.slab = value;
        } else if (key == "SReclaimable") {
            memInfo.sreclaimable = value;
        } else if (key == "SUnreclaim") {
            memInfo.sunreclaim = value;
        } else if (key == "KernelStack") {
            memInfo.kernelStack = value;
        } else if (key == "PageTables") {
            memInfo.pageTables = value;
        } else if (key == "CommitLimit") {
            memInfo.commitLimit = value;
        } else if (key == "Committed_AS") {
            memInfo.committedAS = value;
        } else if (key == "VmallocTotal") {
            memInfo.vmallocTotal = value;
        } else if (key == "VmallocUsed") {
            memInfo.vmallocUsed = value;
        } else if (key == "VmallocChunk") {
            memInfo.vmallocChunk = value;
        } else if (key == "CmaTotal") {
            memInfo.cmaTotal = value;
        } else if (key == "CmaFree") {
            memInfo.cmaFree = value;
        } else if (key == "IonTotalCache") {
            memInfo.ionTotalCache = value;
        } else if (key == "IonTotalUsed") {
            memInfo.ionTotalUsed = value;
        } else if (key == "PActive(anon)") {
            memInfo.pActiveAnon = value;
        } else if (key == "PInactive(anon)") {
            memInfo.pInactiveAnon = value;
        } else if (key == "PActive(file)") {
            memInfo.pActiveFile = value;
        } else if (key == "PInactive(file)") {
            memInfo.pInactiveFile = value;
        } else if (key == "Isolate1Free") {
            memInfo.isolate1Free = value;
        } else if (key == "Isolate2Free") {
            memInfo.isolate2Free = value;
        } else if (key == "RsvTotalUsed") {
            memInfo.rsvTotalUsed = value;
        }
    }
    storage.size = memInfo.total;
    storage.avail = memInfo.cached + memInfo.sreclaimable + memInfo.free;
    storage.used = memInfo.total - storage.avail;
    storage.use = storage.used * 100 / storage.size;
    // killAdbServer();
    return storage;
}

NetworkCardInfo AndroidDebugBridge::ethInfo(const QString &serialNum) const
{
    //获取eth0 ip地址
    return parseNetworkCardInfo(serialNum, "eth0");
}

NetworkCardInfo AndroidDebugBridge::wlanInfo(const QString &serialNum) const
{
    //获取wlan0 ip地址
    return parseNetworkCardInfo(serialNum, "wlan0");
}

QSize AndroidDebugBridge::screenResolution(const QString &serialNum) const
{
    // startAdbServer();
    Utils::SynchronousProcess proc;
    proc.setTimeOutMessageBoxEnabled(true);
    Utils::SynchronousProcessResponse response = proc.runBlocking(adbPath(), {"-s", serialNum, "shell", "wm", "size"});
    // killAdbServer();
    if (response.result != Utils::SynchronousProcessResponse::Finished) {
        qWarning() << response.stdErr();
        return QSize();
    }
    QString output = response.stdOut();
    static const QRegularExpression rgxSize(R"(Physical size: (\d+)x(\d+))");
    QRegularExpressionMatch match = rgxSize.match(output);
    if (match.hasMatch()) {
        return QSize(match.captured(1).toInt(), match.captured(2).toInt());
    }
    return QSize();
}

void AndroidDebugBridge::startTcpIp(const QString &serialNum)
{
    // startAdbServer();
    Utils::SynchronousProcess proc;
    proc.setTimeOutMessageBoxEnabled(true);
    Utils::SynchronousProcessResponse response = proc.runBlocking(adbPath(), {"-s", serialNum, "tcpip", "5555"});
    if (response.result != Utils::SynchronousProcessResponse::Finished) {
        qWarning() << response.stdErr();
    }
    qInfo() << response.allOutput();
    // killAdbServer();
}

