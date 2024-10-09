#ifndef NETUTILS_P_H
#define NETUTILS_P_H

#include "private/microdata_p.h"

namespace MicroKernel {
class NetUtils;
class NetUtilsPrivate : public MicroDataPrivate
{
    Q_DECLARE_PUBLIC(NetUtils)
#if (QT_VERSION < QT_VERSION_CHECK(5, 13, 0))
    Q_DISABLE_COPY(NetUtilsPrivate)
#else
    Q_DISABLE_COPY_MOVE(NetUtilsPrivate)
#endif
    explicit NetUtilsPrivate(NetUtils *const qq);
    ~NetUtilsPrivate();
};

}

#endif // NETUTILS_P_H
