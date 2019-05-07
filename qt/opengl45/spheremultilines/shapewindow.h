#ifndef _SHAPEWINDOW_H_
#define _SHAPEWINDOW_H_

#include "openglwindow.h"
#include "scene.h"
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
class ShapeWindow : public OpenGLWindow
{
public:
    ShapeWindow();
    ~ShapeWindow();

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
    Scene* thescene;
};

#endif // _SHAPEWINDOW_H_
