CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
project(qtmorph)

#
# morphologica setup
#

# From CMAKE_SYSTEM work out which of __OSX__, __GLN__, __NIX__ are required
if(CMAKE_SYSTEM MATCHES Linux.*)
  set(OS_FLAG "-D__GLN__")
elseif(CMAKE_SYSTEM MATCHES BSD.*)
  set(OS_FLAG "-D__NIX__")
elseif(APPLE)
  set(OS_FLAG "-D__OSX__")
else()
  message(ERROR "Operating system not supported: " ${CMAKE_SYSTEM})
endif()

# morphologica uses c++-17 language features
set(CMAKE_CXX_STANDARD 17)

# Add the host definition to CXXFLAGS along with other switches, depending on OS/Compiler
if (APPLE)
  set(CMAKE_CXX_FLAGS "${OS_FLAG} -Wall -Wfatal-errors -g -O3")
else()
  # This assumes a gcc compiler (or a gcc mimic like Clang)
  set(CMAKE_CXX_FLAGS "${OS_FLAG} -Wall -Wfatal-errors -g -O3 -Wno-unused-result -Wno-unknown-pragmas -march=native")
endif()

# Tell clang to be quiet about brace initialisers:
if(CMAKE_CXX_COMPILER_ID MATCHES Clang)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-braces")
endif()

# Additional GL compiler flags.
#
set(OpenGL_GL_PREFERENCE "GLVND") # Following `cmake --help-policy CMP0072`
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGL3_PROTOTYPES -DGL_GLEXT_PROTOTYPES")
if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGL_SILENCE_DEPRECATION")
endif()

# Tell the program where the morph fonts are, to compile them into the binary
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMORPH_FONTS_DIR=\"\\\"${PROJECT_SOURCE_DIR}/morphologica/fonts\\\"\"")

# Find the libraries which will be needed
#find_package(HDF5 REQUIRED)
#find_package(Armadillo REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)

# Define collections of includes that have to be made for morphologica
#set(MORPH_INC_CORE ${ARMADILLO_INCLUDE_DIR} ${ARMADILLO_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR})
set(MORPH_INC_GL ${OPENGL_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS})
include_directories(${MORPH_INC_CORE} ${MORPH_INC_GL})
# Assuming that you installed morphologica in-tree (i.e. 'next to' schnakenberg.cpp).
set(MORPH_INCLUDE_PATH "${PROJECT_SOURCE_DIR}/morphologica" CACHE PATH "The path to morphologica")
include_directories(BEFORE ${MORPH_INCLUDE_PATH}/include) # Allows GL3/gl3.h to be found
include_directories(BEFORE ${MORPH_INCLUDE_PATH})         # Allows morph/Header.h to be found

#set(MORPH_LIBS_CORE ${ARMADILLO_LIBRARY} ${ARMADILLO_LIBRARIES} ${HDF5_C_LIBRARIES})
set(MORPH_LIBS_GL OpenGL::GL Freetype::Freetype)

#
# Qt setup
#

#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# setup for qt packages
#http://www.kdab.com/using-cmake-with-qt-5/
# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Also switch these on as they're on in QtCreator-generated projects
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Gui Core Widgets REQUIRED)

#
# All the library finding is now done. It's time to compile the program.
#

include_directories(${qtmorph_SOURCE_DIR})

#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG_INIT} -D_DEBUG -D_DRAWGI -Wno-deprecated-declarations")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE_INIT} -D_SECURE_SCL=0 -Wno-deprecated-declarations")

# I initially had a dedicated qwindow class derived from
# openglwindow. This would have provided identify functionality to
# GLFW, but without any benefit from the additional links to Qt. The
# right way to do Visual/Qt is to have the Visual enclosed in a
# QWidget, so it can go anywhere in a Qt UI, so I'm proceed with that;
# see qtwidget, below.
#
# Note that we have to compile both our own cpp files (main.cpp) AND
# the special source file openglwindow.cpp. This is something to do
# with the auto compilation of moc files and I don't know a way round it.
#add_executable(qtmorph main.cpp morphologica/morph/qt/openglwindow.cpp)
# Link to Qt libraries and to morphologica OpenGL stuff
#target_link_libraries(qtmorph Qt5::Gui Qt5::Core ${MORPH_LIBS_GL})

# Original example program - draws the graph1 example
add_subdirectory (app1)
# An empty MainWindow based Qt desktop application
add_subdirectory (app2)
# An empty QtWidget based Qt desktop application
add_subdirectory (app3)
