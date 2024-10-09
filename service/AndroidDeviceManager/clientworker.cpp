#include "clientworker.h"
#include <QTcpSocket>
#include "tcpdatareceiver.h"
#include "androiddebugbridge.h"
#include "websockifyjs.h"

#include <QJsonDocument>
#include <QSize>
#include <QDebug>
ClientWorker::ClientWorker(QTcpSocket *socket, QObject *parent)
    : QObject{parent},
    m_receiver{new TcpDataReceiver{socket, this}},
    m_socket{socket}
{
    m_receiver->registerHandler("DEVICES", this, &ClientWorker::devices);
    m_receiver->registerHandler("DEVICE_BASE_INFO", this, &ClientWorker::deviceBaseInfo);
    m_receiver->registerHandler("DEVICE_BATTERY_INFO", this, &ClientWorker::deviceBatteryInfo);
    m_receiver->registerHandler("DEVICE_STORAGE_INFO", this, &ClientWorker::deviceStorageInfo);
    m_receiver->registerHandler("DEVICE_MEMORY_INFO", this, &ClientWorker::deviceMemoryInfo);
    m_receiver->registerHandler("DEVICE_ETH_INFO", this, &ClientWorker::deviceEthInfo);
    m_receiver->registerHandler("DEVICE_WLAN_INFO", this, &ClientWorker::deviceWlanInfo);
    m_receiver->registerHandler("START_WBSOCKET", this, &ClientWorker::startWebsocket);

}

void ClientWorker::devices(const QVariantMap &map)
{
    Q_UNUSED(map);
    DeviceList devices = adb->devices();
    QVariantList response;
    for(const Device &device : devices) {
        QVariantMap info;
        info["SerialNumber"] = device.serialNum;
        info["Status"] = device.status;
        response.append(info);
    }
    reply(response);
}

void ClientWorker::deviceBaseInfo(const QVariantMap &map)
{
    QString serial = value<QString>(map, "data");
    QString deviceModel = adb->deviceModel(serial);
    QString cpuAbi = adb->cpuAbi(serial);
    QString androidVersion = adb->androidVersion(serial);
    QSize screenSize = adb->screenResolution(serial);
    QString screenResolution = QString("%1x%2").arg(screenSize.width()).arg(screenSize.height());
    QVariantMap response;
    response["DeviceModel"] = deviceModel;
    response["CpuAbi"] = cpuAbi;
    response["AndroidVersion"] = androidVersion;
    response["ScreenResolution"] = screenResolution;
    reply(response);
}

void ClientWorker::deviceBatteryInfo(const QVariantMap &map)
{
    QString serial = value<QString>(map, "data");
    BatteryStatus batteryStatus = adb->batteryStatus(serial);
    QVariantMap response;
    response["Level"] = batteryStatus.level;
    response["Temperature"] = batteryStatus.temperature;
    response["Voltage"] = batteryStatus.voltage;
    response["Powered"] = batteryStatus.acPowered || batteryStatus.usbPowered || batteryStatus.wirelessPowered;
    reply(response);
}

void ClientWorker::deviceStorageInfo(const QVariantMap &map)
{
    QString serial = value<QString>(map, "data");
    Storage storage = adb->storage(serial);
    QVariantMap response;
    response["Total"] = storage.size;
    response["Free"] = storage.avail;
    response["Used"] = storage.used;
    response["Use"] = storage.use;
    reply(response);
}

void ClientWorker::deviceMemoryInfo(const QVariantMap &map)
{
    QString serial = value<QString>(map, "data");
    Storage memory = adb->memory(serial);
    QVariantMap response;
    response["Total"] = memory.size;
    response["Free"] = memory.avail;
    response["Used"] = memory.used;
    response["Use"] = memory.use;
    reply(response);
}

void ClientWorker::deviceEthInfo(const QVariantMap &map)
{
    QString serial = value<QString>(map, "data");
    NetworkCardInfo ethInfo = adb->ethInfo(serial);
    QVariantMap response;
    // if (ethInfo.name.isEmpty())
    // {
    //     response["Name"] = "No Ethernet Card";
    //     reply(response);
    //     return;
    // }
    // else
    // {
        response["Name"] = ethInfo.name;
        response["IPV4Address"] = ethInfo.inetAddr;
        response["IPV6AddressSite"] = ethInfo.inet6AddrSite;
        response["IPV6AddressLink"] = ethInfo.inet6AddrLink;
        response["IPV6AddressGlobal"] = ethInfo.inet6AddrGlobal;
        response["NetMask"] = ethInfo.mask;
        response["BroadcastAddress"] = ethInfo.bcast;
    // }
    reply(response);
}

void ClientWorker::deviceWlanInfo(const QVariantMap &map)
{
    QString serial = value<QString>(map, "data");
    NetworkCardInfo wlanInfo = adb->wlanInfo(serial);
    QVariantMap response;
    // if (wlanInfo.name.isEmpty())
    // {
    //     response["Name"] = "No Wireless Card";
    //     reply(response);
    //     return;
    // }
    // else
    // {
        response["Name"] = wlanInfo.name;
        response["IPV4Address"] = wlanInfo.inetAddr;
        response["IPV6AddressSite"] = wlanInfo.inet6AddrSite;
        response["IPV6AddressLink"] = wlanInfo.inet6AddrLink;
        response["IPV6AddressGlobal"] = wlanInfo.inet6AddrGlobal;
        response["NetMask"] = wlanInfo.mask;
        response["BroadcastAddress"] = wlanInfo.bcast;
    // }
    reply(response);
}

void ClientWorker::startWebsocket(const QVariantMap &map)
{
    QString serial = value<QString>(map, "data");
    QVariantMap response;
    response["Port"] = WebsockifyJS::instance()->start(serial);
    reply(response);
}

void ClientWorker::reply(const QVariant &result)
{
    QJsonDocument doc = QJsonDocument::fromVariant(result);
    QByteArray r = doc.toJson();
    m_socket->write(r);
}

