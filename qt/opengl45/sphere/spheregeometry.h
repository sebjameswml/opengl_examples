#ifndef _SPHEREGEOMETRY_H_
#define _SPHEREGEOMETRY_H_

#include <QtGui/QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_5_Core>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>

#include <vector>

// Contains the CPU based computations of the sphere's geometry as
// triangle vertices.
class SphereGeometry : protected QOpenGLFunctions_4_5_Core
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
    std::vector<float> vertexNormals;
    std::vector<float> vertexColors;

    unsigned int numtri;

    QOpenGLShaderProgram* shaderProgram;

    QOpenGLVertexArrayObject vao; // Our Vertex Array Object
    QOpenGLBuffer pvbo;           // Our 'positions' Vertex Buffer Object
    QOpenGLBuffer nvbo;           // Our 'normals' Vertex Buffer Object
    QOpenGLBuffer cvbo;           // Our 'colors' Vertex Buffer Object

    void vertex_push (const float& x, const float& y, const float& z, std::vector<float>& vp);
};

#endif // _SPHEREGEOMETRY_H_
