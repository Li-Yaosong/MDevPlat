/*******************************************************************
 *  Copyright(c) 2022-2025 ISS
 *  All right reserved. See GPL for detailed Information
 *
 *  文件名称: sharedmemory.cpp
 *  简要描述: 共享内存操作处理类
 *  创建日期: 2024/07/17
 *  作者: WangHao
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#include <QSharedMemory>

#include "sharedmemory.h"
#include "private/sharedmemory_p.h"

namespace MicroKernel {

SharedMemoryPrivate::SharedMemoryPrivate(SharedMemory * const qq)
    : MicroDataPrivate(qq)
    , m_shm(Q_NULLPTR)
{
    m_shm.reset(new QSharedMemory());
}

SharedMemoryPrivate::~SharedMemoryPrivate()
{

}

SharedMemory::SharedMemory(QObject *parent)
    : MicroData(*(new SharedMemoryPrivate(this)), parent)
{
}

SharedMemory::SharedMemory(const QString &key, QObject *parent)
    : SharedMemory(parent)
{
    Q_D(SharedMemory);
    d->m_shm->setKey(key);
}

SharedMemory::~SharedMemory()
{

}

bool SharedMemory::lock()
{
    Q_D(SharedMemory);
    return d->m_shm->lock();
}

bool SharedMemory::unlock()
{
    Q_D(SharedMemory);
    return d->m_shm->unlock();
}

bool SharedMemory::detach()
{
    Q_D(SharedMemory);
    return d->m_shm->detach();
}

int SharedMemory::size() const
{
    Q_D(const SharedMemory);
    return d->m_shm->size();
}

void *SharedMemory::data()
{
    Q_D(SharedMemory);
    return d->m_shm->data();
}

const void *SharedMemory::data() const
{
    Q_D(const SharedMemory);
    return d->m_shm->data();
}

const void *SharedMemory::constData() const
{
    Q_D(const SharedMemory);
    return d->m_shm->constData();
}

QString SharedMemory::key() const
{
    Q_D(const SharedMemory);
    return d->m_shm->key();
}

void SharedMemory::setKey(const QString &key)
{
    Q_D(SharedMemory);
    d->m_shm->setKey(key);
}

SharedMemory::Error SharedMemory::error() const
{
    Q_D(const SharedMemory);
    return Error(d->m_shm->error());
}

QString SharedMemory::errorString() const
{
    Q_D(const SharedMemory);
    return d->m_shm->errorString();
}

bool SharedMemory::isAttached() const
{
    Q_D(const SharedMemory);
    return d->m_shm->isAttached();
}

bool SharedMemory::attach(AccessMode mode)
{
    Q_D(SharedMemory);
    return d->m_shm->attach(QSharedMemory::AccessMode(mode));
}

bool SharedMemory::create(int size, AccessMode mode)
{
    Q_D(SharedMemory);
    bool isOk = d->m_shm->create(size, QSharedMemory::AccessMode(mode));
    if (!isOk) {
        /* NOTE 在unix平台程序异常退出, QSharedMemory未执行析构
         * 或者detach时不会对共享内存段进行清理
         */
        if (QSharedMemory::AlreadyExists == d->m_shm->error()) {
            if (d->m_shm->attach(QSharedMemory::AccessMode(mode)))
                d->m_shm->detach();
        }
        isOk = d->m_shm->create(size, QSharedMemory::AccessMode(mode));
    }
    return isOk;
}

}
