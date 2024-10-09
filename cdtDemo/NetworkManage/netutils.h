/*******************************************************************
 *  Copyright(c) 2022-2025 ISS
 *  All right reserved. See GPL for detailed Information
 *
 *  文件名称: netutils.h
 *  简要描述: 网络操作相关接口工具类
 *  创建日期: 2024/07/11
 *  作者: WangHao
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef NETUTILS_H
#define NETUTILS_H

#include "microdata.h"

namespace MicroKernel {
class NetUtilsPrivate;
class NetUtils : public MicroData
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(NetUtils)
#if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
    Q_DISABLE_COPY(NetUtils)
#else
    Q_DISABLE_COPY_MOVE(NetUtils)
#endif
public:
    explicit NetUtils(QObject *parent = nullptr);
    ~NetUtils();

    QStringList addresses() const;
    bool portUsed(quint16 port) const;
};

}

#endif // NETUTILS_H
