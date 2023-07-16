#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <morph/qt/viswidget.h>
#include <morph/GraphVisual.h>
#include <morph/TriangleVisual.h>
//#include <morph/HexGrid.h>
//#include <morph/HexGridVisual.h>
#include <morph/ScatterVisual.h>
#include <morph/Scale.h>
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

MainWindow::~MainWindow() { delete ui; }

// This gets called by viswidget via a callback, with a guarantee that the GL environment has been set up
void MainWindow::buildmodels (morph::Visual* _v)
{
#if 0
    morph::HexGrid hg(0.01f, 3.0f, 0.0f);
    hg.setCircularBoundary (0.6f);
    // Make some dummy data (a sine wave) to make an interesting surface
    morph::vvec<float> data(hg.num(), 0.0f);
    for (unsigned int ri=0; ri<hg.num(); ++ri) {
        data[ri] = 0.05f + 0.05f*std::sin(20.0f*hg.d_x[ri]) * std::sin(10.0f*hg.d_y[ri]) ; // Range 0->1
    }
    morph::vec<float, 3> offset = { 0.0f, -0.05f, 0.0f };
    auto hgv = std::make_unique<morph::HexGridVisual<float>>(&hg, offset);
    _v.bindmodel (hgv);
    hgv->setScalarData (&data);
    hgv->hexVisMode = morph::HexVisMode::HexInterp; // Or morph::HexVisMode::Triangles for a smoother surface plot
    hgv->finalize();
    _v.addVisualModel (hgv);

#else
# if 1
    morph::vec<float, 3> offset = { 0.0, 0.0, 0.0 };
    morph::Scale<float> scale;
    scale.setParams (1.0, 0.0);

    // Note use of morph::vvecs here, which can be passed into
    // VisualDataModel::setDataCoords(std::vector<vec<float>>* _coords)
    // and setScalarData(const std::vector<T>* _data)
    // This is possible because morph::vvec derives from std::vector.
    morph::vvec<morph::vec<float, 3>> points(20*20);
    morph::vvec<float> data(20*20);
    size_t k = 0;
    for (int i = -10; i < 10; ++i) {
        for (int j = -10; j < 10; ++j) {
            float x = 0.1*i;
            float y = 0.1*j;
            // z is some function of x, y
            float z = x * std::exp(-(x*x) - (y*y));
            points[k] = {x, y, z};
            data[k] = z;
            k++;
        }
    }

    auto sv = std::make_unique<morph::ScatterVisual<float>> (offset);
    _v->bindmodel (sv);
    sv->setDataCoords (&points);
    sv->setScalarData (&data);
    sv->radiusFixed = 0.03f;
    sv->colourScale = scale;
    sv->cm.setType (morph::ColourMapType::Plasma);
    sv->finalize();
    _v->addVisualModel (sv);

# else
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
# endif
#endif
}

void MainWindow::viswidget_init()
{
    // Create widget. Seems to open in its own window with a new context.
    morph::qt::viswidget* vw = new morph::qt::viswidget (this->parentWidget());
    // Set callback on vm
    vw->buildmodels = &MainWindow::buildmodels;
    // Choose lighting effects if you want
    vw->v.lightingEffects();
    // Add the OpenGL widget to the UI.
    this->ui->verticalLayout->addWidget (vw);
    // Keep a copy of vw
    this->p_vw = vw;
}

void MainWindow::on_pushButton_clicked()
{
    std::cout << "click\n";
#if 0 // This gets created, but with dodgy fonts.
    auto gv = std::make_unique<morph::GraphVisual<double>> (morph::vec<float>({1.5,0,0}));
    // Access to viswidget via ui:
    static_cast<morph::qt::viswidget*>(this->p_vw)->v.bindmodel (gv);
    gv->twodimensional = false;
    morph::vvec<double> x;
    x.linspace (-1.5, 1.5, 25);
    gv->setdata (x, x.pow(2));
    gv->finalize();
    static_cast<morph::qt::viswidget*>(this->p_vw)->v.addVisualModel (gv);
#endif
}
