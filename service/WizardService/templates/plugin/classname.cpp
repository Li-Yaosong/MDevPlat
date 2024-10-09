#include <QtPlugin>
#include <QDebug>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QWidget>
#else
#include <QWidget>
#endif

#include <IPluginProxy>
%INCLUDE%

using namespace Common;

%ClassName%::%ClassName%(QObject* parent)
    : m_proxy(NULL)
{
    Q_UNUSED(parent)
	// 插件资源初始化
    // 如果插件作为扩展点定义方，建议在此处初始化主窗口等
}

%ClassName%::~%ClassName%()
{
	// 插件资源释放
}

/*!
 * \brief 启动插件（本函数由框架负责调用，用于插件的启动）
 * \a proxy 插件代理
 */
void %ClassName%::start(Common::IPluginProxy* proxy)
{
	// 保存插件代理		 注意：不允许修改及删除
    m_proxy = proxy;   
    if(NULL == m_proxy)
    {
        return;
    }
	
	// 在插件启动时可以在此处注册服务、获取服务、注册事件监听等操作
    // 注：如果在start（）流程中进行了资源的获取，则必须在stop（）流程中进行资源的释放
    registServices();
    getServices();
}

/*!
 * \brief 停止插件（本函数由框架负责调用，用于插件的停止）
 * \a proxy 插件代理
 */
void %ClassName%::stop(Common::IPluginProxy* proxy)
{
    m_proxy = proxy;
    if(NULL == m_proxy)
    {
        return;
    }
	// 在插件停止时不需要自己停止提供服务、释放服务使用、释放事件监听等操作，这些操作都由框架自动完成
    // 注：如果在start（）流程中进行了资源的获取，则必须在stop（）流程中进行资源的释放
}

/*!
 * \brief 注册服务（无服务提供时可忽略）
 */
bool %ClassName%::registServices()
{
    if(NULL == m_proxy)
    {
        return false;
    }

    return true;
    //在此处注册服务（可在此处添加代码注册向导中未配置的其它服务）
	/*!	手动注册服务示例（手动添加代码注册的服务不能被开发向导直接识别即服务使用方也需要手动配置获取服务）
    *   手动注册服务请使用插件代理proxy调用服务注册接口registerInterface，比如本插件将对外提供服务IAddtion，
    *   实现服务的类名是Addtion，具体提供的服务函数是 int add(int& arg1, int& arg2),代码模型如下：
        ========================================================
        iaddtion.h文件
        #include <QObject>
        class IAddtion
        {
            ......

            virtual int add(int& arg1, int& arg2) = 0;

            ......
        };
        ========================================================
        addtion.h文件
        #include "iaddtion.h"
        class Addtion : public QObject, public IAddtion
        {
            ......

            Q_INVOKABLE int add(int& arg1, int& arg2);

            ......

        };
        ========================================================
        addtion.cpp文件
        #include "addtion.h"

        ......

        int Addtion::add(int& arg1, int& arg2)
        {
            return arg1 + arg2;
        }

        ......
        ========================================================
    *   则可使用如下示例方式进行注册服务：
		在本类的声明文件中添加服务头文件：#include "iaddtion.h"
                在本类的声明中添加成员变量：IAddtion* m_addtion;
		在本类的实现文件中添加服务类头文件：#include "addtion.h"
		在本类的实现文件中的构造函数中初始化服务对象：m_addtion = new Addtion(this);
		
		在此处添加服务对象注册代码：
		if(NULL != m_addtion)
		{
                        m_proxy->registerInterface("IAddtion", m_addtion);
		}
	*/

}

/*!
 * \brief 获取服务
 */
bool %ClassName%::getServices()
{
    if(NULL == m_proxy)
    {
        return false;
    }

    return true;
    //在此处获取服务（可在此处添加代码获取向导中未选择的其它服务）
    /*!	获取服务的使用示例,比如获取IAddtion服务，并使用服务函数int add(int& arg1, int& arg2)
    QList<InterfaceRegistration> regList = m_proxy->getInterfaces("IAddtion");
    foreach (const InterfaceRegistration& reg, regList)
    {
        int sub = 0;
        int arg1 = 1;
        int arg2 = 1;

        // 通过服务注册项获取服务对象指针，并通过服务对象指针直接调用接口（该方式仅适用于服务提供方与服务使用方都是动态库插件的情况）。
        IAddtion* iaddtion = qobject_cast<IAddtion*>(reg.getService());
        if (NULL != iaddtion)
        {
            sub = iaddtion->add(arg1, arg2);
        }

        // 通过服务注册项的callInterface调用接口（该方式适用于任何插件类型）。
        // 注：通过callInterface最多向服务函数传递八个参数
        reg.callInterface("add", QtEZ_RETURN_ARG(int, sub), QtEZ_ARG(int, arg1), QtEZ_ARG(int, arg2));
    }
	*/

}

Q_EXPORT_PLUGIN2(%PluginName%, %ClassName%)
