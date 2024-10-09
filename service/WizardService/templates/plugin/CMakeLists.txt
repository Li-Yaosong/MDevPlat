cmake_minimum_required(VERSION 3.5)

project(%PluginName%)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
SET(CMAKE_CXX_STANDARD 11)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES 
    %SOURCES%
)

set(HEADERS
    %HEADERS%
)

%FORM%

set(LIBRARY_OUTPUT_PATH ${PLUGIN_DIR}/${PROJECT_NAME}/)
%TEMPLATETYPE%(${PROJECT_NAME} %ADDFILES%)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE %DEFINESSHARDLIBRARY%
    PRIVATE KERNEL_TARGET_NAME=\"${PROJECT_NAME}\"
)

SET(CMAKE_PREFIX_PATH $ENV{QT5_DIR})
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets)

set(LIBINCLUDEDIRS
    $ENV{QT5_DIR}/include/MicroKernel
)

set(LIBFILES
    $ENV{QT5_DIR}/lib/libMicroKernel4.so
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath,./:../../")

include_directories(${LIBINCLUDEDIRS})
target_link_libraries(${PROJECT_NAME} ${LIBFILES})

install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/MANIFEST.MF"
    "${CMAKE_CURRENT_SOURCE_DIR}/plug.xml"
    DESTINATION ${LIBRARY_OUTPUT_PATH})
