#include <QCoreApplication>

#include "processhandler.h"
#include <QDebug>
#include <QProcess>
#include <QObject>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ProcessHandler simulatorManager;
    ProcessHandler androidDeviceManager;
    ProcessHandler theiaWeb;
    ProcessHandler site;
    ProcessHandler adbServer;
    ProcessHandler wizardService;
    ProcessHandler webadb;

#ifdef Q_OS_WIN
    simulatorManager.start(QCoreApplication::applicationDirPath() + "/SimulatorManager.exe");
    androidDeviceManager.start(QCoreApplication::applicationDirPath() + "/AndroidDeviceManager.exe");
    theiaWeb.start(QCoreApplication::applicationDirPath() + "/theia-web.exe");
    site.start(QCoreApplication::applicationDirPath() + "/site.exe");
    adbServer.start(QCoreApplication::applicationDirPath() + "/adb.exe", {});
#elif defined(Q_OS_LINUX)
    simulatorManager.start(QCoreApplication::applicationDirPath() + "/SimulatorManager");
    androidDeviceManager.start(QCoreApplication::applicationDirPath() + "/AndroidDeviceManager");
    wizardService.start(QCoreApplication::applicationDirPath() + "/WizardService");
    // webadb.start(QCoreApplication::applicationDirPath() + "/nodejs/bin/npm", {
    //                                                                           "--prefix",
    //                                                                           QCoreApplication::applicationDirPath() + "/webadb/packages/webadb",
    //                                                                           "start"});
    // // site.start()
#endif


;



    return a.exec();
}
