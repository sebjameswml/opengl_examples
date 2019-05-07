#include "scene.h"
#include <iostream>

using std::cout;
using std::endl;
using std::vector;

Scene::Scene (QOpenGLShaderProgram *program)
{
    // This call is critical:
    initializeOpenGLFunctions();
    this->shaderProgram = program;
    this->initialize();
}

Scene::~Scene()
{
    vector<SphereLayer*>::iterator sli = this->layers.begin();
    while (sli != this->layers.end()) {
        delete ((*sli++));
    }

}

void
Scene::initialize (void)
{
    // Some times, create a layer, with parameters (sidelength and z position)
    SphereLayer* l1 = new SphereLayer(this->shaderProgram, 50, 0);
    this->layers.push_back (l1);
    // Then:
    SphereLayer* l2 = new SphereLayer(this->shaderProgram, 50, 0.3);
    this->layers.push_back (l2);
}

void
Scene::render (void)
{
    // for each in this->layers, call render?
    vector<SphereLayer*>::iterator sli = this->layers.begin();
    while (sli != this->layers.end()) {
        (*sli)->render();
        ++sli;
    }
}
