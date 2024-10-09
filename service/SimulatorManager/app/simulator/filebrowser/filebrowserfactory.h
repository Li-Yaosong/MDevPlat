#ifndef FILEBROWSERFACTORY_H
#define FILEBROWSERFACTORY_H

#include <QSharedPointer>

namespace HandleldTerminal {
namespace Internal {

class AbstractFileBrowser;
class Simulator;

struct FileBrowserFactory
{
    static AbstractFileBrowser *create(Simulator *sim, QObject *parent = nullptr);
};

}
}
#endif // FILEBROWSERFACTORY_H
