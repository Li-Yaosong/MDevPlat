#include "clientworker.h"
#include "tcpdatareceiver.h"
#include "wizarddefines.h"
#include "wizardutils.h"

#include <QJsonDocument>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>

using namespace KernelWizard;

ClientWorker::ClientWorker(QTcpSocket *socket, QObject *parent) : QObject(parent)
{
    m_socket = socket;
    m_receiver = new TcpDataReceiver(m_socket.data(), this);
    m_receiver->registerHandler("CREATE_KERNEL", this, &ClientWorker::createKernel);
    m_receiver->registerHandler("CREATE_Plugin", this, &ClientWorker::createPlugin);

    WizardUtils::init();
}

void ClientWorker::reply(const QVariant &result)
{
    QJsonDocument doc = QJsonDocument::fromVariant(result);
    QByteArray r = doc.toJson();
    m_socket->write(r);
}

void ClientWorker::createKernel(const QVariantHash &map)
{
    QString projectData = value<QString>(map, "data");

    QVariantHash result;
    result["result"] = "Failed";
    QStringList projectInfos = projectData.split('|');
    if(projectInfos.size() < 2)
    {
        result.insert("message", "The project info error!");
        reply(result);
        return;
    }
    QString projectName = projectInfos[0];
    QString projectPath = projectInfos[1].replace('\\', '/');

    if(projectName.isEmpty() || projectPath.isEmpty())
    {
        result.insert("message", "The project name or project path is empty!");
        reply(result);
        return;
    }

    if(QDir(projectPath + "/" + projectName).exists())
    {
        result.insert("message", "The project is exists!");
        reply(result);
        return;
    }

    /* 创建项目文件夹 */
    const QStringList projectSubs = {"src", "demo", "test"};
    bool ret = WizardUtils::createEmptyDir(projectPath, projectName, projectSubs);
    projectPath += "/" + projectName;

#ifdef USE_CMAKE_WIZARD
    const QString rootCamke = projectPath + "/CMakeLists.txt";
    QString content = WizardUtils::getFileContent(rootCamke);
    content += "\nset(ROOT_DIR ${CMAKE_BINARY_DIR}/bin)\nset(PLUGIN_DIR ${ROOT_DIR}/plugins)\n"
                "set(CMAKE_BUILD_TYPE \"Debug\")";
    WizardUtils::writeFile(rootCamke, content);
#endif

    /* 创建项目主文件夹 */
    const QStringList srcSubs = {"libs", "app", "plugins", "tools"};
    ret |= WizardUtils::createEmptyDir(projectPath, "src", srcSubs);
    ret |= WizardUtils::createEmptyDir(projectPath, "demo");
    ret |= WizardUtils::createEmptyDir(projectPath, "test");

    //创建工作区
    const QString workspaceConfigPath = projectPath + "/" + projectName + ".code.workspace";
    QVariantHash space;
    QVariantHash obj;
    obj["path"] = ".";

    space["folders"] = QVariantList() << obj;
    space["name"] = projectName;
    ret |= WizardUtils::createJsonFile(workspaceConfigPath, space);

    /* 创建项目源码文件夹 */
    const QString mainDir = projectPath + "/src";
    ret |= WizardUtils::createEmptyDir(mainDir, "libs");
    ret |= WizardUtils::createEmptyDir(mainDir, "plugins");
    ret |= WizardUtils::createEmptyDir(mainDir, "tools");
    ret |= WizardUtils::createMainDir(mainDir, projectName);

#ifdef USE_QMAKE_WIZARD
    /* 创建项目配置文件 */
    ret |= WizardUtils::copyFile("/kernel/project.pri", projectPath + "/config.pri");
    ret |= WizardUtils::copyFile("/kernel/plugins.pri", mainDir + "/plugins/plugins.pri");
#endif

    if(ret)
    {
        result["result"] = "Successe";
        result["message"] = projectPath;
    }
    else result.insert("message", "Create project failed!");
    reply(result);
}

