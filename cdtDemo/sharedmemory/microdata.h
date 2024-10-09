/*******************************************************************
 *  Copyright(c) 2022-2025 ISS
 *  All right reserved. See GPL for detailed Information
 *
 *  文件名称: microdata.h
 *  简要描述: 该类是作为Qt风格的Q_D指针基类
 *  创建日期: 2024/07/01
 *  作者: WangHao
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef MICRODATA_H
#define MICRODATA_H

#include <QObject>

namespace MicroKernel {

class MicroDataPrivate;
class MicroData : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MicroData)
#if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
    Q_DISABLE_COPY(MicroData)
#else
    Q_DISABLE_COPY_MOVE(MicroData)
#endif
public:
    explicit MicroData(QObject *parent = Q_NULLPTR);
    virtual ~MicroData();

protected:
    const QScopedPointer<MicroDataPrivate> d_ptr;
    explicit MicroData(MicroDataPrivate &dd, QObject *parent = Q_NULLPTR);
};

}


#endif // MICRODATA_H
