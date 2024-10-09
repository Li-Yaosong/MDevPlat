cmake_minimum_required(VERSION 3.5)

project(app)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES 
	main.cpp
)

set(EXECUTABLE_OUTPUT_PATH ${ROOT_DIR})

add_executable(${PROJECT_NAME}
	${SOURCES}
)

#example(export QT5_DIR=/home/cyh/Qt5.12.12/5.12.12/gcc_64)
set(CMAKE_PREFIX_PATH $ENV{QT5_DIR})
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets)

set(LIBINCLUDEDIRS
    $ENV{QT5_DIR}/include/MicroKernel
)

set(LIBFILES
    $ENV{QT5_DIR}/lib/libMicroKernel4.so
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath,./")

include_directories(${LIBINCLUDEDIRS})
target_link_libraries(${PROJECT_NAME} ${LIBFILES})
