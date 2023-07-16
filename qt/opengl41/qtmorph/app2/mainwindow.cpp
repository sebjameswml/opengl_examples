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
    this->viswidget_init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// This gets called by viswidget via a callback, with a guarantee that the GL environment has been set up
void MainWindow::buildmodels (morph::Visual* _v)
{
    auto gv = std::make_unique<morph::GraphVisual<double>> (morph::vec<float>({0,0,0}));
    // This mandatory line of boilerplate code sets the parent pointer in GraphVisual and binds some functions
    _v->bindmodel (gv);
    // Allow 3D
    gv->twodimensional = false;
    // Data for the x axis. A vvec is like std::vector, but with built-in maths methods
    morph::vvec<double> x;
    // This works like numpy's linspace() (the 3 args are "start", "end" and "num"):
    x.linspace (-0.5, 0.8, 14);
    // Set a graph up of y = x^3
    gv->setdata (x, x.pow(3));
    // finalize() makes the GraphVisual compute the vertices of the OpenGL model
    gv->finalize();
    // Add the GraphVisual OpenGL model to the Visual scene, transferring ownership of the unique_ptr
    _v->addVisualModel (gv);
}

void MainWindow::viswidget_init()
{
    // Create widget. Seems to open in its own window with a new context.
    //morph::qt::viswidget* vw = new morph::qt::viswidget;
    morph::qt::viswidget* vw = new morph::qt::viswidget (this->parentWidget());

    // Set callback on vm
    vw->buildmodels = &MainWindow::buildmodels;

    // Add the OpenGL widget to the UI.
    this->ui->verticalLayout->addWidget (vw);
}

void MainWindow::on_pushButton_clicked()
{
    std::cout << "click\n";
}
