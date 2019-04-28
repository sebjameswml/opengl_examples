#ifndef _SPHEREGEOMETRY_H_
#define _SPHEREGEOMETRY_H_

#include <QtGui/QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_5_Core>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>

#include <vector>
using std::vector;

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
    QOpenGLBuffer ivbo;           // Indices Vertex Buffer Object
    QOpenGLBuffer pvbo;           // positions Vertex Buffer Object
    QOpenGLBuffer nvbo;           // normals Vertex Buffer Object
    QOpenGLBuffer cvbo;           // colors Vertex Buffer Object

    vector<GLushort> indices;
    vector<float> vertexPositions;
    vector<float> vertexNormals;
    vector<float> vertexColors;

    QOpenGLShaderProgram* shaderProgram;

    QOpenGLVertexArrayObject vao; // Our Vertex Array Object

    // Sphere calculation - calculate location of triangle vertices
    // for the sphere. The sphere will be made up of two "caps" of
    // triangles, and a series of rings.
    void computeSphere (void);

    void vertex_push (const float& x, const float& y, const float& z, vector<float>& vp);

    void setupVBO (QOpenGLBuffer& buf, vector<float>& dat, const char* arrayname);
};

#endif // _SPHEREGEOMETRY_H_
