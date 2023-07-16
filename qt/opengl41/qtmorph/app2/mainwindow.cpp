#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <morph/qt/viswidget.h>
#include <morph/GraphVisual.h>
#include <morph/TriangleVisual.h>
#include <morph/vec.h>
#include <morph/vvec.h>

#include <QVBoxLayout>
#include <QFrame>
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mainparent = parent;
    this->myinit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::myinit()
{
    // Create widget. Seems to open in its own window with a new context.
    //morph::qt::viswidget* vw = new morph::qt::viswidget;
    morph::qt::viswidget* vw = new morph::qt::viswidget (mainparent);
    // Add the OpenGL widget to the UI.
    this->ui->verticalLayout->addWidget(vw);

#if 1 // Simplest VisualModel is ok (no text)
    // ~Simplest possible GL model
    morph::vec<float, 3> c1 = { 0, 0, 0 };
    morph::vec<float, 3> c2 = { 0.25, 0, 0 };
    morph::vec<float, 3> c3 = { 0.0, 0.3, 0 };
    morph::vec<float, 3> colour1 = { 1.0, 0.4, 0.2 };

    auto tv = std::make_unique<morph::TriangleVisual> (morph::vec<float>({0,0,0}), c1, c2, c3, colour1);
    vw->v.bindmodel (tv);
    // finalize now builds the model, but leaves the call to VisualModel::postVertexInit to be called at render time.
    tv->finalize();
    vw->v.addVisualModel (tv);

#else // GraphVisual still nok.
    // We can now add VisualModels to the Visual inside the Widget. Create a GraphVisual
    // object (obtaining a unique_ptr to the object) with a spatial offset within the
    // scene of 0,0,0
    auto gv = std::make_unique<morph::GraphVisual<double>> (morph::vec<float>({0,0,0}));
    // This mandatory line of boilerplate code sets the parent pointer in GraphVisual and binds some functions
    vw->v.bindmodel (gv);
    // Allow 3D
    gv->twodimensional = false;
    // Data for the x axis. A vvec is like std::vector, but with built-in maths methods
    morph::vvec<double> x;
    // This works like numpy's linspace() (the 3 args are "start", "end" and "num"):
    x.linspace (-0.5, 0.8, 14);
    // Set a graph up of y = x^3
    gv->setdata (x, x.pow(3));
    // finalize() makes the GraphVisual compute the vertices of the OpenGL model
    std::cout << "gv->finalize()\n";
    gv->finalize();
    // Add the GraphVisual OpenGL model to the Visual scene, transferring ownership of the unique_ptr
    std::cout << "addVisualModel to vw\n";
    vw->v.addVisualModel (gv);
#endif
    this->ovw = (QOpenGLWidget*)vw; // Can probably access vw via this->ui->verticalLayout
}

void MainWindow::on_pushButton_clicked()
{
    std::cout << "click\n";
#if 0
    // Heh - good luck...
    morph::qt::viswidget* vw = (morph::qt::viswidget*) this->ovw;
    auto gv = std::make_unique<morph::GraphVisual<double>> (morph::vec<float>({0,2,0}));
    vw->v.bindmodel (gv);
    gv->twodimensional = false;
    morph::vvec<double> x;
    x.linspace (-0.5, 0.8, 14);
    gv->setdata (x, x.pow(4));
    gv->finalize();
    vw->v.addVisualModel (gv);
#endif
}
