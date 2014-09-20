#include "AttributeEditorWidget.h"
#include "AE/AEWidgetFactory.h"
#include "AE/AEWidget.h"

#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>

#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtGui/QMenu>
#include <QtGui/QScrollBar>
#include <QtGui/QFrame>
#include <QtGui/QDialog>		
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>

#include "macros.h"

using namespace FabricSplice;

AttributeEditorWidget::AttributeEditorWidget(QWidget* parent)
    : QWidget(parent)
{
	
	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);
	setLayout(mainLayout);	
}

AttributeEditorWidget::~AttributeEditorWidget()
{
}	

void AttributeEditorWidget::clearAllWidgets()
{
  // todo: remove all widgets
}

void AttributeEditorWidget::setWrapper(SpliceGraphWrapper::Ptr wrapper)
{
  printf("AttributeEditorWidget::setWrapper\n");
  m_wrapper = wrapper;
  if(m_wrapper)
  {
    // todo: get all ports etc
  }
}

void AttributeEditorWidget::widgetValueChanged(std::string attributeName)
{
	// for applications using QT
  QSpliceGraphWrapper wrapper;
  wrapper.wrapper = m_wrapper;
	emit attributeChanged(wrapper, attributeName);
}

AEWidget * AttributeEditorWidget::getWidget(std::string argName)
{
	return m_widgetMap[argName];
}

void AttributeEditorWidget::setWidgetPort( std::string name, FabricSplice::DGPort port , bool silent )
{
	// we want to make sure our args stay in sync
  FABRIC_TRY("AttributeEditorWidget::setWidgetRTVal", 

  	// in case we are displaying the UI we want to update it too 
  	AEWidget * widget = m_widgetMap[name];
  	if (widget)
  	{
  		// we set the internal value silently as no to triger more updates
  		widget->setSilentUpdate(silent);
  		widget->setPort( port );
  		widget->setSilentUpdate(false);

  		// the value changed this might be interesting to other part of the system		
  		emit valueForWidgetChanged( name );
  	}

  );
	
}

void AttributeEditorWidget::setWidgetConnected(std::string name , bool value)
{
	AEWidget * widget = getWidget(name);
	if (widget)
	{
		widget->setConnected(value);
	}
}

void AttributeEditorWidget::setWidgetLocked(std::string name, bool value)
{
	AEWidget * widget = getWidget(name);
	if (widget)
	{
		widget->setLocked(value);
	}
}

void AttributeEditorWidget::widgetAdded(AEWidget * widget , std::string name)
{
	m_widgetMap[name] = widget;
	
	connect(widget, SIGNAL(AEWigetValueChanged(std::string)) ,
			this , SLOT(widgetValueChanged(std::string)) );

	// we silence the uiUpate signal as to double update
	widget->setSilentUpdate(true);
	
  FABRIC_TRY("AttributeEditorWidget::widgetAdded", 

    // todo: widget added  
   //  FabricCore::RTVal nameVal = constructStringRTVal(name.c_str());
  	// NodeData::DGPortList & args = m_nodeData->getArgs();
   //  for(size_t i=0;i<args.size();i++)
   //  {
   //    if(args[i].getName() != name)
   //      continue;
  	// 	widget->setPort( args[i] );
   //  }
  );
	
	widget->setSilentUpdate(false);	
}

void AttributeEditorWidget::updateAllWidgets()
{
  // todo: update all widgets
 //  NodeData::DGPortList & args = m_nodeData->getArgs();
	// for (WidgetMap::iterator it = m_widgetMap.begin() ; it != m_widgetMap.end() ; it++ ) 
	// {
 //    FABRIC_TRY("AttributeEditorWidget::updateAllWidgets", 

 //      std::string name = it->first;
 //      for(size_t i=0;i<args.size();i++)
 //      {
 //        if(args[i].getName() == name)
 //        {
 //      		it->second->setPort( args[i] );
 //        }
 //      }
 //    );
	// }
}
