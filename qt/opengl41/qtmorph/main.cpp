/*
 * Qt morphologica example
 */

#include <QtGui/QGuiApplication>

#define USING_QT 1
#include <morph/Visual.h>

#include <morph/GraphVisual.h>
#include <morph/vvec.h>

int main (int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    morph::Visual v (1024, 768, "Qt test");
    //v.addLabel ("morphologica in a Qt window!", {0,0,0});

    // Visualize a graph. Minimal example showing how a default graph appears

    // Create a GraphVisual object (obtaining a unique_ptr to the object) with a spatial offset within the scene of 0,0,0
    auto gv = std::make_unique<morph::GraphVisual<double>> (morph::vec<float>({0,0,0}));
    // This mandatory line of boilerplate code sets the parent pointer in GraphVisual and binds some functions
    v.bindmodel (gv);
    // Data for the x axis. A vvec is like std::vector, but with built-in maths methods
    morph::vvec<double> x;
    // This works like numpy's linspace() (the 3 args are "start", "end" and "num"):
    x.linspace (-0.5, 0.8, 14);
    // Set a graph up of y = x^3
    gv->setdata (x, x.pow(3));
    // finalize() makes the GraphVisual compute the vertices of the OpenGL model
    gv->finalize();
    // Add the GraphVisual OpenGL model to the Visual scene, transferring ownership of the unique_ptr
    v.addVisualModel (gv);

    // app.exec() now handles the rendering of the graphics

    // during app.exec(), then OpenGLWIndow gets its render called, which calls
    // Visual::render() via a callback. That's the plan. Also need to figure out how to
    // run dynamic stuff with Qt. Possibly with a timer?
    return app.exec();
}
