#include <QtCore/qmath.h>
#include "spheregeometry.h"
#include <iostream>

using std::cout;
using std::endl;

SphereGeometry::SphereGeometry(QOpenGLShaderProgram *program)
    : vertexLocation(0)
    , colorLocation(0)
    , matrixLocation(0)
    , pvbo(QOpenGLBuffer::VertexBuffer)
    , cvbo(QOpenGLBuffer::VertexBuffer)
{
    this->shaderProgram = program;
    cout << "shaderProgram: " << (unsigned long long int)this->shaderProgram << endl;
    this->initialize();
}

void SphereGeometry::initialize()
{
    cout << "SphereGeometry::init, innit?" << endl;

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

    this->vao.create();
    this->vao.bind(); // sets the Vertex Array Object current to the OpenGL context so we can write attributes to it

    // Binds data from the "C++ world" to the OpenGL shader world for "position"
    //QOpenGLBuffer pvbo(QOpenGLBuffer::VertexBuffer);
    pvbo.create();
    pvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    pvbo.bind();
    pvbo.allocate(this->vertexPositions.data(), this->vertexPositions.size() * sizeof(float));

    // this labels an attribute "position" that points to the memory
    // slot from the last buffer allocate() the position attribute is
    // an input to our vertex shader
    this->shaderProgram->enableAttributeArray("position");
    this->shaderProgram->setAttributeBuffer("position", GL_FLOAT, 0, 3);

    //QOpenGLBuffer cvbo(QOpenGLBuffer::VertexBuffer);
    cvbo.create();
    cvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    cvbo.bind();
    cvbo.allocate(this->vertexColors.data(), this->vertexColors.size() * sizeof(float));

    // this labels an attribute "color" that points to the memory slot
    // from the last buffer allocate() the color attribute is an input
    // to our vertex shader
    this->shaderProgram->enableAttributeArray("color");
    this->shaderProgram->setAttributeBuffer("color", GL_FLOAT, 0, 3);

    // Release (unbind) all
    pvbo.release();
    cvbo.release();
    this->vao.release();
}

void SphereGeometry::render()
{
    //pvbo.bind();
    //this->shaderProgram->enableAttributeArray("position");
    //this->shaderProgram->setAttributeBuffer("position", GL_FLOAT, 0, 3);

    //cvbo.bind();
    //this->shaderProgram->enableAttributeArray("color");
    //this->shaderProgram->setAttributeBuffer("color", GL_FLOAT, 0, 3);

    // Render using our shader
    this->shaderProgram->bind();
    cout << "after bind, shaderProgram: " << (unsigned long long int)this->shaderProgram << endl;
    this->vao.bind(); // sets this vertex array object as the one to use.

    cout << "Num triangles is " << this->vertexPositions.size()/3 << endl;
    glDrawArrays(GL_TRIANGLES, 0, this->vertexPositions.size()/3);

    this->vao.release();
    this->shaderProgram->release();
}
