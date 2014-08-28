#include "GLWidget.h"

#include <stdio.h>
#include <stdlib.h>
// #include <sys/time.h>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <ostream>
#include <fstream>
#include <streambuf>
#include <memory>

#include <QtGui/qapplication.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "macros.h"

using namespace FabricSplice;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//  GLWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////////

GLWidget::GLWidget(QGLFormat format, QWidget *parent) :
	QGLWidget(format, parent)
{	
  m_fps = 0.0;

  for(int i=0;i<16;i++)
    m_fpsStack[i] = 0.0;

  m_fpsTimer.start();
  resetRTVals();
	setFocusPolicy(Qt::StrongFocus);
}

void GLWidget::resetRTVals()
{
  FABRIC_TRY("GLWidget::resetRTVals",

    m_camera = constructObjectRTVal("SpliceStandaloneCamera");
    if(!m_camera.isValid())
    {
      printf("[SceneLibTreeWidget] Error: Cannot construct SpliceStandaloneCamera RTVal (extension loaded?)\n");
      return;
    }

    m_viewport = constructObjectRTVal("SpliceStandaloneViewport");
    if(!m_viewport.isValid())
    {
      printf("[SceneLibTreeWidget] Error: Cannot construct SpliceStandaloneViewport RTVal (extension loaded?)\n");
      return;
    }

    m_viewport.setMember("camera", m_camera);

    m_drawContext = constructObjectRTVal("DrawContext");
    if(!m_drawContext.isValid())
    {
      printf("[SceneLibTreeWidget] Error: Cannot construct DrawContext RTVal (extension loaded?)\n");
      return;
    }

  );

  m_requiresInitialize = true;
}

FabricCore::RTVal GLWidget::getInlineViewport()
{
  FABRIC_TRY_RETURN("GLWidget::getInlineViewport", FabricCore::RTVal(),

     FabricCore::RTVal drawContext = m_viewport.maybeGetMember("drawContext");
     return drawContext.maybeGetMember("viewport");

  );
}

void GLWidget::paintGL()
{
  // compute the fps
  double ms = m_fpsTimer.elapsed();
  if(ms == 0.0)
    m_fps = 0.0;
  else
    m_fps = 1000.0 / ms;

  double averageFps = 0.0;
  for(int i=0;i<15;i++) {
    m_fpsStack[i+1] = m_fpsStack[i];
    averageFps += m_fpsStack[i];
  }
  m_fpsStack[0] = m_fps;
  averageFps += m_fps;
  averageFps /= 16.0;
  m_fps = averageFps;

  m_fpsTimer.start();

	// perform drawing
  if(m_requiresInitialize)
  {
    initializeGL();
    resizeGL(width(), height());
  }

  FABRIC_TRY("GLWidget::paintGL",

    m_viewport.callMethod("", "paintGL", 0, 0);

  );

  emit redrawn();
}

void GLWidget::setTime(float time)
{
  FABRIC_TRY("GLWidget::setTime",

    FabricCore::RTVal timeVal = constructFloat32RTVal(time);
    m_viewport.callMethod("", "setTime", 1, &timeVal);

  );
  updateGL();
}


void GLWidget::setWireFrame(bool wireFrame)
{
  FABRIC_TRY("GLWidget::setWireFrame",

    FabricCore::RTVal wireFrameVal = constructBooleanRTVal(wireFrame);
    m_viewport.callMethod("", "setWireFrame", 1, &wireFrameVal);

  );
  updateGL();
}

void GLWidget::frameAll()
{
  FABRIC_TRY("GLWidget::frameAll",

    m_viewport.callMethod("", "frameAll", 0, 0);

  );
  updateGL();
}

void GLWidget::frameSelection()
{
  FABRIC_TRY("GLWidget::frameSelection",

    m_viewport.callMethod("", "frameSelection", 0, 0);

  );
  updateGL();
}

