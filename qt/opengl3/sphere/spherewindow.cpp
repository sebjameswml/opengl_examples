#include <QtGui/QScreen>
#include <QtCore/qmath.h>
#include "spherewindow.h"
#include <iostream>

using std::cout;
using std::endl;

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
        this->render();
    }
}

void SphereWindow::initialize()
{
    initializeOpenGLFunctions();
    // One can also get a pointer to OpenGLFunctions:
    // QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
    // and use them like this:
    // f->glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    glClearColor (0.0f, 0.5f, 1.0f, 1.0f);

    // initialize shaders
    this->shaderProg = new QOpenGLShaderProgram();
    // GLSL version 140(1.4) == OpenGL 3.1, GLSL 330(3.3) == OpenGL 3.3, GLSL 450(4.5) == OpenGL 4.5
    if (!this->shaderProg->addShaderFromSourceCode (QOpenGLShader::Vertex,
                                                    "#version 140\n"
                                                    "in vec3 position;\n"   // attribute named position with 3 elements per vertex in
                                                    "in vec3 color;\n"      // A colour attribute
                                                    "out vec4 fragColor;\n" // vec4: a vector of 4 floats
                                                    "void main() {\n"
                                                    " fragColor = vec4(color, 1.0);\n"
                                                    " gl_Position = vec4(position, 1.0);\n"
                                                    "}\n")) {
        close();
    }

    if (!this->shaderProg->addShaderFromSourceCode (QOpenGLShader::Fragment,
                                                    "#version 140\n"
                                                    "in vec4 fragColor;\n"
                                                    "out vec4 finalcolor;\n"
                                                    "void main() {\n"
                                                    " finalcolor = fragColor;\n"
                                                    "}\n")) {
        close();
    }

    if (!this->shaderProg->link()) {
        close();
    }

    if (!this->shaderProg->bind()) { // bind Shader (Do not release until VAO is created)
        close();
    }

    // Create the sphere geometry. This creates VAO
    this->sphere = new SphereGeometry (this->shaderProg);

    // Now VAO was created in SphereGeometry object, release shaderProg
    this->shaderProg->release();
}

void SphereWindow::render()
{
    cout << "SphereWindow::render()" << endl;
    const qreal retinaScale = devicePixelRatio();
    glViewport (0, 0, width() * retinaScale, height() * retinaScale);

    // Clear
    glClear (GL_COLOR_BUFFER_BIT);

    this->sphere->render();
}
