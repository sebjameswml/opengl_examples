#include <QtCore/qmath.h>
#include "spheregeometry.h"
#include <iostream>

using std::cout;
using std::endl;
using std::vector;
SphereGeometry::SphereGeometry(QOpenGLShaderProgram *program)
    : ivbo(QOpenGLBuffer::IndexBuffer)
    , pvbo(QOpenGLBuffer::VertexBuffer)
    , nvbo(QOpenGLBuffer::VertexBuffer)
    , cvbo(QOpenGLBuffer::VertexBuffer)
{
    // This call is critical when using a separate "geometry" class like this
    initializeOpenGLFunctions();
    this->shaderProgram = program;
    this->initialize();
}

SphereGeometry::~SphereGeometry()
{
    if (this->ivbo.isCreated()) {
        this->ivbo.destroy();
    }
    if (this->pvbo.isCreated()) {
        this->pvbo.destroy();
    }
    if (this->nvbo.isCreated()) {
        this->nvbo.destroy();
    }
    if (this->cvbo.isCreated()) {
        this->cvbo.destroy();
    }
    this->vao.destroy();
}

void SphereGeometry::initialize()
{
    // Sphere calculation - calculate location of triangle vertices for the sphere.
    int rings = 5;
    int segments = 6;
    float r = 1.0f;

    this->vertexPositions.clear();

    // The indices index
    GLushort idx = 0;

    // First cap, draw as a triangle fan, but record indices so that
    // we only need a single call to glDrawElements.
    float rings0 = M_PI * -0.5;
    float _z0  = sin(rings0);
    float z0  = r * _z0;
    float r0 =  cos(rings0);
    float rings1 = M_PI * (-0.5 + 1.0f / rings);
    float _z1 = sin(rings1);
    float z1 = r * _z1;
    float r1 = cos(rings1);
    // Push the central point
    this->vertex_push (0.0f, 0.0f, z0, this->vertexPositions);
    this->vertex_push (0.0f, 0.0f, -1.0f, this->vertexNormals);
    this->vertex_push (0.5f, 0.1f, 0.0f, this->vertexColors);
    GLushort capMiddle = idx++;

    bool firstseg = true;
    for (int j = 0; j < segments; j++) {
        float segment = 2 * M_PI * (float) (j) / segments;
        float x = cos(segment);
        float y = sin(segment);

        rings1 = M_PI * (-0.5 + 1.0f / rings);
        _z1 = sin(rings1);
        z1 = r * _z1;
        r1 = cos(rings1);

        float _x1 = x*r1;
        float x1 = _x1*r;
        float _y1 = y*r1;
        float y1 = _y1*r;

        this->vertex_push (x1, y1, z1, this->vertexPositions);
        this->vertex_push (_x1, _y1, _z1, this->vertexNormals);
        this->vertex_push (0.0f, 0.4f, 0.5f, this->vertexColors);

        if (!firstseg) {
            this->indices.push_back (capMiddle);
            this->indices.push_back (idx-1);
            this->indices.push_back (idx++);
        } else {
            idx++;
            firstseg = false;
        }
    }
    cout << "Last tri is " << capMiddle << "," << idx-1 << "," << idx << "."<<endl;
    this->indices.push_back (capMiddle);
    this->indices.push_back (idx-1);
    this->indices.push_back (capMiddle+1);

    cout << "vertexPositions in cap: " << (vertexPositions.size()/3) << endl;
    cout << "current idx: " << idx << endl;
    cout << "indices size: " << indices.size() << endl;

#if 1
    // Now the triangles around the the rings
    for (int i = 1; i < 2;/*(rings-1);*/ i++) {

        rings0 = M_PI * (-0.5 + (float) (i) / rings);
        _z0  = sin(rings0);
        z0  = r * _z0;
        r0 =  cos(rings0);

        rings1 = M_PI * (-0.5 + (float) (i+1) / rings);
        _z1 = sin(rings1);
        z1 = r * _z1;
        r1 = cos(rings1);

        GLushort ringstartIdx = idx;
        for (int j = 0; j < segments; j++) {

            // "current" segment
            float segment = 2 * M_PI * (float)j / segments;
            float x = cos(segment);
            float y = sin(segment);

            // Two vertices per segment
            float _x0 = x*r0;
            float x0 = _x0*r;
            float _y0 = y*r0;
            float y0 = _y0*r;

            float _x1 = x*r1;
            float x1 = _x1*r;
            float _y1 = y*r1;
            float y1 = _y1*r;

            // NB: Only add ONE vertex per segment. ALREADY have the first ring!
            this->vertex_push (x0, y0, z0, this->vertexPositions);
            // The vertex normal of a vertex that makes up a sphere is
            // just a normal vector in the direction of the vertex.
            this->vertex_push (_x0, _y0, _z0, this->vertexNormals);
            if (j%2) {
                this->vertex_push (1.0f, 0.2f, 0.0f, this->vertexColors);
            } else {
                this->vertex_push (0.0f, 0.2f, 1.0f, this->vertexColors);
            }

            this->vertex_push (x1, y1, z1, this->vertexPositions);
            this->vertex_push (_x1, _y1, _z1, this->vertexNormals);
            if (j%2) {
                this->vertex_push (0.0f, 0.7f, 0.5f, this->vertexColors);
            } else {
                this->vertex_push (0.5f, 0.7f, 0.0f, this->vertexColors);
            }

            this->indices.push_back (idx++);
            this->indices.push_back (idx++);
            this->indices.push_back (idx);
            this->indices.push_back (idx-1);
            if (j == segments - 1) {
                // Last vertex is back to the start
                //this->indices.push_back (ringstartIdx+1);
                //this->indices.push_back (ringstartIdx);
            } else {
                this->indices.push_back (idx);
                this->indices.push_back (idx+1);
            }
            cout << "max index: " << idx+1 << endl;
        }
    }
    // end of sphere calculation
    cout << "vertexPositions coords: " << (this->vertexPositions.size()/3) << endl;
#endif
    this->vao.create();
    this->vao.bind(); // sets the Vertex Array Object current to the OpenGL context so we can write attributes to it

    // Index buffer - slightly different from process to setupVBO
    // (because no setAttributeBuffer call)
    if (this->ivbo.create() == false) {
        cout << "ivbo create failed" << endl;
    }
    this->ivbo.setUsagePattern (QOpenGLBuffer::StaticDraw);
    if (this->ivbo.bind() == false) {
        cout << "ivbo bind failed" << endl;
    }
    int sz = this->indices.size() * sizeof(GLushort);
    this->ivbo.allocate (indices.data(), sz);
    this->shaderProgram->enableAttributeArray("ebo");
    //this->shaderProgram->setAttributeBuffer("ebo", GL_USHORT, 0, 1); //??

    // Binds data from the "C++ world" to the OpenGL shader world for
    // "position", "normalin" and "color"
    this->setupVBO (this->pvbo, this->vertexPositions, "position");
    this->setupVBO (this->nvbo, this->vertexNormals, "normalin");
    this->setupVBO (this->cvbo, this->vertexColors, "color");

    // Release (unbind) all
    // this->ivbo.release(); // but causes problem to release this..
    this->pvbo.release();
    this->nvbo.release();
    this->cvbo.release();
    this->vao.release();
}

void SphereGeometry::setupVBO (QOpenGLBuffer& buf, vector<float>& dat, const char* arrayname)
{
    if (buf.create() == false) {
        cout << "VBO create failed" << endl;
    }
    buf.setUsagePattern (QOpenGLBuffer::StaticDraw);
    if (buf.bind() == false) {
        cout << "VBO bind failed" << endl;
    }
    buf.allocate (dat.data(), dat.size() * sizeof(float));
    // Because array attributes are disabled by default in OpenGL 4:
    this->shaderProgram->enableAttributeArray (arrayname);
    this->shaderProgram->setAttributeBuffer (arrayname, GL_FLOAT, 0, 3);
}

void SphereGeometry::render()
{
    // Render using our shader
    this->vao.bind();

    // Note: Wireframe mode:
    glPolygonMode(GL_FRONT, GL_LINE);

    // glDrawElements(mode, count, type, GLvoid* indices)
    glDrawElements (GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_SHORT, 0);

    this->vao.release();
}

void SphereGeometry::vertex_push (const float& x, const float& y, const float& z, vector<float>& vp)
{
    vp.push_back (x);
    vp.push_back (y);
    vp.push_back (z);
}
