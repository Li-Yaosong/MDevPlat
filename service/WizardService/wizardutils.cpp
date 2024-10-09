#include "wizardutils.h"

#include <QCoreApplication>
#include <QDir>
#include <QTextStream>
#include <QJsonDocument>
#include <QDebug>

QString WizardUtils::templatePath = "";

WizardUtils::WizardUtils()
{

}

void WizardUtils::init()
{
    templatePath = qApp->applicationDirPath() + "/templates";
}

bool WizardUtils::createMainDir(QString dirPath, const QString &projectName)
{
    dirPath += "/app";
    QDir dir(dirPath);
    if(!dir.exists())
    {
        dir.mkdir(dirPath);
    }

    QString content, newFilePath;

#ifdef USE_QMAKE_WIZARD
    //创建app.pro
    const QString proFilePath = templatePath + "/kernel/project.pro";
    content = getFileContent(proFilePath);
    content.replace("%PROJECT_NAME%", projectName);
    newFilePath = dirPath + "/app.pro";
    if(!writeFile(newFilePath, content))
    {
        return false;
    }
#endif

#ifdef USE_CMAKE_WIZARD
    //创建CMakeLists.txt
    const QString cmakeFilePath = templatePath + "/kernel/CMakeLists.txt";
    content = getFileContent(cmakeFilePath);
    content.replace("%PROJECT_NAME%", projectName);
    newFilePath = dirPath + "/CMakeLists.txt";
    if(!writeFile(newFilePath, content))
    {
        return false;
    }
#endif

    //创建main.cpp
    const QString cppFilePath = templatePath + "/kernel/main.cpp.tpl";
    content = getFileContent(cppFilePath);
    content.replace("{project_name}", projectName);
    newFilePath = dirPath + "/main.cpp";
    if(!writeFile(newFilePath, content))
    {
        return false;
    }

    return true;
}

bool WizardUtils::createEmptyDir(QString dirPath, const QString &dirName, const QStringList &subdirs)
{
    QDir dir(dirPath);
    if(!dir.exists() || dirName.isEmpty())
    {
        return false;
    }

    dirPath += "/" + dirName;
    QDir newDir(dirPath);
    if(!newDir.exists())
    {
        newDir.mkdir(dirPath);
    }

    QString content;
    bool ret = true;

#ifdef USE_QMAKE_WIZARD
    const QString emptyProPath = templatePath + "/subdirs.pro";
    QString subDirStr;
    for(int i = 0; i < subdirs.size(); ++i)
    {
        if(i)
        {
            subDirStr += " \\\n\t";
        }

        subDirStr += subdirs[i];
    }

    content = getFileContent(emptyProPath);
    content.replace("%SUBDIRS%", subDirStr);
    const QString newFilePath = dirPath + "/" + dirName + ".pro";
    ret &= writeFile(newFilePath, content);
#endif

#ifdef USE_CMAKE_WIZARD
    const QString emptyCmakePath = templatePath + "/CMakeLists.txt";
    content = getFileContent(emptyCmakePath);
    for(const QString &sub : subdirs)
    {
        content += QString("\nADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/%1)").arg(sub);
    }
    content.replace("%PROJECT_NAME%", dirName);
    ret &= writeFile(dirPath + "/CMakeLists.txt", content);
#endif

    return ret;
}

bool WizardUtils::createJsonFile(const QString &path, const QVariantHash &varData)
{
    QJsonDocument doc = QJsonDocument::fromVariant(varData);

    QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    file.write(doc.toJson());
    file.close();

    return true;
}

bool WizardUtils::copyFile(const QString &sPath, const QString &dPath)
{
    QFile file(templatePath + sPath);
    if(!file.exists())
    {
        return false;
    }

    return file.copy(dPath);
}

#ifdef USE_QMAKE_WIZARD
bool WizardUtils::modifyPro(const QString &proPath, const QString &subName)
{
    if(!QFile(proPath).exists())
    {
        return false;
    }

    QString content = getFileContent(proPath);
    QStringList fileList = content.split('\n');
    int index = 0;
    while(index < fileList.size() && !fileList[index].contains("SUBDIRS")){
        ++index;
    }
    while (index < fileList.size() && fileList[index].trimmed().endsWith("\\")) {
        ++index;
    }

    if(index < fileList.size())
    {
        if(fileList[index].trimmed().endsWith("=")){
            fileList[index] = fileList[index].trimmed() + ' ' + subName;
        }
        else{
            fileList[index] += " \\";
            fileList.insert(index + 1, '\t' + subName);
        }
    }
    else
    {
        fileList.append('\t' + subName);
    }

    writeFile(proPath, fileList.join('\n'));

    return true;
}
#endif

#ifdef USE_CMAKE_WIZARD
bool WizardUtils::modifyCMakeLists(const QString &cmakePath, const QString &subName)
{
    if(!QFile(cmakePath).exists())
    {
        return false;
    }

    QString content = getFileContent(cmakePath);
    content += QString("\nADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/%1)").arg(subName);
    writeFile(cmakePath, content);

    return true;
}
#endif

QString WizardUtils::getFileContent(const QString &path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return "";
    }

    QTextStream stream(&file);
    QString content = stream.readAll();
    file.close();
    return content;
}

bool WizardUtils::writeFile(const QString &path, const QString &content)
{
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug() << file.errorString();
        return false;
    }

    if(path.endsWith(".cpp") || path.endsWith(".h"))
    {
        QByteArray bom = QByteArray::fromHex("EFBBBF");
        file.write(bom);
    }

    QTextStream ostream(&file);
    ostream.setCodec("UTF-8");
    ostream << content;
    file.close();

    return true;
}
