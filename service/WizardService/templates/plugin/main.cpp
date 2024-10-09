#include <QApplication>

#include <CommClient>

#include "%PLUGINHEADER%"

using namespace Execute;

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    qApp->setQuitOnLastWindowClosed(false);

    CommClient client;
    client.start(new %ClassName%, "%PluginName%");

    return a.exec();
}
