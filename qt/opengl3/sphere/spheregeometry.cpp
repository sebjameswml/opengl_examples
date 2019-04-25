#include <QtCore/qmath.h>
#include "spheregeometry.h"
#include <iostream>

using std::cout;
using std::endl;
using std::vector;
SphereGeometry::SphereGeometry(QOpenGLShaderProgram *program)
    : vertexLocation(0)
    , colorLocation(0)
    , matrixLocation(0)
    , pvbo(QOpenGLBuffer::VertexBuffer)
    , cvbo(QOpenGLBuffer::VertexBuffer)
{
    // This call is critical when using a separate "geometry" class like this
    initializeOpenGLFunctions();

    this->shaderProgram = program;
    cout << "shaderProgram: " << (unsigned long long int)this->shaderProgram << endl;
    this->initialize();
}

SphereGeometry::~SphereGeometry()
{
    pvbo.destroy();
    cvbo.destroy();
}

void SphereGeometry::initialize()
{
    cout << "SphereGeometry::init, innit?" << endl;
    QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();

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
            this->vertex_push (x0,y0,z0, vertexPositions);
            this->vertex_push (1.0f, 0.0f, 0.2f, vertexColors);

            this->vertex_push (x1,y1,z1, vertexPositions);
            this->vertex_push (0.0f, 1.0f, 0.6f, vertexColors);
        }
    }
    ////////////////////////////////////////////////////

    cout << "vp tri 1 = (" << vertexPositions[0] << ","<< vertexPositions[1] << ","<< vertexPositions[2] << ")" << endl;

    this->vao.create();
    this->vao.bind(); // sets the Vertex Array Object current to the OpenGL context so we can write attributes to it

    // Do pvbo and cvbo get bound to vao?

    // Binds data from the "C++ world" to the OpenGL shader world for "position"
    //QOpenGLBuffer pvbo(QOpenGLBuffer::VertexBuffer);
    if (pvbo.create() == false) {
        cout << "pvbo create returned false" << endl;
    }
    if (pvbo.isCreated() == true) {
        cout << "pvbo WAS created!" << endl;
    }
    pvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    if (pvbo.bind() == false) {
        cout << "pvbo bind failed" << endl;
    }
    pvbo.allocate(vertexPositions.data(), vertexPositions.size() * sizeof(float));

    // this labels an attribute "position" that points to the memory
    // slot from the last buffer allocate() the position attribute is
    // an input to our vertex shader
    this->shaderProgram->enableAttributeArray("position");
    this->shaderProgram->setAttributeBuffer("position", GL_FLOAT, 0, 3);

    //QOpenGLBuffer cvbo(QOpenGLBuffer::VertexBuffer);
    cvbo.create();
    cvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    cvbo.bind();
    cvbo.allocate(vertexColors.data(), vertexColors.size() * sizeof(float));

    this->numtri = vertexPositions.size()/3;

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
    // Render using our shader
    this->shaderProgram->bind();

#if 0 // Shows that OpenGL buffer contains the data
    pvbo.bind();
    vector<float> va(this->numtri * 3, 0);
    pvbo.read(0, va.data(), this->numtri * 3);
    pvbo.release();
    cout << "va tri 1 = (" << va[0] << ","<< va[1] << ","<< va[2] << ")" << endl;
#endif

#if 0 // Don't think we need this here. It didn't fix the crashing
    pvbo.bind();
    this->shaderProgram->enableAttributeArray("position");
    this->shaderProgram->setAttributeBuffer("position", GL_FLOAT, 0, 3);

    cvbo.bind();
    this->shaderProgram->enableAttributeArray("color");
    this->shaderProgram->setAttributeBuffer("color", GL_FLOAT, 0, 3);
#endif

    cout << "after bind, shaderProgram: " << (unsigned long long int)this->shaderProgram << endl;
    this->vao.bind(); // sets this vertex array object as the one to use.

    cout << "Num triangles is " << numtri << endl;
    glDrawArrays(GL_TRIANGLES, 0, this->numtri);

    this->vao.release();
    this->shaderProgram->release();
}

void SphereGeometry::vertex_push (const float& x, const float& y, const float& z, vector<float>& vp)
{
#if 0
    this->vertexPositions.push_back (x);
    this->vertexPositions.push_back (y);
    this->vertexPositions.push_back (z);
#else
    vp.push_back (x);
    vp.push_back (y);
    vp.push_back (z);
#endif
}

void SphereGeometry::color_push (const float& r, const float& g, const float& b)
{
    this->vertexColors.push_back (r);
    this->vertexColors.push_back (g);
    this->vertexColors.push_back (b);
}
