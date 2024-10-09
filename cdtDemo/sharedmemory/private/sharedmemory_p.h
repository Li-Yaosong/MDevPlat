#ifndef SHAREDMEMORY_P_H
#define SHAREDMEMORY_P_H

#include <QScopedPointer>

#include "private/microdata_p.h"

QT_BEGIN_NAMESPACE
class QSharedMemory;
QT_END_NAMESPACE

namespace MicroKernel {
class SharedMemory;
class SharedMemoryPrivate : public MicroDataPrivate
{
    Q_DECLARE_PUBLIC(SharedMemory)
#if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
    Q_DISABLE_COPY(SharedMemoryPrivate)
#else
    Q_DISABLE_COPY_MOVE(SharedMemoryPrivate)
#endif
public:
    explicit SharedMemoryPrivate(SharedMemory *const qq);
    ~SharedMemoryPrivate();

private:
    QScopedPointer<QSharedMemory> m_shm;
};
}

#endif // SHAREDMEMORY_P_H
