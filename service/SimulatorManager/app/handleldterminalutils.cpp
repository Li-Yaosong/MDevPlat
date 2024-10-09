#include "handleldterminalutils.h"

namespace HandleldTerminal {
namespace Utils {

static const quint64 GB_SIZE = 1024 * 1024 * 1024;
static const quint64 MB_SIZE = 1024 * 1024;

QString memorySize(quint64 size)
{
    if (size > GB_SIZE) {
        return QString("%1 GB").arg(QString::number(size * 1.0 / GB_SIZE , 'f', 2));
    }
    else if (size > MB_SIZE) {
        return QString("%1 MB").arg(QString::number(size * 1.0 / MB_SIZE, 'f', 2));
    }
    return QString("%1 KB").arg(QString::number(size * 1.0 / 1024, 'f', 2));
}
}
}
