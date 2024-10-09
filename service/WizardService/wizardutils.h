#ifndef WIZARDUTILS_H
#define WIZARDUTILS_H

#include <QObject>

class WizardUtils
{
public:
    WizardUtils();

    static void init();
    static bool createMainDir(QString dirPath, const QString &projectName);
    static bool createEmptyDir(QString dirPath, const QString &dirName, const QStringList &subdirs = QStringList());
    static bool createJsonFile(const QString &path, const QVariantHash &varData);
    static bool copyFile(const QString &sPath, const QString &dPath);

#ifdef USE_QMAKE_WIZARD
    static bool modifyPro(const QString &proPath, const QString &subName);
#endif

#ifdef USE_CMAKE_WIZARD
    static bool modifyCMakeLists(const QString &cmakePath, const QString &subName);
#endif

    static QString getFileContent(const QString &path);
    static bool writeFile(const QString &path, const QString &content);

private:
    static QString templatePath;
};

#endif // WIZARDUTILS_H
