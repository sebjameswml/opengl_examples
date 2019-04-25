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
    this->initialize();
}

SphereGeometry::~SphereGeometry()
{
    if (this->pvbo.isCreated()) {
        this->pvbo.destroy();
    }
    if (this->cvbo.isCreated()) {
        this->cvbo.destroy();
    }
    this->vao.destroy();
}

void SphereGeometry::initialize()
{
    // Sphere calculation - calculate location of triangle vertices for the sphere.
    int rings = 12;
    int segments = 12;
    float r = 1.0f;
    for (int i = 0; i <= rings; i++) {

        double rings0 = M_PI * (-0.5 + (double) (i - 1) / rings);
        double z0  = sin(rings0);
        double r0 =  cos(rings0);

        double rings1 = M_PI * (-0.5 + (double) i / rings);
        double z1 = sin(rings1);
        double r1 = cos(rings1);

        for (int j = 0; j < segments; j++) {

            // "current" segment
            double segment = 2 * M_PI * (double) (j - 1) / segments;
            double x = cos(segment);
            double y = sin(segment);

            // "next" segment
            double segn = 2 * M_PI * (double) (j) / segments;
            double xn = cos(segn);
            double yn = sin(segn);

            // Two triangles per segment
            double x0 = x*r0*r;
            double y0 = y*r0*r;

            double x1 = x*r1*r;
            double y1 = y*r1*r;

            double xn0 = xn*r0*r;
            double yn0 = yn*r0*r;

            double xn1 = xn*r1*r;
            double yn1 = yn*r1*r;

            this->vertex_push (x0,y0,z0);
            this->color_push (1.0f, 0.2f, 0.0f);

            this->vertex_push (x1,y1,z1);
            this->color_push (1.0f, 0.2f, 0.0f);

            this->vertex_push (xn0,yn0,z0);
            this->color_push (1.0f, 0.2f, 0.0f);

            this->vertex_push (xn0,yn0,z0);
            this->color_push (0.0f, .2f, 0.6f);

            this->vertex_push (x1,y1,z1);
            this->color_push (0.0f, .2f, 0.6f);

            this->vertex_push (xn1,yn1,z1);
            this->color_push (0.0f, .2f, 0.6f);
        }
    }
    // end of sphere calculation

    this->vao.create();
    this->vao.bind(); // sets the Vertex Array Object current to the OpenGL context so we can write attributes to it

    // Binds data from the "C++ world" to the OpenGL shader world for "position"
    if (this->pvbo.create() == false) {
        cout << "pvbo create failed" << endl;
    }
    this->pvbo.setUsagePattern (QOpenGLBuffer::StaticDraw);
    if (this->pvbo.bind() == false) {
        cout << "pvbo bind failed" << endl;
    }
    this->pvbo.allocate (vertexPositions.data(), vertexPositions.size() * sizeof(float));

    // this labels an attribute "position" that points to the memory
    // slot from the last buffer allocate() the position attribute is
    // an input to our vertex shader
    this->shaderProgram->enableAttributeArray("position");
    this->shaderProgram->setAttributeBuffer("position", GL_FLOAT, 0, 3);

    this->cvbo.create();
    this->cvbo.setUsagePattern (QOpenGLBuffer::StaticDraw);
    this->cvbo.bind();
    this->cvbo.allocate (vertexColors.data(), vertexColors.size() * sizeof(float));

    this->numtri = vertexPositions.size()/3;

    // this labels an attribute "color" that points to the memory slot
    // from the last buffer allocate() the color attribute is an input
    // to our vertex shader
    this->shaderProgram->enableAttributeArray ("color");
    this->shaderProgram->setAttributeBuffer ("color", GL_FLOAT, 0, 3);

    // Release (unbind) all
    this->pvbo.release();
    this->cvbo.release();
    this->vao.release();
}

void SphereGeometry::render()
{
    // Render using our shader
    this->shaderProgram->bind();
    this->vao.bind(); // sets this vertex array object as the one to use.
    glDrawArrays (GL_TRIANGLES, 0, this->numtri);
    this->vao.release();
    this->shaderProgram->release();
}

void SphereGeometry::vertex_push (const float& x, const float& y, const float& z, vector<float>& vp)
{
    vp.push_back (x);
    vp.push_back (y);
    vp.push_back (z);
}

void SphereGeometry::vertex_push (const float& x, const float& y, const float& z)
{
    this->vertexPositions.push_back (x);
    this->vertexPositions.push_back (y);
    this->vertexPositions.push_back (z);
}

void SphereGeometry::color_push (const float& r, const float& g, const float& b)
{
    this->vertexColors.push_back (r);
    this->vertexColors.push_back (g);
    this->vertexColors.push_back (b);
}
