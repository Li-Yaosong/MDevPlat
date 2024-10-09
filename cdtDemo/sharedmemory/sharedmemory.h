/*******************************************************************
 *  Copyright(c) 2022-2025 ISS
 *  All right reserved. See GPL for detailed Information
 *
 *  文件名称: sharedmemory.h
 *  简要描述: 共享内存操作处理类
 *  创建日期: 2024/07/17
 *  作者: WangHao
 *  说明:
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/
#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include "microdata.h"

namespace MicroKernel {
class SharedMemoryPrivate;
class SharedMemory : public MicroData
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SharedMemory)
#if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
    Q_DISABLE_COPY(SharedMemory)
#else
    Q_DISABLE_COPY_MOVE(SharedMemory)
#endif
public:

    enum class AccessMode
    {
        ReadOnly  = 0,
        ReadWrite = 1,
    };

    enum class Error
    {
        NoError,
        PermissionDenied,
        InvalidSize,
        KeyError,
        AlreadyExists,
        NotFound,
        LockError,
        OutOfResources,
        UnknownError,
    };

    explicit SharedMemory(QObject *parent = nullptr);
    explicit SharedMemory(const QString &key, QObject *parent = Q_NULLPTR);
    ~SharedMemory();

    bool lock();
    bool unlock();

    bool detach();
    int size() const;

    void *data();
    const void *data() const;
    const void *constData() const;

    QString key() const;
    void setKey(const QString &key);

    Error error() const;
    QString errorString() const;

    bool isAttached() const;
    bool attach(AccessMode mode = AccessMode::ReadWrite);
    bool create(int size, AccessMode mode = AccessMode::ReadWrite);
};

}

#endif // SHAREDMEMORY_H
