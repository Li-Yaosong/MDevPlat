#include "filebrowserfactory.h"
#include "abstractfilebrowser.h"
#include "linuxsimulator.h"
#include "androidsimulator.h"
#include "linuxfilebrowser.h"
#include "androidfilebrowser.h"
#include "handleldterminaldefines.h"

namespace HandleldTerminal {
namespace Internal {

AbstractFileBrowser *FileBrowserFactory::create(Simulator *sim, QObject *parent)
{
    if (sim == nullptr)
        return nullptr;
    if (sim->type() == HandleldTerminal::SimulatorType::Linux) {
        LinuxSimulator *ls = qobject_cast<LinuxSimulator *>(sim);
        if (ls != nullptr) {
            return new LinuxFileBrowser(ls, parent);
        }
    } else if (sim->type() == HandleldTerminal::SimulatorType::Android) {
        AndroidSimulator *as = qobject_cast<AndroidSimulator *>(sim);
        if (as != nullptr) {
            return new AndroidFileBrowser(as, parent);
        }
    }
    return nullptr;
}
}
}
