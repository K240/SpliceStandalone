#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>

#include "ManipulationTool.h"

#include <FabricSplice.h>
#include "macros.h"

#include <map>
#include <iostream>

using namespace FabricSplice;

/////////////////////////////////////////////////////
// ManipulationCmd

FabricCore::RTVal ManipulationCmd::s_rtval_commands;

ManipulationCmd::ManipulationCmd()
{
  m_rtval_commands = s_rtval_commands;
}

ManipulationCmd::~ManipulationCmd()
{
}

bool ManipulationCmd::redoIt()
{
  if(m_rtval_commands.isValid())
  {
    for(int i=0; i<m_rtval_commands.getArraySize(); i++)
    {
      m_rtval_commands.getArrayElement(i).callMethod("", "doAction", 0, 0);
    }
  }
  return true;
}

bool ManipulationCmd::undoIt()
{
  if(m_rtval_commands.isValid())
  {
    for(int i=0; i<m_rtval_commands.getArraySize(); i++)
    {
      m_rtval_commands.getArrayElement(i).callMethod("", "undoAction", 0, 0);
    }
  }
  return true;
}

/////////////////////////////////////////////////////
// ManipulationTool

class EventFilterObject : public QObject
{
  public:
    ManipulationTool *tool;
    bool eventFilter(QObject *object, QEvent *event);
};

static EventFilterObject sEventFilterObject;

const char helpString[] = "Click and drag to interact with Fabric:Splice.";

ManipulationTool::ManipulationTool(GLWidget * glView) 
  : mView(glView) , m_active(false)
{

}

void ManipulationTool::toolOnSetup()
{
  FABRIC_TRY("ManipulationTool::toolOnSetup",
    
    FabricCore::RTVal eventDispatcherHandle = FabricSplice::constructObjectRTVal("EventDispatcherHandle");
    if(eventDispatcherHandle.isValid()){
      mEventDispatcher = eventDispatcherHandle.callMethod("EventDispatcher", "getEventDispatcher", 0, 0);

      if(mEventDispatcher.isValid()){
        mEventDispatcher.callMethod("", "activateManipulation", 0, 0);
      }
    }
  );
  m_active = true;

  sEventFilterObject.tool = this;
  mView->installEventFilter(&sEventFilterObject);
  mView->setFocus();
  mView->setMouseTracking(true);

  mView->updateGL();
}

void ManipulationTool::toolOffCleanup()
{

  mView->removeEventFilter(&sEventFilterObject);
  mView->clearFocus();

  FABRIC_TRY("ManipulationTool::toolOffCleanup",
    if(mEventDispatcher.isValid()){
      mEventDispatcher.callMethod("", "deactivateManipulation", 0, 0);
    }
  );
   
  m_active = false;
  mView->setMouseTracking(false);
  mView->updateGL();

}

bool EventFilterObject::eventFilter(QObject *object, QEvent *event)
{
  return tool->onEvent(event);
}

bool ManipulationTool::onEvent(QEvent *event)
{
  if(!mEventDispatcher.isValid())
  {
    return false;
  }

  // skip the alt key, so that we can continue to use the camera
  if(QApplication::keyboardModifiers().testFlag(Qt::AltModifier))
    return false;

  // Now we translate the Qt events to FabricEngine events..
  FabricCore::RTVal klevent;

  if(event->type() == QEvent::Enter)
  {
    // FABRIC_TRY_RETURN("ManipulationTool::onEvent", false,
      klevent = constructObjectRTVal("MouseEvent");
    // );
  }
  else if(event->type() == QEvent::Leave)
  {
    // FABRIC_TRY_RETURN("ManipulationTool::onEvent", false,
      klevent = constructObjectRTVal("MouseEvent");
    // );
  }
  else if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) 
  {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    // FABRIC_TRY_RETURN("ManipulationTool::onEvent", false,
      klevent = constructObjectRTVal("KeyEvent");
      klevent.setMember("key", constructUInt32RTVal(keyEvent->key()));
      klevent.setMember("count", constructUInt32RTVal(keyEvent->count()));
      klevent.setMember("isAutoRepeat", constructBooleanRTVal(keyEvent->isAutoRepeat()));
    // );
  } 
  else if ( event->type() == QEvent::MouseMove || 
            event->type() == QEvent::MouseButtonDblClick || 
            event->type() == QEvent::MouseButtonPress || 
            event->type() == QEvent::MouseButtonRelease
          ) 
  {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

    // FABRIC_TRY_RETURN("ManipulationTool::onEvent", false,
      klevent = constructObjectRTVal("MouseEvent");

      FabricCore::RTVal klpos = constructRTVal("Vec2");
      klpos.setMember("x", constructFloat32RTVal(mouseEvent->pos().x()));
      klpos.setMember("y", constructFloat32RTVal(mouseEvent->pos().y()));

      klevent.setMember("button", constructUInt32RTVal(mouseEvent->button()));
      klevent.setMember("buttons", constructUInt32RTVal(mouseEvent->buttons()));
      klevent.setMember("pos", klpos);
    // );
  } 
  else if (event->type() == QEvent::Wheel) 
  {
    QWheelEvent *mouseWheelEvent = static_cast<QWheelEvent *>(event);

    // FABRIC_TRY_RETURN("ManipulationTool::onEvent", false,
      klevent = constructObjectRTVal("MouseWheelEvent");

      FabricCore::RTVal klpos = constructRTVal("Vec2");
      klpos.setMember("x", constructFloat32RTVal(mouseWheelEvent->pos().x()));
      klpos.setMember("y", constructFloat32RTVal(mouseWheelEvent->pos().y()));

      klevent.setMember("buttons", constructUInt32RTVal(mouseWheelEvent->buttons()));
      klevent.setMember("delta", constructSInt32RTVal(mouseWheelEvent->delta()));
      klevent.setMember("pos", klpos);
    // );
  }

  if(klevent.isValid())
  {
    int eventType = int(event->type());

    bool result = false;

    // FABRIC_TRY_RETURN("ManipulationTool::onEvent", false,
      klevent.setMember("eventType", constructUInt32RTVal(eventType));

      QInputEvent *inputEvent = static_cast<QInputEvent *>(event);
      klevent.setMember("modifiers", constructUInt32RTVal(inputEvent->modifiers()));

      //////////////////////////
      // Setup the viewport
      FabricCore::RTVal inlineViewport = mView->getInlineViewport();
      klevent.setMember("viewport", inlineViewport);

      //////////////////////////
      // Setup the Host
      // We cannot set an interface value via RTVals.
      FabricCore::RTVal host = constructObjectRTVal("Host");
      host.setMember("hostName", constructStringRTVal("Splice Standalone"));
      klevent.setMember("host", host);

      //////////////////////////
      // Invoke the event...
      mEventDispatcher.callMethod("Boolean", "onEvent", 1, &klevent);

      result = klevent.callMethod("Boolean", "isAccepted", 0, 0).getBoolean();
      if(result)
        event->accept();

      if(host.maybeGetMember("redrawRequested").getBoolean())
        mView->updateGL();

      if(host.callMethod("Boolean", "undoRedoCommandsAdded", 0, 0).getBoolean())
      {
        // Cache the rtvals in a static variable that the command will then stor in the undo stack.
        ManipulationCmd::s_rtval_commands = host.callMethod("UndoRedoCommand[]", "getUndoRedoCommands", 0, 0);
      }

      klevent.invalidate();
    // );
    return result;
  }
  // the event was not handled by FabricEngine manipulation system. 
  return false;
}
