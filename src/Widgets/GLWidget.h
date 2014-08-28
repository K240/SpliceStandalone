#ifndef __GLWIDGET_H__
#define __GLWIDGET_H__

// #include <GL/glew.h>

#include <QtCore/QtCore>
#include <QtGui/QMouseEvent>
#include <QtGui/QMainWindow>
#include <QtGui/QImage>
#include <QtOpenGL/QGLWidget>
#include <QtCore/QElapsedTimer>

#include <FabricSplice.h>
#include <FabricCore.h>

namespace FabricSplice
{
  class GLWidget : public QGLWidget
  {
  	Q_OBJECT

  public:

  	GLWidget(QGLFormat format, QWidget *parent = NULL);
  	virtual ~GLWidget() {}

  	FabricCore::RTVal getInlineViewport();

    void resetRTVals();

    void setTime(float time);
    void setWireFrame(bool wireFrame);
    void frameAll();
    void frameSelection();
    void toggleGrid();
    void resetCameraPosition();

    /// returns the real frames per second of this TimeSlider
    double fps() const { return m_fps; }

  signals:

    void redrawn();

  protected:

    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

    void getArgsForMouseEvent(QMouseEvent * event, std::vector<FabricCore::RTVal> & args);
  	void mousePressEvent(QMouseEvent *event);
  	void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
  	void wheelEvent(QWheelEvent *event);
  	void keyPressEvent(QKeyEvent *event);

    FabricCore::RTVal m_camera;
    FabricCore::RTVal m_viewport;
    FabricCore::RTVal m_drawContext;

    bool m_requiresInitialize;

    QElapsedTimer m_fpsTimer;
    double m_fps;
    double m_fpsStack[16];

  };
};

#endif // __GLWIDGET_H__
