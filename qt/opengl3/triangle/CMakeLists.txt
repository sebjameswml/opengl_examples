project(ModernTriangle)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#setup for qt packages
#http://stackoverflow.com/questions/14115024/how-to-link-qtmain-in-cmake-with-qt5
#http://www.kdab.com/using-cmake-with-qt-5/
# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Gui)
find_package(Qt5Core)

SET(Main_SRC
./main.cpp
./main.h
./openglwindow.cpp
./openglwindow.h
)
SOURCE_GROUP(Main FILES ${Main_SRC})

INCLUDE_DIRECTORIES(${ModernTriangle_SOURCE_DIR})

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG_INIT} -D_DEBUG -D_DRAWGI -Wno-deprecated-declarations")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE_INIT} -D_SECURE_SCL=0 -Wno-deprecated-declarations")

ADD_EXECUTABLE(ModernTriangle ${Main_SRC})

TARGET_LINK_LIBRARIES(ModernTriangle Qt5::Gui Qt5::Core)
