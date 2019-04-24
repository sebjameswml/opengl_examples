#include <QtGui/QScreen>
#include <QtCore/qmath.h>
#include "spherewindow.h"
#include <iostream>

SphereWindow::SphereWindow()
{
}

void SphereWindow::mousePressEvent(QMouseEvent *e)
{
    // Save mouse press position
    mousePressPosition = QVector2D(e->localPos());
}

void SphereWindow::mouseReleaseEvent(QMouseEvent *e)
{
    // Mouse release position - mouse press position
    QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;

    // Rotation axis is perpendicular to the mouse position difference
    // vector
    QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

    // Accelerate angular speed relative to the length of the mouse sweep
    qreal acc = diff.length() / 100.0;

    // Calculate new rotation axis as weighted sum
    this->rotationAxis = (this->rotationAxis * this->angularSpeed + n * acc).normalized();

    // Increase angular speed
    this->angularSpeed += acc;
}

void SphereWindow::timerEvent(QTimerEvent *)
{
    // Decrease angular speed (friction)
    this->angularSpeed *= 0.99;

    // Stop rotation when speed goes below threshold
    if (this->angularSpeed < 0.01) {
        this->angularSpeed = 0.0;
    } else {
        // Update rotation
        this->rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

        // Request an update
        this->sphere->render();
    }
}

void SphereWindow::initialize()
{
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    // "initShaders"
    this->shaderProg = new QOpenGLShaderProgram();
    this->shaderProg->addShaderFromSourceCode (QOpenGLShader::Vertex,
                                               "#version 450\n"        // GLSL version 1.4 == OpenGL 3.1. GLSL 4.5 == OpenGL 4.5
                                               "in vec3 position;\n"   // attribute named position with 3 elements per vertex in
                                               "in vec3 color;\n"      // A colour attribute
                                               "out vec4 fragColor;\n" // vec4: a vector of 4 floats
                                               "void main() {\n"
                                               " fragColor = vec4(color, 1.0);\n"
                                               " gl_Position = vec4(position, 1.0);\n"
                                               "}\n");
    this->shaderProg->addShaderFromSourceCode (QOpenGLShader::Fragment,
                                               "#version 450\n" // GLSL version 4.5
                                               "in vec4 fragColor;\n"
                                               "out vec4 finalcolor;\n"
                                               "void main() {\n"
                                               " finalcolor = fragColor;\n"
                                               "}\n");

    this->shaderProg->link();
    this->shaderProg->bind(); // bind Shader (Do not release until VAO is created)

    // Create the sphere geometry
    this->sphere = new SphereGeometry (this->shaderProg);
}

void SphereWindow::render()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    // Clear
    glClear(GL_COLOR_BUFFER_BIT);

    this->sphere->render();
}
