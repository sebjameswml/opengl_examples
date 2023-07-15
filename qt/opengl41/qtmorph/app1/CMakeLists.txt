add_executable(qtwidget widget.cpp)
target_link_libraries(qtwidget Qt5::Gui Qt5::Widgets Qt5::Core ${MORPH_LIBS_GL})
