#include <QtCore/qmath.h>
#include "cubegeometry.h"
#include <iostream>

using std::cout;
using std::endl;
using std::vector;
CubeGeometry::CubeGeometry(QOpenGLShaderProgram *program)
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

CubeGeometry::~CubeGeometry()
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

void CubeGeometry::initialize()
{
    // Cube calculation - calculate location of triangle vertices for the shape.
    static const float cube_positions[] =
    {//  x   y   z  _
        -0.30f, -0.30f, -0.30f, 1.0f,
        -0.30f, -0.30f,  0.30f, 1.0f,
        -0.30f,  0.30f, -0.30f, 1.0f,
        -0.30f,  0.30f,  0.30f, 1.0f,
        0.30f, -0.30f, -0.30f, 1.0f,
        0.30f, -0.30f,  0.30f, 1.0f,
        0.30f,  0.30f, -0.30f, 1.0f,
        0.30f,  0.30f,  0.30f, 1.0f
    };

    float mag = sqrt (3*(0.3*0.3));
    float normcmp = 0.3/mag;
    static const float cube_normals[] =
    {//  x   y   z  _
        -normcmp, -normcmp, -normcmp, 1.0f,
        -normcmp, -normcmp,  normcmp, 1.0f,
        -normcmp,  normcmp, -normcmp, 1.0f,
        -normcmp,  normcmp,  normcmp, 1.0f,
        normcmp, -normcmp, -normcmp, 1.0f,
        normcmp, -normcmp,  normcmp, 1.0f,
        normcmp,  normcmp, -normcmp, 1.0f,
        normcmp,  normcmp,  normcmp, 1.0f
    };

    static const float cube_colors[] =
    {//  x   y   z  _
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f
    };

    static const GLushort cube_indices[] =
    {
        0, 1, 2, 3, 6, 7, 4, 5, // First strip
        0xffff,                 // restart index
        2, 6, 0, 4, 1, 5, 3, 7  // Second strip
    };

    this->vao.create();
    this->vao.bind(); // sets the Vertex Array Object current to the OpenGL context so we can write attributes to it

    if (this->ivbo.create() == false) {
        cout << "ivbo create failed" << endl;
    }
    this->ivbo.setUsagePattern (QOpenGLBuffer::StaticDraw);
    if (this->ivbo.bind() == false) {
        cout << "ivbo bind failed" << endl;
    }
    this->ivbo.allocate (cube_indices, sizeof(cube_indices));

    // this labels an attribute "position" that points to the memory
    // slot from the last buffer allocate() the position attribute is
    // an input to our vertex shader
    this->shaderProgram->enableAttributeArray("ebo");
    //this->shaderProgram->setAttributeBuffer("ebo", GL_USHORT, 0, 1); //??

    // Binds data from the "C++ world" to the OpenGL shader world for "position"
    if (this->pvbo.create() == false) {
        cout << "pvbo create failed" << endl;
    }
    this->pvbo.setUsagePattern (QOpenGLBuffer::StaticDraw);
    if (this->pvbo.bind() == false) {
        cout << "pvbo bind failed" << endl;
    }
    this->pvbo.allocate (cube_positions, sizeof(cube_positions));

    // this labels an attribute "position" that points to the memory
    // slot from the last buffer allocate() the position attribute is
    // an input to our vertex shader
    this->shaderProgram->enableAttributeArray("position");
    this->shaderProgram->setAttributeBuffer("position", GL_FLOAT, 0, 4);

#if 0
    // Binds data from the "C++ world" to the OpenGL shader world for "position"
    if (this->nvbo.create() == false) {
        cout << "nvbo create failed" << endl;
    }
    this->nvbo.setUsagePattern (QOpenGLBuffer::StaticDraw);

    if (this->nvbo.bind() == false) {
        cout << "nvbo bind failed" << endl;
    }
    this->nvbo.allocate (cube_normals, sizeof(cube_normals));

    // this labels an attribute "position" that points to the memory
    // slot from the last buffer allocate() the position attribute is
    // an input to our vertex shader
    this->shaderProgram->enableAttributeArray("normalin");
    this->shaderProgram->setAttributeBuffer("normalin", GL_FLOAT, 0, 4);
#endif

    this->cvbo.create();
    this->cvbo.setUsagePattern (QOpenGLBuffer::StaticDraw);
    this->cvbo.bind();
    this->cvbo.allocate (cube_colors, sizeof(cube_colors));

    // this labels an attribute "color" that points to the memory slot
    // from the last buffer allocate() the color attribute is an input
    // to our vertex shader
    this->shaderProgram->enableAttributeArray ("color");
    this->shaderProgram->setAttributeBuffer ("color", GL_FLOAT, 0, 4);

    // Release (unbind) all
    //this->ivbo.release(); // ??
    this->pvbo.release();
#if 0
    this->nvbo.release();
#endif
    this->cvbo.release();
    this->vao.release();
}

void CubeGeometry::render()
{
    // Render using our shader, which should have been bound in
    // ShapeWindow::render()
    this->vao.bind(); // sets this vertex array object as the one to use.
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(0xffff);
    glDrawElements (GL_TRIANGLE_STRIP, 17, GL_UNSIGNED_SHORT, NULL);
    this->vao.release();
}

void CubeGeometry::vertex_push (const float& x, const float& y, const float& z, vector<float>& vp)
{
    vp.push_back (x);
    vp.push_back (y);
    vp.push_back (z);
}
