/*
 * Qt morphologica example
 */

#include <QtGui/QGuiApplication>

#define USING_QT 1
#include <morph/Visual.h>

int main (int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    morph::Visual v(1024,768,"Qt test"); // Becomes a thing *containing* an OpenGLWindow that gets shown
    v.addLabel ("morphologica in a Qt window!", {0,0,0});

    // Do your addVisualModel setup here...

    // during app.exec(), then OpenGLWIndow gets its render called, which calls
    // Visual::render() via a callback. That's the plan. Also need to figure out how to
    // run dynamic stuff with Qt.
    return app.exec();
}
