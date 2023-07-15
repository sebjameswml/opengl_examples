#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <morph/qt/viswidget.h>
#include <morph/GraphVisual.h>
#include <morph/vvec.h>

#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mainparent = parent;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::myinit()
{
    // Create widget.
    morph::qt::viswidget* vw = new morph::qt::viswidget;
    // Calling show ensures initializeGL() method gets called
    vw->show();

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
    gv->finalize();
    // Add the GraphVisual OpenGL model to the Visual scene, transferring ownership of the unique_ptr
    vw->v.addVisualModel (gv);

    // ?
    //this->ui->verticalLayout->addWidget(vw);
}

//void MainWindow::on_pushButton_clicked()
//{
//}
