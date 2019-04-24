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
        render();
    }
}

void SphereWindow::initialize()
{
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_program = new QOpenGLShaderProgram();
    m_program->addShaderFromSourceCode (QOpenGLShader::Vertex,
                                        "#version 450\n"        // GLSL version 1.4
                                        "in vec3 position;\n"   // attribute named position with 3 elements per vertex in
                                        "in vec3 color;\n"      // A colour attribute
                                        "out vec4 fragColor;\n" // vec4: a vector of 4 floats
                                        "void main() {\n"
                                        " fragColor = vec4(color, 1.0);\n"
                                        " gl_Position = vec4(position, 1.0);\n"
                                        "}\n");
    m_program->addShaderFromSourceCode (QOpenGLShader::Fragment,
                                        "#version 450\n" //GLSL version 1.4
                                        "in vec4 fragColor;\n"
                                        "out vec4 finalcolor;\n"
                                        "void main() {\n"
                                        " finalcolor = fragColor;\n"
                                        "}\n");

    m_program->link();
    m_program->bind(); // bind Shader (Do not release until VAO is created)

    /////////////////////////////////////////////////////
    // Sphere calculation - the CPU intensive bit
    // draw a sphere to represent a neuron
    int rings = 20;
    int segments = 30;
    float r = 0.6f;
    for (int i = 0; i <= rings; i++) {
        double rings0 = M_PI * (-0.5 + (double) (i - 1) / rings);
        double z0  = sin(rings0);
        double zr0 =  cos(rings0);

        double rings1 = M_PI * (-0.5 + (double) i / rings);
        double z1 = sin(rings1);
        double zr1 = cos(rings1);

        for (int j = 0; j <= segments; j++) {
            double segment = 2 * M_PI * (double) (j - 1) / segments;
            double x = cos(segment);
            double y = sin(segment);

            double x0 = x*zr0*r;
            double y0 = y*zr0*r;
            double x1 = x*zr1*r;
            double y1 = y*zr1*r;

            // I'm not rendering enough vertices, but this gets us started...
            this->vertex_push (x0,y0,z0);
            this->color_push (1.0f, 0.0f, 0.2f);

            this->vertex_push (x1,y1,z1);
            this->color_push (0.0f, 1.0f, 0.6f);
        }
    }
    ////////////////////////////////////////////////////

    m_vao.create();
    m_vao.bind(); // sets the Vertex Array Object current to the OpenGL context so we can write attributes to it

    // Binds data from the "C++ world" to the OpenGL shader world for "position"
    QOpenGLBuffer m_vvbo(QOpenGLBuffer::VertexBuffer);
    m_vvbo.create();
    m_vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vvbo.bind();
    m_vvbo.allocate(vertexPositions.data(), vertexColors.size() * sizeof(float));
    // this labels an attribute "position" that points to the memory
    // slot from the last buffer allocate() the position attribute is
    // an input to our vertex shader
    m_program->enableAttributeArray("position");
    m_program->setAttributeBuffer("position", GL_FLOAT, 0, 3);

    QOpenGLBuffer m_vcbo(QOpenGLBuffer::VertexBuffer);
    m_vcbo.create();
    m_vcbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vcbo.bind();
    m_vcbo.allocate(vertexColors.data(), vertexColors.size() * sizeof(float));
    // this labels an attribute "color" that points to the memory slot
    // from the last buffer allocate() the color attribute is an input
    // to our vertex shader
    m_program->enableAttributeArray("color");
    m_program->setAttributeBuffer("color", GL_FLOAT, 0, 3);

    // Release (unbind) all
    m_vvbo.release();
    m_vcbo.release();
    m_vao.release();
    m_program->release();
}

void SphereWindow::render()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    // Clear
    glClear(GL_COLOR_BUFFER_BIT);

    // Render using our shader
    m_program->bind();
    m_vao.bind(); //sets
    glDrawArrays(GL_TRIANGLES, 0, this->vertexPositions.size()/3);
    m_vao.release();
    m_program->release();
}
