/*******************************************************************
 *  Copyright(c) 2022-2025 ISS
 *  All right reserved. See GPL for detailed Information
 *
 *  文件名称: netutils.cpp
 *  简要描述: 网络操作相关接口工具类
 *  创建日期: 2024/07/11
 *  作者: WangHao
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#include <QUdpSocket>
#include <QNetworkInterface>

#include "netutils.h"
#include "private/netutils_p.h"

namespace MicroKernel {

QString formatNetAddress(const QList<QNetworkAddressEntry> &list){

    QString name;
    QString result;

    for(const QNetworkAddressEntry &net : list)
    {
        QString curIP = net.ip().toString();
        if(curIP.contains("::"))
        {
            curIP = curIP.split("::").back();
        }

        if(curIP.contains('%'))
        {
            QStringList ipInfo = curIP.split('%');
            name = ipInfo.back();
            curIP = ipInfo.first();
        }

        result += (result.isEmpty() ? "" : " | ") + curIP;
    }

    return name + ": " + result;
}

NetUtilsPrivate::NetUtilsPrivate(NetUtils * const qq)
    : MicroDataPrivate(qq)
{

}

NetUtilsPrivate::~NetUtilsPrivate()
{

}

NetUtils::NetUtils(QObject *parent)
    : MicroData(*(new NetUtilsPrivate(this)), parent)
{

}

NetUtils::~NetUtils()
{

}
/*!
 * \brief 网络地址查询
 * \return 返回ip地址列表
 */
QStringList NetUtils::addresses() const
{
    QStringList hosts { };
    const auto &nets = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &net : qAsConst(nets)) {
        if (!net.isValid())
            continue;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
        QNetworkInterface::InterfaceType type = net.type();
        if (QNetworkInterface::Virtual == type ||
            QNetworkInterface::Loopback == type)
            continue;
#else

#endif
        QNetworkInterface::InterfaceFlags f = net.flags();
        if (f.testFlag(QNetworkInterface::IsUp) &&
            !f.testFlag(QNetworkInterface::IsLoopBack)) {
            hosts.append( formatNetAddress(net.addressEntries()) );
        }
    }

    return hosts;
}
/*!
 * \brief 端口占用判断
 * 如果传入的端口被占用返回true,否则返回false
 * \param port 端口号
 * \return 返回布尔值
 */
bool NetUtils::portUsed(quint16 port) const
{
    QUdpSocket socket;
    bool result = socket.bind(port);
    socket.close();
    return result;
}

}
