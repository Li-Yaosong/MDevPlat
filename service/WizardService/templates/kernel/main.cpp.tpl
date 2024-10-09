#include <KernelApplication>
#include <PluginManager>
#include <KernelSettings>
#include <QMessageBox>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QTextCodec>
#endif

using namespace PluginFramework;

int main(int argc, char* argv[])
{
    KernelApplication a(argc, argv);
    a.setApplicationName(QObject::tr("Micro Plugin Framework"));
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
#endif

    PluginManager::instance();
    return a.exec();
}
