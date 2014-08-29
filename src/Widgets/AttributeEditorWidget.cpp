#include "AttributeEditorWidget.h"
#include "AE/AEWidgetFactory.h"
#include "AE/NodeData.h"
#include "AE/AEWidget.h"
#include "AE/AETreeView.h"
#include "AE/AEModel.h"
#include "AE/AEFilterModel.h"

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

	m_filterMode = AEFilterModeWildcard;
	m_proxyModel = NULL;
  m_model = NULL;
/*
	// there is currently a bug in Qt or our code that made this not working
	// it will be fixed and activated in future release
	m_filterLine = new QLineEdit();
	mainLayout->addWidget(m_filterLine);
*/
	m_treeView = new AETreeView(this);
	m_treeView->setSortingEnabled(false);
	mainLayout->addWidget(m_treeView);
	m_nodeData = NodeDataPtr( new NodeData() );

	connect( m_treeView , SIGNAL(widgetAdded(AEWidget*,std::string)) , 
			 this		, SLOT(widgetAdded(AEWidget*,std::string)) );
	
	setLayout(mainLayout);	

}

AttributeEditorWidget::~AttributeEditorWidget()
{
}	

void AttributeEditorWidget::clearTree()
{	
	// make sure we delete each item in the Map
  // m_treeView->model()->clear();
 //  QLayoutItem* item;
 //  while ( ( item = m_listWidget->layout()->takeAt( 0 ) ) != NULL )
 //  {
 //    delete item->widget();
 //    delete item;
 //  }

	// for (WidgetMap::iterator it = m_widgetMap.begin() ; it != m_widgetMap.end() ; ++it)
	// {
	// 	delete (*it).second;
	// }
	
	m_widgetMap.clear();
	
	if (m_treeView)
	{
		m_treeView->setModel(NULL);
	}

  if(m_model)
  {
    delete m_model;
    m_model = NULL;
  }
	
  if(m_proxyModel)
  {
  	delete m_proxyModel;
  	m_proxyModel = NULL;
  }
}	

void AttributeEditorWidget::updateParams()
{
  setWrapper(m_wrapper);
}

void AttributeEditorWidget::setWrapper(SpliceGraphWrapper::Ptr wrapper)
{
  printf("AttributeEditorWidget::setWrapper\n");
  m_wrapper = wrapper;
  if(m_wrapper)
  {
  
  	//update the data.
  	m_nodeData->setDefaultValues(wrapper->getGraph());
  	
  	clearTree();
  	m_model = new AEModel(m_nodeData , m_treeView);
  	
  	m_treeView->setModel(m_model);
  	m_treeView->resizeColumnToContents( 0 );
  	
  	m_treeView->expandRootItem();
  }
}

void AttributeEditorWidget::widgetValueChanged(std::string attributeName)
{
	// we need to make sure we keep the internal args in sync 
	if (m_nodeData->invalidateArg(attributeName))
	{
		// we emit an argsChanged so anythings relying on those args
		// know they need to update themselves if they want the accurate info
		// notes that this is only invalidate if the args was not already dirty

		// for applications using QT
    QSpliceGraphWrapper wrapper;
    wrapper.wrapper = m_wrapper;
		emit attributeChanged(wrapper, attributeName);

    m_nodeData->cleanAllArgs();
	}
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

void AttributeEditorWidget::removeProxyFilter()
{
	m_treeView->setModel(m_model);
}

void AttributeEditorWidget::addProxyFilter()
{
	if (!m_proxyModel)
	{
		m_proxyModel = new AEFilterModel(this);
		m_proxyModel->setSourceModel(m_model);
		m_proxyModel->setFilterRole(998); // AENameFilterRole
		m_proxyModel->setFilterKeyColumn(0);	
		m_proxyModel->setDynamicSortFilter(true);    
	}	     
	
	m_treeView->setModel(m_proxyModel);
	
}

void AttributeEditorWidget::filterTextChanged(QString text)
{
	m_proxyModel->invalidate();
	std::cout << "AttributeEditorWidget::filterTextChanged " << text.toStdString().c_str()  << std::endl;
	if ( m_filterMode == AEFilterModeRegEx )
		m_proxyModel->setFilterRegExp(text);
	else if ( m_filterMode == AEFilterModeWildcard )
		m_proxyModel->setFilterWildcard(text);
	else if ( m_filterMode == AEFilterModeString )
		m_proxyModel->setFilterFixedString(text);

	m_treeView->expandAllAfterSearch();
	m_filterText = text;
}

void AttributeEditorWidget::filterModeChanged(AEFilterMode mode)
{
	m_filterMode = mode;
	filterTextChanged( m_filterText );
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
  
    FabricCore::RTVal nameVal = constructStringRTVal(name.c_str());
  	NodeData::DGPortList & args = m_nodeData->getArgs();
    for(size_t i=0;i<args.size();i++)
    {
      if(args[i].getName() != name)
        continue;
  		widget->setPort( args[i] );
    }
  );
	
	widget->setSilentUpdate(false);	
}

void AttributeEditorWidget::updateAllWidget()
{
  NodeData::DGPortList & args = m_nodeData->getArgs();
	for (WidgetMap::iterator it = m_widgetMap.begin() ; it != m_widgetMap.end() ; it++ ) 
	{
    FABRIC_TRY("AttributeEditorWidget::updateAllWidget", 

      std::string name = it->first;
      for(size_t i=0;i<args.size();i++)
      {
        if(args[i].getName() == name)
        {
      		it->second->setPort( args[i] );
        }
      }
    );
	}
}