void GLWidget::toggleGrid()
{
  FABRIC_TRY("GLWidget::toggleGrid",

    m_viewport.callMethod("", "toggleGrid", 0, 0);

  );
  updateGL();
}

void GLWidget::resetCameraPosition()
{
  FABRIC_TRY("GLWidget::resetCameraPosition",

    m_camera.callMethod("", "resetPosition", 0, 0);

  );
  updateGL();
}

/*************************************************************************/
/* QGLWidget calls                                                       */
/*************************************************************************/

void GLWidget::getArgsForMouseEvent(QMouseEvent * event, std::vector<FabricCore::RTVal> & args)
{
  args.resize(4);
  args[0] = constructSInt32RTVal(event->pos().x());
  args[1] = constructSInt32RTVal(event->pos().y());
  int buttons = 0;
  if(event->buttons().testFlag(Qt::LeftButton))
    buttons += 1;
  if(event->buttons().testFlag(Qt::RightButton))
    buttons += 2;
  if(event->buttons().testFlag(Qt::MidButton))
    buttons += 4;
  args[2] = constructSInt32RTVal(buttons);
  int modifiers = 0;
  if(qApp->keyboardModifiers().testFlag(Qt::AltModifier))
    modifiers += 1;
  if(qApp->keyboardModifiers().testFlag(Qt::ShiftModifier))
    modifiers += 2;
  if(qApp->keyboardModifiers().testFlag(Qt::ControlModifier))
    modifiers += 4;
  args[3] = constructSInt32RTVal(modifiers);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{

  FABRIC_TRY("GLWidget::mousePressEvent",

    std::vector<FabricCore::RTVal> args;
    getArgsForMouseEvent(event, args);
    m_viewport.callMethod("", "mousePressEvent", args.size(), &args[0]);

  );
  updateGL();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
  FABRIC_TRY("GLWidget::mouseMoveEvent",

    std::vector<FabricCore::RTVal> args;
    getArgsForMouseEvent(event, args);
    m_viewport.callMethod("", "mouseMoveEvent", args.size(), &args[0]);

  );
  updateGL();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
  FABRIC_TRY("GLWidget::mouseReleaseEvent",

    std::vector<FabricCore::RTVal> args;
    getArgsForMouseEvent(event, args);
    m_viewport.callMethod("", "mouseReleaseEvent", args.size(), &args[0]);

  );
  updateGL();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
  FABRIC_TRY("GLWidget::wheelEvent",

    std::vector<FabricCore::RTVal> args(1);
    args[0] = constructSInt32RTVal(event->delta());
    m_viewport.callMethod("", "wheelEvent", args.size(), &args[0]);

  );
	updateGL();
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
// 	Qt::KeyboardModifiers key_modifiers = qApp->keyboardModifiers();

// 	// Move camera smoothly to center view on currently picked object, on pressing F (as in Maya)
// 	if (m_interactionState == GLWidget::USER_INTERACTION)
// 	{
// 		// Trigger camera focus
// 		if ( event->key()==Qt::Key_F )
// 		{
// //			m_focusing = true;
// //			m_focusStartCam = m_camera->copy();
// //			m_focusTimer.restart();
// 		}

// 		// Save Current View
// 		if ( event->key()==Qt::Key_S )
// 		{
// 			QImage capture = grabFrameBuffer();
// 			capture.save("/tmp/optixGrab.png");
// 			std::cout << "image Saved at " << "/tmp/optixGrab.png" << std::endl;
// 		}
// 	}

}

void GLWidget::initializeGL()
{
  FABRIC_TRY("GLWidget::initializeGL",

    m_viewport.callMethod("", "initializeGL", 0, 0);

  );
  m_requiresInitialize = false;
}

void GLWidget::resizeGL(int width, int height)
{
  FABRIC_TRY("GLWidget::resizeGL",

    m_viewport.setMember("width", constructUInt32RTVal(width));
    m_viewport.setMember("height", constructUInt32RTVal(height));
    m_viewport.callMethod("", "resizeGL", 0, 0);

  );
}




