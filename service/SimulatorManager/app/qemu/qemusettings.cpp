#include "qemusettings.h"
#include <QGlobalStatic>
#include <QCoreApplication>
#include <QDir>

Q_GLOBAL_STATIC(HandleldTerminal::Internal::QemuSettings, settings)

namespace HandleldTerminal {
namespace Internal {

QemuSettings::QemuSettings()
{
    QDir dir(QCoreApplication::applicationDirPath());
    if (dir.cd("qemu/bin")) {
        m_path = dir.absolutePath();
    }
}

QemuSettings *QemuSettings::instance()
{
    return settings();
}

QString QemuSettings::path() const
{
    return m_path;
}

QString QemuSettings::path(CpuType t) const
{
    if (t == HandleldTerminal::x86_64)
        return x86_64();
    else if (t == HandleldTerminal::aarch64)
        return aarch64();
    return QString();
}

QString QemuSettings::qemuImage() const
{
    return path("qemu-img");
}

QString QemuSettings::x86_64() const
{
    return path("qemu-system-x86_64");
}

QString QemuSettings::aarch64() const
{
    return path("qemu-system-aarch64");
}

QStringList QemuSettings::supported() const
{
    return {"x86_64", "arm64-v8a"};
}

QString QemuSettings::path(const QString &exe) const
{
    QDir dir(m_path);
    QString _exe = exe;
#ifdef Q_OS_WIN
    _exe.append(".exe");
#endif
    QString file = dir.filePath(_exe);
    if (QFile::exists(file)) {
        return file;
    }
    return QString();
}

}
}
