#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QScreen>
#include <QtCore/qmath.h>
#include "main.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize( 4 );
    format.setSamples(24);
    format.setVersion(3, 1);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    //format.setProfile( QSurfaceFormat::CoreProfile );

    ModernTriangle window;
    window.setFormat(format);
    window.resize(640, 480);
    window.show();

    return app.exec();
}

void ModernTriangle::initialize()
{
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_program = new QOpenGLShaderProgram();
    m_program->addShaderFromSourceCode (QOpenGLShader::Vertex,
                                        "#version 450\n"        // GLSL version 1.4
                                        "in vec3 position;\n"   // attribute named position with 3 elements per vertex in
                                        "in vec3 color;\n"      // A colour attribute
                                        "out vec4 fragColor;\n" // vec4: a vector of 4 floats
                                        "void main() {\n"
                                        " fragColor = vec4(color, 1.0);\n"
                                        " gl_Position = vec4(position, 1.0);\n"
                                        "}\n");
    m_program->addShaderFromSourceCode (QOpenGLShader::Fragment,
                                        "#version 450\n" //GLSL version 1.4
                                        "in vec4 fragColor;\n"
                                        "out vec4 finalcolor;\n"
                                        "void main() {\n"
                                        " finalcolor = fragColor;\n"
                                        "}\n");

    m_program->link();
    m_program->bind(); // bind Shader (Do not release until VAO is created)

    // location of vertex data arrays must be before they are
    // referenced but location not important otherwise
    static const float vertexPositions[] = {
        -1.0f, 0.0f, 0.0f, //(x,y,z) bottom left
        1.0f, 0.0f, 0.0f, //bottom right
        0.0f, 1.0f, 0.0f //top middle
    };

    static const float vertexColors[] = {
        1.0f, .0f, .0f, //red (r,g,b) values for each vertex
        .0f, 1.0f, .0f, //green
        .0f, .0f, 1.0f //blue
    };

    m_vao.create();
    m_vao.bind(); // sets the Vertex Array Object current to the OpenGL context so we can write attributes to it

    QOpenGLBuffer m_vvbo(QOpenGLBuffer::VertexBuffer);
    m_vvbo.create();
    m_vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vvbo.bind();
    m_vvbo.allocate(vertexPositions, 9 * sizeof(float));
    // this labels an attribute "position" that points to the memory
    // slot from the last buffer allocate() the position attribute is
    // an input to our vertex shader
    m_program->enableAttributeArray("position");
    m_program->setAttributeBuffer("position", GL_FLOAT, 0, 3);

    QOpenGLBuffer m_vcbo(QOpenGLBuffer::VertexBuffer);
    m_vcbo.create();
    m_vcbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vcbo.bind();
    m_vcbo.allocate(vertexColors, 9 * sizeof(float));
    // this labels an attribute "color" that points to the memory slot
    // from the last buffer allocate() the color attribute is an input
    // to our vertex shader
    m_program->enableAttributeArray("color");
    m_program->setAttributeBuffer("color", GL_FLOAT, 0, 3);

    // Release (unbind) all
    m_vvbo.release();
    m_vcbo.release();
    m_vao.release();
    m_program->release();
}

void ModernTriangle::render()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    // Clear
    glClear(GL_COLOR_BUFFER_BIT);

    // Render using our shader
    m_program->bind();
    m_vao.bind(); //sets
    glDrawArrays(GL_TRIANGLES, 0, 3);
    m_vao.release();
    m_program->release();
}
