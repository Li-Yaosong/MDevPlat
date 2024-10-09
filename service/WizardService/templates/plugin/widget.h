#ifndef %HEADER_H%
#define %HEADER_H%

#include <%BaseClass%>
#include <QObjectRegister>

namespace Ui {
class %WidgetName%;
}

class %WidgetName% : public %BaseClass%
{
    Q_OBJECT
    Q_OBJECT_REGISTER(%WidgetName%, "")
public:
    Q_INVOKABLE explicit %WidgetName%(QWidget* parent = 0);
    ~%WidgetName%();
private:
    Ui::%WidgetName%* ui;
};

#endif // %HEADER_H%
