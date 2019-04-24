#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QScreen>
#include <QtCore/qmath.h>
#include "sphere.h"
#include <iostream>

using std::vector;

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize( 4 );
    format.setSamples(24);
    format.setVersion(3, 1);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    //format.setProfile( QSurfaceFormat::CoreProfile );

    Sphere window;
    window.setFormat(format);
    window.resize(640, 480);
    window.show();

    return app.exec();
}
