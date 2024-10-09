/*******************************************************************
 *  Copyright(c) 2022-2025 ISS
 *  All right reserved. See GPL for detailed Information
 *
 *  文件名称: microdata.cpp
 *  简要描述: 该类是作为Qt风格的Q_D指针基类
 *  创建日期: 2024/07/01
 *  作者: WangHao
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#include "microdata.h"
#include "private/microdata_p.h"

namespace MicroKernel {

MicroDataPrivate::MicroDataPrivate(MicroData * const qq)
    : q_ptr(qq)
{

}

MicroDataPrivate::~MicroDataPrivate()
{
}

MicroData::MicroData(QObject *parent)
    : MicroData(*(new MicroDataPrivate(this)), parent)
{

}

MicroData::~MicroData()
{

}

MicroData::MicroData(MicroDataPrivate &dd, QObject *parent)
    : d_ptr(&dd)
    , QObject(parent)
{

}

}
