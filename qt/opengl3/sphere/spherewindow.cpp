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
    this->angularSpeed *= 0.97;

    // Stop rotation when speed goes below threshold
    if (this->angularSpeed < 0.01) {
        this->angularSpeed = 0.0;
    } else {
        // Update rotation
        this->rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * this->rotation;

        // Request an update
        this->renderNow();
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
                                                    "uniform mat4 mvp_matrix;\n"
                                                    "in vec3 position;\n"
                                                    "in vec3 color;\n"
                                                    "out vec4 fragColor;\n"
                                                    "void main() {\n"
                                                    " fragColor = vec4(color, 1.0);\n"
                                                    " gl_Position = (mvp_matrix * vec4(position, 1.0));\n"
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

    // Set the perspective from the width/height
    this->setPerspective (this->width(), this->height());

    // Start the update timer
    timer.start (12, this);
}

void SphereWindow::setPerspective (int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);
    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;
    // Reset projection
    this->projection.setToIdentity();
    // Set perspective projection
    this->projection.perspective (fov, aspect, zNear, zFar);
}

void SphereWindow::render()
{
    //cout << "SphereWindow::render()" << endl;
    const qreal retinaScale = devicePixelRatio();
    glViewport (0, 0, this->width() * retinaScale, this->height() * retinaScale);

    // Set the perspective from the width/height
    this->setPerspective (this->width(), this->height());

    // Calculate model view transformation
    QMatrix4x4 rotmat;
    rotmat.translate (0.0, 0.0, -0.50);
    rotmat.rotate (this->rotation);
    //qDebug() << rotmat;

    this->shaderProg->bind();
    // Set modelview-projection matrix
    this->shaderProg->setUniformValue ("mvp_matrix", /* this->projection * */ rotmat);

    // Clear
    glClear (GL_COLOR_BUFFER_BIT);

    this->sphere->render();

    this->shaderProg->release();
}
