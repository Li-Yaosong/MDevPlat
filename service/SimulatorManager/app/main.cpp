#include <QCoreApplication>
#include "server.h"
#include "globalsimulatormanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QString javaHome = QCoreApplication::applicationDirPath() + "/Java/jdk-17.0.12";
    qputenv("JAVA_HOME", javaHome.toUtf8());
    HandleldTerminal::Internal::GlobalSimulatorManager manager;
    manager.list();
    Server s;
    s.start();
    return a.exec();
}
