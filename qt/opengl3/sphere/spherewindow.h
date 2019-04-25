#ifndef _SPHEREWINDOW_H_
#define _SPHEREWINDOW_H_

#include "openglwindow.h"
#include "spheregeometry.h"
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>

#include <QQuaternion>
#include <QVector2D>
#include <QMouseEvent>

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

private:
    QVector2D mousePressPosition;
    QVector3D rotationAxis;
    qreal angularSpeed;
    QQuaternion rotation;

    QOpenGLShaderProgram* shaderProg;
    SphereGeometry* sphere;
};

#endif // _SPHEREWINDOW_H_
