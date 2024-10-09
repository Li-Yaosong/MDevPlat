#ifndef %HEADER_H%
#define %HEADER_H%

#include <QObject>
#include <PluginActivator>
%INCLUDE%

namespace Common {
    class IPluginProxy;
}

class %SHARED_EXPORT% %ClassName% : public QObject        
        ,public Common::PluginActivator
{
    Q_OBJECT
    Q_INTERFACES(Common::PluginActivator)
%COMPAREVERSION%
public:
    %ClassName%(QObject* parent = 0);
    ~%ClassName%();

    //启动插件（本函数由框架负责调用，用于插件的启动）
    void start(Common::IPluginProxy* proxy);
    //停止插件（本函数由框架负责调用，用于插件的停止）
    void stop(Common::IPluginProxy* proxy);

private:
    //注册服务（无服务提供时可忽略）
    bool registServices();
    //获取服务（不使用服务时可忽略）
    bool getServices();

private:
    //插件代理，用于同内核交互获取插件信息
    Common::IPluginProxy* m_proxy;
};

#endif //%HEADER_H%
