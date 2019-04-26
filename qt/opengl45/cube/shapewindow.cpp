#include <QtGui/QScreen>
#include <QtCore/qmath.h>
#include "shapewindow.h"
#include <iostream>

using std::cout;
using std::endl;

ShapeWindow::ShapeWindow()
{
}

void ShapeWindow::mousePressEvent(QMouseEvent *e)
{
    // Save mouse press position
    mousePressPosition = QVector2D(e->localPos());
}

void ShapeWindow::mouseReleaseEvent(QMouseEvent *e)
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

void ShapeWindow::timerEvent(QTimerEvent *)
{
    // Decrease angular speed (friction)
    this->angularSpeed *= 0.996;

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

void ShapeWindow::initialize()
{
    initializeOpenGLFunctions();
    // One can also get a pointer to OpenGLFunctions:
    // QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
    // and use them like this:
    // f->glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    glClearColor (0.8f, 0.8f, 0.8f, 1.0f);

    // initialize shaders
    this->shaderProg = new QOpenGLShaderProgram();
    // GLSL version 140(1.4) == OpenGL 3.1, GLSL 330(3.3) == OpenGL 3.3, GLSL 450(4.5) == OpenGL 4.5
    if (!this->shaderProg->addShaderFromSourceCode (QOpenGLShader::Vertex,
                                                    "#version 450\n"
                                                    "uniform mat4 mvp_matrix;\n"
                                                    "in vec4 position;\n"
                                                    "in vec4 normalin;\n"
                                                    "in vec4 color;\n"
                                                    "out vec4 normal;\n"
                                                    "out vec4 fragColor;\n"
                                                    "void main() {\n"
                                                    " fragColor = color;\n"
                                                    " normal = normalin;\n" // not working, apparently
                                                    " gl_Position = (mvp_matrix * position);\n"
                                                    "}\n")) {
        close();
    }

    if (!this->shaderProg->addShaderFromSourceCode (QOpenGLShader::Fragment,
                                                    "#version 450\n"
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

    // Enable depth buffer
    //glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    //glEnable(GL_CULL_FACE);

    // Create the cube geometry. This creates VAO
    this->cube = new CubeGeometry (this->shaderProg);

    // Now VAO was created in CubeGeometry object, release shaderProg
    this->shaderProg->release();

    // Set the perspective from the width/height
    this->setPerspective (this->width(), this->height());

    // Start the update timer. Every 8 ms fast enough for 120 Hz screen.
    timer.start (8, this);
}

void ShapeWindow::setPerspective (int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);
    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 0.1, zFar = 5.0, fov = 35.0;
    // Reset projection
    this->projection.setToIdentity();
    // Set perspective projection
    this->projection.perspective (fov, aspect, zNear, zFar);
}

void ShapeWindow::render()
{
    //cout << "ShapeWindow::render()" << endl;
    const qreal retinaScale = devicePixelRatio();
    glViewport (0, 0, this->width() * retinaScale, this->height() * retinaScale);

    // Set the perspective from the width/height
    this->setPerspective (this->width(), this->height());

    // Calculate model view transformation
    QMatrix4x4 rotmat;
    rotmat.translate (0.0, 0.0, -1.50);
    rotmat.rotate (this->rotation);
    //qDebug() << rotmat;

    this->shaderProg->bind();
    // Set modelview-projection matrix
    this->shaderProg->setUniformValue ("mvp_matrix", this->projection * rotmat);

    // Clear
    glClear (GL_COLOR_BUFFER_BIT);

    this->cube->render();

    this->shaderProg->release();
}
