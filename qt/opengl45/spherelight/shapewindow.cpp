#include <QtGui/QScreen>
#include <QtCore/qmath.h>
#include "shapewindow.h"
#include <iostream>

using std::cout;
using std::endl;

ShapeWindow::ShapeWindow()
{
}

ShapeWindow::~ShapeWindow()
{
    delete this->sphere;
    delete this->shaderProg;
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
    // initializeOpenGLFunctions() is called in OpenGLWindow::renderNow()

    // One can also get a pointer to OpenGLFunctions:
    // QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
    // and use them like this:
    // f->glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    glClearColor (0.8f, 0.7f, 0.8f, 1.0f);

    // initialize shaders
    this->shaderProg = new QOpenGLShaderProgram();

    // Add shaders from files, making it easier to read/modify the shader code
    if (!this->shaderProg->addShaderFromSourceFile (QOpenGLShader::Vertex, "../vshader.glsl")) {
        close();
    }
    if (!this->shaderProg->addShaderFromSourceFile (QOpenGLShader::Fragment, "../fshader.glsl")) {
        close();
    }

    if (!this->shaderProg->link()) {
        close();
    }

    if (!this->shaderProg->bind()) { // bind Shader (Do not release until VAO is created)
        close();
    }

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    //glEnable(GL_CULL_FACE);

    // Create the shape geometry. This creates VAO
    this->sphere = new SphereGeometry (this->shaderProg);

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
    const qreal zNear = 1.5, zFar = 5.0, fov = 45.0;
    // Reset projection
    this->projection.setToIdentity();
    // Set perspective projection
    this->projection.perspective (fov, aspect, zNear, zFar);
}

void ShapeWindow::render()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport (0, 0, this->width() * retinaScale, this->height() * retinaScale);

    // Set the perspective from the width/height
    this->setPerspective (this->width(), this->height());

    // Calculate model view transformation
    QMatrix4x4 rotmat;
    rotmat.translate (0.0, 0.0, -3.50); // send backwards into distance
    rotmat.rotate (this->rotation);

    // Bind shader program...
    this->shaderProg->bind();

    // Set modelview-projection matrix
    this->shaderProg->setUniformValue ("mvp_matrix", this->projection * rotmat);

    // Lighting
    QMatrix4x4 modelmat;
    rotmat.translate (0.0, 0.0, -3.50); // send backwards into distance
    this->shaderProg->setUniformValue ("model", modelmat);
    QVector3D lpos = { 13.0f, 0.2f, 0.4f };
    QVector3D lcol = { 10.0f, 10.0f, 10.0f };
    this->shaderProg->setUniformValue ("light.position", lpos);
    this->shaderProg->setUniformValue ("light.intensities", lcol);

    // Clear color buffer and **also depth buffer**
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->sphere->render();

    // ...and release the shaderProg
    this->shaderProg->release();
}
