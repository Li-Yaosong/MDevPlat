#ifndef HANDHELDTERMINALDEFINES_H
#define HANDHELDTERMINALDEFINES_H

#include <QString>

namespace HandleldTerminal {
enum SimulatorType {
    Android,
    Linux
};

enum CpuType {
    x86_64,
    aarch64,
    unkonwn
};

struct FileInfo
{
    QString name;
    quint64 size;
    bool isFolder;
};

}

#endif // HANDHELDTERMINALDEFINES_H
