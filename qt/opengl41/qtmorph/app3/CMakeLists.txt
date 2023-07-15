add_executable(app3 main.cpp widget.cpp widget.ui)
target_link_libraries(app3 Qt5::Gui Qt5::Widgets Qt5::Core ${MORPH_LIBS_GL})
