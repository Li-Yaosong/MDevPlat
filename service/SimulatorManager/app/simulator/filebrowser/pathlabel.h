#ifndef PATHLABEL_H
#define PATHLABEL_H

#include <QFrame>
class QHBoxLayout;
class QPushButton;

class PathLabel : public QFrame
{
    Q_OBJECT
public:
    PathLabel(QWidget *parent = nullptr);

    void setPath(const QString &path);

    void append(const QString &dir);

signals:
    void pathChanged(const QString &path);

private slots:
    void handlePath();
private:
    QHBoxLayout *m_layout;
    QList<QPushButton *> m_buttons;
};

#endif // PATHLABEL_H
