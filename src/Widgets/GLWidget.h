#ifndef __GLWIDGET_H__
#define __GLWIDGET_H__

// #include <GL/glew.h>

#include <QtCore/QtCore>
#include <QtGui/QMouseEvent>
#include <QtGui/QMainWindow>
#include <QtGui/QImage>
#include <QtOpenGL/QGLWidget>
#include <QtCore/QTime>

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

    void enableRedraw(bool enable = true) { m_redrawEnabled = enable; }
    bool isRedrawEnabled() { return m_redrawEnabled; }
    void setTime(float time);
    void setWireFrame(bool wireFrame);
    void toggleGrid();
    void resetCameraPosition();
    void toggleFullScreen();

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
    bool m_requiresResize;

    QTime m_fpsTimer;
    double m_fps;
    double m_fpsStack[16];
    bool m_redrawEnabled;
    bool m_painting;

    QWidget * m_prevParent;

  };
};

#endif // __GLWIDGET_H__
