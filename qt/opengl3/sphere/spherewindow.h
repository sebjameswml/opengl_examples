#ifndef _SPHEREWINDOW_H_
#define _SPHEREWINDOW_H_

#include "openglwindow.h"
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

    void initialize() override;
    void render() override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void timerEvent(QTimerEvent *) override;

private:
    // Compute positions and colours of vertices for the sphere and
    // store in these:
    std::vector<float> vertexPositions;
    std::vector<float> vertexColors;

    GLuint vertexLocation;
    GLuint colorLocation;
    GLuint matrixLocation;

    QVector2D mousePressPosition;
    QVector3D rotationAxis;
    qreal angularSpeed;
    QQuaternion rotation;

    QOpenGLVertexArrayObject m_vao; // Our Vertex Array Object
    QOpenGLBuffer m_vvbo; // Our vertice Vertex Buffer Object
    QOpenGLBuffer m_vcbo; // Our color Vertex Buffer Object

    QOpenGLShaderProgram* m_program;

    void vertex_push (const float& x, const float& y, const float& z) {
        this->vertexPositions.push_back (x);
        this->vertexPositions.push_back (y);
        this->vertexPositions.push_back (z);
    }
    void color_push (const float& r, const float& g, const float& b) {
        this->vertexColors.push_back (r);
        this->vertexColors.push_back (g);
        this->vertexColors.push_back (b);
    }
};

#endif // _SPHEREWINDOW_H_
