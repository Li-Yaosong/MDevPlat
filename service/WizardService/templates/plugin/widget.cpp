#include "%INCLUDE%"
#include "%UIFILE_HEADER%"

%WidgetName%::%WidgetName%(QWidget* parent)
	: %BaseClass%(parent)
	, ui(new Ui::%WidgetName%)
{
    ui->setupUi(this);
}

%WidgetName%::~%WidgetName%()
{
    delete ui;
}
