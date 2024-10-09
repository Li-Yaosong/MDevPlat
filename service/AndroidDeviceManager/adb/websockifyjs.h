#ifndef WEBSOCKIFYJS_H
#define WEBSOCKIFYJS_H

#include <QObject>
#include <QHash>
class ProcessHandler;
class WebsockifyJS : public QObject
{
    Q_OBJECT
    //只读属性
    Q_PROPERTY(QString path READ path CONSTANT)
    Q_PROPERTY(QString websockifyPath READ websockifyPath CONSTANT)
public:

    static WebsockifyJS *instance();
    QString path() const;
    QString websockifyPath() const;

    Q_INVOKABLE QString start(const QString &target);

private:
    explicit WebsockifyJS(QObject *parent = nullptr);
    QString m_path;
    QString m_websockifyPath;
    // QHash<QString, ProcessHandler *> m_processes;
    ProcessHandler *m_process = nullptr;
    static WebsockifyJS *m_instance;
};

#endif // WEBSOCKIFYJS_H
