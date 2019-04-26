#ifndef _SPHEREWINDOW_H_
#define _SPHEREWINDOW_H_

#include "openglwindow.h"
#include "spheregeometry.h"
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>

#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QMouseEvent>
#include <QBasicTimer>

#include <vector>

// header style declaration
class SphereWindow : public OpenGLWindow
{
public:
    SphereWindow();

    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void timerEvent(QTimerEvent*) override;

    void initialize() override;
    void render() override;

    void setPerspective (int w, int h);

private:
    QVector2D mousePressPosition;
    QVector3D rotationAxis;
    qreal angularSpeed;
    QQuaternion rotation;
    QMatrix4x4 projection;

    QBasicTimer timer;

    QOpenGLShaderProgram* shaderProg;
    SphereGeometry* sphere;
};

#endif // _SPHEREWINDOW_H_
