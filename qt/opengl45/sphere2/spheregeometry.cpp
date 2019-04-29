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

void SphereGeometry::computeSphere (vector<float> positionOffset, GLushort& idx)
{
    int rings = 6;    // each cap is a "ring"
    int segments = 8; // number of segments in a ring
    float r = 0.04f;     // sphere radius
    // The indices index

    // First cap, draw as a triangle fan, but record indices so that
    // we only need a single call to glDrawElements.
    float rings0 = M_PI * -0.5;
    float _z0  = sin(rings0);
    float z0  = r * _z0 + positionOffset[2];
    float r0 =  cos(rings0);
    float rings1 = M_PI * (-0.5 + 1.0f / rings);
    float _z1 = sin(rings1);
    float z1 = r * _z1 + positionOffset[2];
    float r1 = cos(rings1);
    // Push the central point
    this->vertex_push (0.0f + positionOffset[0], 0.0f + positionOffset[1], z0 + positionOffset[2], this->vertexPositions);
    this->vertex_push (0.0f, 0.0f, -1.0f, this->vertexNormals);
    this->vertex_push (0.0f, 0.0f, 0.0f, this->vertexColors);

    GLushort capMiddle = idx++;
    GLushort ringStartIdx = idx;
    GLushort lastRingStartIdx = idx;

    bool firstseg = true;
    for (int j = 0; j < segments; j++) {
        float segment = 2 * M_PI * (float) (j) / segments;
        float x = cos(segment);
        float y = sin(segment);

        float _x1 = x*r1;
        float x1 = _x1*r + positionOffset[0];
        float _y1 = y*r1;
        float y1 = _y1*r + positionOffset[1];

        this->vertex_push (x1, y1, z1, this->vertexPositions);
        this->vertex_push (_x1, _y1, _z1, this->vertexNormals);
        if (j%2) {
            this->vertex_push (1.0f, 0.2f, 0.0f, this->vertexColors);
        } else {
            this->vertex_push (0.0f, 0.2f, 1.0f, this->vertexColors);
        }

        if (!firstseg) {
            this->indices.push_back (capMiddle);
            this->indices.push_back (idx-1);
            this->indices.push_back (idx++);
        } else {
            idx++;
            firstseg = false;
        }
    }
    this->indices.push_back (capMiddle);
    this->indices.push_back (idx-1);
    this->indices.push_back (capMiddle+1);

    // Now add the triangles around the rings
    for (int i = 2; i < rings; i++) {

        rings0 = M_PI * (-0.5 + (float) (i) / rings);
        _z0  = sin(rings0);
        z0  = r * _z0 + positionOffset[2];
        r0 =  cos(rings0);

        for (int j = 0; j < segments; j++) {

            // "current" segment
            float segment = 2 * M_PI * (float)j / segments;
            float x = cos(segment);
            float y = sin(segment);

            // One vertex per segment
            float _x0 = x*r0;
            float x0 = _x0*r + positionOffset[0];
            float _y0 = y*r0;
            float y0 = _y0*r + positionOffset[1];

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

            if (j == segments - 1) {
                // Last vertex is back to the start
                this->indices.push_back (ringStartIdx++);
                this->indices.push_back (idx);
                this->indices.push_back (lastRingStartIdx);
                this->indices.push_back (lastRingStartIdx);
                this->indices.push_back (idx++);
                this->indices.push_back (lastRingStartIdx+segments);
            } else {
                this->indices.push_back (ringStartIdx++);
                this->indices.push_back (idx);
                this->indices.push_back (ringStartIdx);
                this->indices.push_back (ringStartIdx);
                this->indices.push_back (idx++);
                this->indices.push_back (idx);
            }
        }
        lastRingStartIdx += segments;
    }

    // bottom cap
    rings0 = M_PI * 0.5;
    _z0  = sin(rings0);
    z0  = r * _z0;
    r0 =  cos(rings0);
    // Push the central point of the bottom cap
    this->vertex_push (0.0f + positionOffset[0], 0.0f + positionOffset[1], z0 + positionOffset[2], this->vertexPositions);
    this->vertex_push (0.0f, 0.0f, 1.0f, this->vertexNormals);
    this->vertex_push (1.0f, 1.0f, 1.0f, this->vertexColors);
    capMiddle = idx++;
    firstseg = true;
    // No more vertices to push, just do the indices for the bottom cap
    ringStartIdx = lastRingStartIdx;
    for (int j = 0; j < segments; j++) {
        if (j != segments - 1) {
            this->indices.push_back (capMiddle);
            this->indices.push_back (ringStartIdx++);
            this->indices.push_back (ringStartIdx);
        } else {
            // Last segment
            this->indices.push_back (capMiddle);
            this->indices.push_back (ringStartIdx);
            this->indices.push_back (lastRingStartIdx);
        }
    }

    // end of sphere calculation
    cout << "Number of vertexPositions coords: " << (this->vertexPositions.size()/3) << endl;
}

void SphereGeometry::initialize()
{
    vector<float> po = {{ -2.5f, -2.5f, 0.0f }};
    GLushort idx = 0;

    unsigned int sidelen = 38;
    for (unsigned int a = 0; a < sidelen; a++) {
        po[0] = -2.5f;
        for (unsigned int b = 0; b < sidelen; b++) {
            this->computeSphere (po, idx);
            po[0] += 0.1f;
        }
        po[1] += 0.1f;
    }
    cout << "After compute sphere " << (sidelen*sidelen) << " times, we have "
         << (this->vertexPositions.size()/3) << " vertex coordinates" << endl;
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
    //glPolygonMode(GL_FRONT, GL_LINE);

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
