#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QOpenGLWidget>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //void addSpecial (QOpenGLWidget*);

    void myinit();

//private slots:
//    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QWidget *mainparent;
};
#endif // MAINWINDOW_H
