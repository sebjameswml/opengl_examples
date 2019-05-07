#ifndef _SCENE_H_
#define _SCENE_H_

#include <QtGui/QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_5_Core>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>

#include <vector>
using std::vector;

#include "spherelayer.h"

// Contains a scene made up of several spherelayers.
class Scene : protected QOpenGLFunctions_4_5_Core
{
public:
    Scene (QOpenGLShaderProgram *program);
    ~Scene();

    void initialize (void);
    void render (void);

    //! The sphere layers.
    vector<SphereLayer*> layers;

    //! Sphere attributes common to each spherelayer.
    int rings = 6;
    VBOint segments = 8; // number of segments in a ring
    float r = 0.04f;  // sphere radius

private:
    //! The scene holds the shaderProgram
    QOpenGLShaderProgram* shaderProgram;
};

#endif // _SCENE_H_
