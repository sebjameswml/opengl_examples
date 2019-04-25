#ifndef _SPHEREGEOMETRY_H_
#define _SPHEREGEOMETRY_H_

#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions_3_1>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>

#include <vector>

// Contains the CPU based computations of the sphere's geometry as
// triangle vertices.
class SphereGeometry : protected QOpenGLFunctions_3_1
{
public:
    SphereGeometry (QOpenGLShaderProgram *program);
    ~SphereGeometry();

    void initialize();
    void render();

private:
    // Compute positions and colours of vertices for the sphere and
    // store in these:
    std::vector<float> vertexPositions;
    std::vector<float> vertexColors;

    unsigned int numtri;

    QOpenGLShaderProgram* shaderProgram;

    GLuint vertexLocation;
    GLuint colorLocation;
    GLuint matrixLocation;

    QOpenGLVertexArrayObject vao; // Our Vertex Array Object
    QOpenGLBuffer pvbo;           // Our 'positions' Vertex Buffer Object
    QOpenGLBuffer cvbo;           // Our 'colors' Vertex Buffer Object

    void vertex_push (const float& x, const float& y, const float& z, std::vector<float>& vp);
    void color_push (const float& r, const float& g, const float& b);
};

#endif // _SPHEREGEOMETRY_H_