void ClientWorker::createPlugin(const QVariantHash &map)
{
    QString pluginInfo = value<QString>(map, "data");
    QJsonDocument doc = QJsonDocument::fromJson(pluginInfo.toLocal8Bit());
    QVariantHash plugin = doc.toVariant().toHash();
    QString pluginName = plugin[NAME].toString();
    QString projectPath = plugin[PATH].toString().replace('\\', '/');
    QString className = plugin[CLASSNAME].toString();
    QString pluginType = plugin[PLUGINTYPE].toString();
    QString triggerType = plugin[TRIGGERTYPE].toString();
    QString allowStop = plugin[ALLOWSTOP].toString();
    bool isCreateUI = (plugin[ISCREATEUI].toString() == "true");
    QString widgetName = plugin[WIDGETNAME].toString();

    QVariantHash result;
    result["result"] = "Failed";

    if(!QDir(projectPath + "/src/plugins").exists())
    {
        result.insert("message", "The project path error!");
        reply(result);
        return;
    }

    for(const QString & filed : {pluginName, projectPath, className, pluginType, triggerType, allowStop})
    {
        if(filed.isEmpty())
        {
            result.insert("message", "The plugin info error!");
            reply(result);
            return;
        }
    }

    const QString pluginPath = projectPath + "/src/plugins/" + pluginName;
    QDir dir(pluginPath);
    if(!dir.exists())
    {
        dir.mkdir(pluginPath);
    }
    else
    {
        result.insert("message", "The plugin name exists!");
        reply(result);
        return;
    }

    const QString templateFolderPath = qApp->applicationDirPath() + "/templates/plugin";

    QString filePath, content, newFile;
    // 生成global头文件
    if(pluginType == "dll")
    {
        filePath = templateFolderPath + "/classname_global.h";
        content = WizardUtils::getFileContent(filePath);
        content.replace("%ClassName%", className.toUpper());
        newFile = pluginPath + "/" + className.toLower() + "_global.h";
        WizardUtils::writeFile(newFile, content);
    }

    // 生成插件类头文件
    filePath = templateFolderPath + "/classname.h";
    content = WizardUtils::getFileContent(filePath);
    content.replace("%HEADER_H%", className.toUpper() + "_H");
    content.replace("%ClassName%", className);
    if(pluginType == "dll")
    {
        content.replace("%SHARED_EXPORT%", className.toUpper() + "SHARED_EXPORT");
        content.replace("%COMPAREVERSION%", QString("#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)\n"
                                            "\tQ_PLUGIN_METADATA(IID \"%1\")\n#endif\n").arg(pluginName));
        content.replace("%INCLUDE%", QString("#include \"%1_global.h\"").arg(className.toLower()));
    }
    else
    {
        content.replace("%SHARED_EXPORT%", QString());
        content.replace("%COMPAREVERSION%", QString());
        content.replace("%INCLUDE%", QString());
    }
    newFile = pluginPath + "/" + className.toLower() + ".h";
    WizardUtils::writeFile(newFile, content);

    if(pluginType == "exe")
    {
        // 生main.cpp文件
        filePath = templateFolderPath + "/main.cpp";
        content = WizardUtils::getFileContent(filePath);
        content.replace("%PLUGINHEADER%", className.toLower() + ".h");
        content.replace("%ClassName%", className);
        content.replace("%PluginName%", pluginName);
        newFile = pluginPath + "/" + "main.cpp";
        WizardUtils::writeFile(newFile, content);
    }

    // 生成插件类源文件
    filePath = templateFolderPath + "/classname.cpp";
    content = WizardUtils::getFileContent(filePath);
    content.replace("%INCLUDE%", QString("#include \"%1.h\"").arg(className.toLower()));
    content.replace("%ClassName%", className);
    content.replace("%PluginName%", pluginName);
    newFile = pluginPath + "/" + className.toLower() + ".cpp";
    WizardUtils::writeFile(newFile, content);

    // 生成MANIFEST.MF文件
    filePath = templateFolderPath + "/MANIFEST.MF";
    content = WizardUtils::getFileContent(filePath);
    content.replace("%SYMBOLIC%", pluginName);
    content.replace("%VERSION%", "1.0.0");
    content.replace("%COMPATVERSION%", "1.0.0");
    content.replace("%ACTIVATION%", triggerType);
    content.replace("%TYPE%", pluginType);
    content.replace("%UNLOAD%", allowStop);
    content.replace("%VENDOR%", "");
    content.replace("%COPYRIGHT%", "");
    content.replace("%LICENSE%", "");
    content.replace("%DESCRIPTION%", "");
    content.replace("%CATEGORY%", "");
    content.replace("%WEBSITE%", "");
    content.replace("%DEPENDENCY%", "");
    newFile = pluginPath + "/" + "MANIFEST.MF";
    WizardUtils::writeFile(newFile, content);

    //生成xml文件
    filePath = templateFolderPath + "/plug.xml";
    content = WizardUtils::getFileContent(filePath);
    content.replace("%ClassName%", className);
    content.replace("%PluginName%", pluginName);
    newFile = pluginPath + "/" + "plug.xml";
    WizardUtils::writeFile(newFile, content);

    //工程文件定义
    QStringList headers, sources;
    if(pluginType == "dll"){
        headers << className.toLower() + "_global.h";
    }
    else{
        sources << "main.cpp";
    }
    headers << className.toLower() + ".h";
    sources << className.toLower() + ".cpp";
    if(isCreateUI)
    {
        headers << widgetName.toLower() +".h";
        sources << widgetName.toLower() + ".cpp";
    }

#ifdef USE_QMAKE_WIZARD
    // 生成pro文件
    filePath = templateFolderPath + "/project.pro";
    content = WizardUtils::getFileContent(filePath);
    content.replace("%PluginName%", pluginName);
    content.replace("%TEMPLATETYPE%", (pluginType == "dll" ? "lib" : "app"));
    content.replace("%DEFINESSHARDLIBRARY%", QString("DEFINES += %1_LIBRARY").arg(className.toUpper()));
    content.replace("%ExeSupport%", "");
    content.replace("%INSTALLS_INTERFACE_FILES%", "");
    content.replace("%INSTALLS_PRF_FILE%", "");

    if(isCreateUI){
        content.replace("%FORM%", "FORMS += " + widgetName.toLower() + ".ui");
    }
    else{
        content.replace("%FORM%", "");
    }

    content.replace("%SOURCES%", sources.join(" \\\n\t"));
    content.replace("%HEADERS%", headers.join(" \\\n\t"));

    newFile = pluginPath + "/" + pluginName + ".pro";
    WizardUtils::writeFile(newFile, content);
#endif

#ifdef USE_CMAKE_WIZARD
    //生成CMakeLists.txt
    filePath = templateFolderPath + "/CMakeLists.txt";
    content = WizardUtils::getFileContent(filePath);
    content.replace("%PluginName%", pluginName);
    content.replace("%SOURCES%", sources.join("\n    "));
    content.replace("%HEADERS%", sources.join("\n    "));
    if(isCreateUI){
        content.replace("%FORM%", QString("set(FORMS \n    %1.ui\n)").arg(widgetName.toLower()));
    }
    else{
        content.replace("%FORM%", "");
    }
    content.replace("%TEMPLATETYPE%", (pluginType == "dll" ? "add_library" : "add_executable"));
    QString addFiles = (pluginType == "dll" ? "SHARED" : "");
    addFiles += "\n    ${HEADERS}\n    ${SOURCES}";
    addFiles += (isCreateUI ? "\n    ${FORMS}\n" : "\n");
    content.replace("%ADDFILES%", addFiles);
    content.replace("%DEFINESSHARDLIBRARY%", QString("%1_LIBRARY").arg(className.toUpper()));
    newFile = pluginPath + "/CMakeLists.txt";
    WizardUtils::writeFile(newFile, content);
#endif

    //创建ui文件
    if(isCreateUI) createPluginUi(plugin, pluginPath);

#ifdef USE_QMAKE_WIZARD
    WizardUtils::modifyPro(projectPath + "/src/plugins/plugins.pro", pluginName);
#endif
#ifdef USE_CMAKE_WIZARD
    WizardUtils::modifyCMakeLists(projectPath + "/src/plugins/CMakeLists.txt", pluginName);
#endif

    result["result"] = "Successe";
    reply(result);
}

