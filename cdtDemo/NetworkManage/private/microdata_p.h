/*******************************************************************
 *  Copyright(c) 2022-2025 ISS
 *  All right reserved. See GPL for detailed Information
 *
 *  文件名称: microdata_p.h
 *  简要描述: 该类是作为Qt风格的Q_D指针基类
 *  创建日期: 2024/07/01
 *  作者: WangHao
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef MICRODATA_P_H
#define MICRODATA_P_H

#include <QtGlobal>

namespace MicroKernel {
class MicroData;
class MicroDataPrivate
{
    Q_DECLARE_PUBLIC(MicroData)
#if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
    Q_DISABLE_COPY(MicroDataPrivate)
#else
    Q_DISABLE_COPY_MOVE(MicroDataPrivate)
#endif
public:
    explicit MicroDataPrivate(MicroData *const qq);
    virtual ~MicroDataPrivate();

protected:
    MicroData *const q_ptr;
};
}

#endif // MICRODATA_P_H
