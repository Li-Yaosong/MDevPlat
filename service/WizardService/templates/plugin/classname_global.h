#ifndef %ClassName%_GLOBAL_H
#define %ClassName%_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(%ClassName%_LIBRARY)
#  define %ClassName%SHARED_EXPORT Q_DECL_EXPORT
#else
#  define %ClassName%SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // %ClassName%_GLOBAL_H
