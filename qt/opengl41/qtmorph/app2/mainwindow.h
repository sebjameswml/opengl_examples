#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QOpenGLWidget>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace morph { class Visual; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void viswidget_init();
    static void buildmodels (morph::Visual* _v);

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QWidget *mainparent;
    //QOpenGLWidget* ovw;
};
#endif // MAINWINDOW_H
