#include "pathlabel.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QFontMetrics>

PathLabel::PathLabel(QWidget *parent)
    : QFrame(parent)
{
    m_layout = new QHBoxLayout(this);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    setFrameStyle(QFrame::Box);
    setFrameRect(QRect());
    setFrameShadow(QFrame::Raised);
}

void PathLabel::setPath(const QString &path)
{
    for (int i = 0; i < m_buttons.count(); ++i) {
        m_buttons.at(i)->hide();
    }
    QString _path = path;
    append("Simulator:");
    if (_path.startsWith("/")) {
        _path.remove(0, 1);
    }
    if (_path.endsWith("/")) {
        _path.chop(1);
    }
    if (_path.isEmpty())
        return;
    QStringList dirs = _path.split("/");
    for (int i = 0; i < dirs.count(); ++i) {
        append(dirs.at(i));
    }
}

void PathLabel::append(const QString &dir)
{
    for (int i = 0; i < m_buttons.count(); ++i) {
        QPushButton *button = m_buttons.at(i);
        if (button->isHidden()) {
            button->setText(dir);
            button->show();
            return;
        }
    }
    QPushButton *button = new QPushButton(dir);
    m_layout->addWidget(button, 0, Qt::AlignLeft | Qt::AlignVCenter);
    connect(button, &QPushButton::clicked, this, &PathLabel::handlePath);
    m_buttons << button;
}

void PathLabel::handlePath()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button == nullptr)
        return;

    int index = m_layout->indexOf(button);
    Q_ASSERT(index != -1);
    QString path = "/";
    QStringList dirs;
    for (int i = 1; i <= index; ++i) {
        dirs << m_buttons.at(i)->text();
    }
    path += dirs.join("/");
    for (int i = index + 1; i < m_buttons.count(); ++i) {
        m_buttons.at(i)->hide();
    }
    emit pathChanged(path);
}
