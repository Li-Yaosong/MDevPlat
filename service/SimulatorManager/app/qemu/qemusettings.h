#ifndef QEMUSETTINGS_H
#define QEMUSETTINGS_H

#include "handleldterminaldefines.h"

#include <QString>

#define qQemu HandleldTerminal::Internal::QemuSettings::instance()

namespace HandleldTerminal {
namespace Internal {

class QemuSettings
{
public:
    QemuSettings();

    static QemuSettings *instance();

    QString path() const;

    QString path(HandleldTerminal::CpuType t) const;

    QString qemuImage() const;

    QString x86_64() const;

    QString aarch64() const;

    QStringList supported() const;

private:
    QString path(const QString &exe) const;
private:
    QString m_path;
};

}
}
#endif // QEMUSETTINGS_H
