#include "openglwindow.h"

#include <QtCore/QCoreApplication>

#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>

#include <iostream>

using std::cout;
using std::endl;

//! [1]
OpenGLWindow::OpenGLWindow(QWindow *parent)
    : QWindow(parent)
    , m_update_pending(false)
    , m_animating(false)
    , m_context(0)
    , m_paint_device(0)
{
    setSurfaceType(QWindow::OpenGLSurface);
}

//! [1]
OpenGLWindow::~OpenGLWindow()
{
    delete m_paint_device;
}

//! [2]
void OpenGLWindow::render(QPainter *p)
{
    cout << "OpenGLWindow::render(QPainter *p) called (unimplemented)" << endl;
    Q_UNUSED(p);
}

void OpenGLWindow::initialize()
{
}

void OpenGLWindow::render()
{
    cout << "OpenGLWindow::render() called" << endl;
    if (!m_paint_device) {
        m_paint_device = new QOpenGLPaintDevice;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    m_paint_device->setSize(size());

    QPainter _painter (m_paint_device);
    render (&_painter);
}
//! [2]

//! [3]
void OpenGLWindow::renderLater()
{
    cout << "OpenGLWindow::renderLater() called" << endl;
    if (!m_update_pending) {
        m_update_pending = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

bool OpenGLWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::UpdateRequest:
    {
        m_update_pending = false;
        renderNow();
        return true;
    }
    default:
    {
        return QWindow::event(event);
    }
    }
}

void OpenGLWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed()) {
        renderNow();
    }
}
//! [3]

//! [4]
void OpenGLWindow::renderNow()
{
    //cout << "OpenGLWindow::renderNow() called" << endl;
    if (!isExposed()) {
        return;
    }

    bool needsInitialize = false;

    if (!m_context) {
        cout << "Creating new QOpenGLContext" << endl;
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());
        m_context->create();
        needsInitialize = true;
    }
    m_context->makeCurrent(this);

    if (needsInitialize) {
        cout << "OpenGLWindow::renderNow() : NeedsInitialize" << endl;
        initializeOpenGLFunctions();
        initialize();
    }

    render();
    m_context->swapBuffers(this);
    if (m_animating) {
        renderLater();
    }
}
//! [4]

void OpenGLWindow::setAnimating(bool animating)
{
    m_animating = animating;

    if (animating) {
        renderLater();
    }
}