void ClientWorker::createPluginUi(const QVariantHash &plugin, const QString pluginPath)
{
    QString baseClass = plugin[UIBASECLASS].toString();
    QString widgetName = plugin[WIDGETNAME].toString();

    if(widgetName.isEmpty() || !QStringList({"QWidget", "QDialog", "QMainWindow"}).contains(baseClass))
    {
        return;
    }

    const QString templateFolderPath = qApp->applicationDirPath() + "/templates/plugin";
    QString filePath, content, newFile;
    // 生成头文件
    filePath = templateFolderPath + "/widget.h";
    content = WizardUtils::getFileContent(filePath);
    content.replace("%HEADER_H%", widgetName.toUpper() + "_H");
    content.replace("%BaseClass%", baseClass);
    content.replace("%WidgetName%", widgetName);
    newFile = pluginPath + "/" + widgetName.toLower() + ".h";
    WizardUtils::writeFile(newFile, content);

    // 生成源文件
    filePath = templateFolderPath + "/widget.cpp";
    content = WizardUtils::getFileContent(filePath);
    content.replace("%INCLUDE%", widgetName.toLower() + ".h");
    content.replace("%BaseClass%", baseClass);
    content.replace("%WidgetName%", widgetName);
    content.replace("%UIFILE_HEADER%", "ui_" + widgetName.toLower() + ".h");
    newFile = pluginPath + "/" + widgetName.toLower() + ".cpp";
    WizardUtils::writeFile(newFile, content);

    // 生成ui文件
    filePath = templateFolderPath + (baseClass == "QMainWindow" ? "/mainwindow.ui" : "/widget.ui");
    content = WizardUtils::getFileContent(filePath);
    content.replace("%BaseClass%", baseClass);
    content.replace("%WidgetName%", widgetName);
    newFile = pluginPath + "/" + widgetName.toLower() + ".ui";
    WizardUtils::writeFile(newFile, content);

}

