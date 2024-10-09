#include <QCoreApplication>
#include "server.h"
#include "androiddebugbridge.h"
#include "synchronousprocess.h"
#include "websockifyjs.h"
// int main(int argc, char *argv[])
// {
//     QCoreApplication a(argc, argv);
//     Server s;
//     s.start();
//     return a.exec();
// }
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Server s;
    s.start();
    qDebug() << "adb-tool directory:" << adb->path();
    qDebug() << "adb-tool path:     " << adb->adbPath();
    qDebug() << endl;
    DeviceList devices = adb->devices();
    for(const Device &d : devices) {
        QString device = d.serialNum;
        qDebug() << "-----------------------------------";
        qDebug() << "Serial Number:     " << d.serialNum << d.status;
        qDebug() << "Device Model:      " << adb->deviceModel(device);
        qDebug() << "Android Version:   " << adb->androidVersion(device);
        qDebug() << "CPU Abi:           " << adb->cpuAbi(device);
        qDebug() << "Battery Status:    ";
        BatteryStatus status = adb->batteryStatus(device);
        qDebug() << "   AC powered:             " << status.acPowered;
        qDebug() << "   USB powered:            " << status.usbPowered;
        qDebug() << "   Wireless powered:       " << status.wirelessPowered;
        qDebug() << "   Max charging current:   " << status.maxChargingCurrent;
        qDebug() << "   Max charging voltage:   " << status.maxChargingVoltage;
        qDebug() << "   Charge counter:         " << status.chargeCounter;
        qDebug() << "   status:                 " << status.status;
        qDebug() << "   health:                 " << status.health;
        qDebug() << "   present:                " << status.present;
        qDebug() << "   level:                  " << status.level;
        qDebug() << "   scale:                  " << status.scale;
        qDebug() << "   voltage:                " << status.voltage;
        qDebug() << "   temperature:            " << status.temperature;
        qDebug() << "   technology:             " << status.technology;
        qDebug() << "Storage:           ";
        Storage storage = adb->storage(device);
        qDebug() << "   Avail:                  " << storage.avail;
        qDebug() << "   Used:                   " << storage.used;
        qDebug() << "   Size:                   " << storage.size;
        qDebug() << "   Use:                    " << storage.use << "%";
        qDebug() << "Memory Info:       ";
        Storage memory = adb->memory(device);
        qDebug() << "   Avail:                  " << memory.avail;
        qDebug() << "   Used:                   " << memory.used;
        qDebug() << "   Size:                   " << memory.size;
        qDebug() << "   Use:                    " << memory.use << "%";
        NetworkCardInfo eth = adb->ethInfo(device);
        qDebug() << "Ethernet Info:     ";
        if (eth.name.isEmpty()) {
            qDebug() << "   No Ethernet Card";
        } else {
            qDebug() << "   Name:                   " << eth.name;
            qDebug() << "   IPV4 Address:           " << eth.inetAddr;
            for(QString addr : eth.inet6AddrSite) {
                qDebug() << "   IPV6 Address(Site):     " << addr;
            }
            for(QString addr : eth.inet6AddrLink) {
                qDebug() << "   IPV6 Address(Link):     " << addr;
            }
            for(QString addr : eth.inet6AddrGlobal) {
                qDebug() << "   IPV6 Address(Global):   " << addr;
            }
            qDebug() << "   Netmask:                " << eth.mask;
            qDebug() << "   Broadcast Address:      " << eth.bcast;
        }
        NetworkCardInfo wlan = adb->wlanInfo(device);
        qDebug() << "Wireless Info:     ";
        if (wlan.name.isEmpty()) {
            qDebug() << "   No Wireless Card";
        } else {
            qDebug() << "   Name:                   " << wlan.name;
            qDebug() << "   IPV4 Address:           " << wlan.inetAddr;
            for(QString addr : wlan.inet6AddrSite) {
                qDebug() << "   IPV6 Address(Site):     " << addr;
            }
            for(QString addr : wlan.inet6AddrLink) {
                qDebug() << "   IPV6 Address(Link):     " << addr;
            }
            for(QString addr : wlan.inet6AddrGlobal) {
                qDebug() << "   IPV6 Address(Global):   " << addr;
            }
            qDebug() << "   Netmask:                " << wlan.mask;
            qDebug() << "   Broadcast Address:      " << wlan.bcast;
        }
        qDebug() << "-----------------------------------" << endl;
    }
    adb->killAdbServer();
    // if (!devices.isEmpty()) {
    //     WebsockifyJS::instance()->start(devices.at(0).serialNum);
    // }

    return a.exec();
}
