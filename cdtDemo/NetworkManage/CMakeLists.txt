cmake_minimum_required(VERSION 3.5)

project(NetworkManage)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
SET(CMAKE_CXX_STANDARD 11)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall")

SET(CMAKE_BUILD_TYPE "Release")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES 
	main.cpp
	widget.cpp
        microdata.cpp
        netutils.cpp
)

set(HEADERS
        widget.h
        microdata.h
        private/microdata_p.h
        private/netutils_p.h
        netutils.h
)

set(FORMS
    widget.ui 
)

#set(QRC_FILES
#    app.qrc
#)
#qt5_add_resources(QRC_HEADERS ${QRC_FILES})

add_executable(${PROJECT_NAME}
	${HEADERS}
	${SOURCES}
	${FORMS}
)

SET(CMAKE_PREFIX_PATH $ENV{QT5_DIR})
find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)
