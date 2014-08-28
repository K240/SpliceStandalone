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
  FabricCore::RTVal currentArgs = m_wrapper->getParameters();
  setParams(currentArgs);
}

void AttributeEditorWidget::setWrapper(SpliceGraphWrapper::Ptr wrapper)
{
  m_wrapper = wrapper;
  if(m_wrapper)
    setParams(m_wrapper->getParameters());  
}

void AttributeEditorWidget::setParams(FabricCore::RTVal newArgs)
{	
	//update the data.
	m_nodeData->setDefaultValues(newArgs);
	
	clearTree();
	m_model = new AEModel(m_nodeData , m_treeView);
	
	m_treeView->setModel(m_model);
	m_treeView->resizeColumnToContents( 0 );
	
//	this is currently de-acticated 
//	addProxyFilter();
//	connect( m_filterLine	, SIGNAL(textChanged(const QString &)) ,
//					 this			, SLOT(filterTextChanged(QString )) );

	m_treeView->expandRootItem();
}

FabricCore::RTVal AttributeEditorWidget::getArgs()
{
  FABRIC_TRY_RETURN("AttributeEditorWidget::getArgs", FabricCore::RTVal(),

  	FabricCore::RTVal currentArgs = m_nodeData->getArgs();
  	FabricCore::RTVal dirtyArgs = m_nodeData->getDirtyArgs();
    
    FabricCore::RTVal children = dirtyArgs.callMethod("Parameter[]", "getChildren", 0, 0);
  	for(size_t i = 0; i < children.getArraySize(); ++i)
  	{
      FabricCore::RTVal child = children.getArrayElement(i);
  		std::string argName = child.callMethod("String", "getName", 0, 0).getStringCString();
  		WidgetMap::iterator it = m_widgetMap.find(argName);
  		if ( it != m_widgetMap.end() )
  		{
  			AEWidget * wdg = it->second;
        FabricCore::RTVal newChild = wdg->getRTVal();
  			currentArgs.callMethod("", "addChild", 1, &newChild );
  		}
  		else
  		{
  			std::cout << "missing Widget in the MAP ??"<< std::endl;
  		}
  	}
  	dirtyArgs.callMethod("", "clear", 0, 0);
  	return currentArgs;

  );
}


void AttributeEditorWidget::widgetValueChanged(std::string attributeName)
{
	// we need to make sure we keep the internal args in sync 
	AEWidget * widget = m_widgetMap[attributeName];
	if (widget)
	{
    FABRIC_TRY("AttributeEditorWidget::widgetValueChanged", 
  
      FabricCore::RTVal internalRTVal = widget->getRTVal();
  		m_nodeData->getArgs().callMethod("", "addChild", 1, &internalRTVal);

    );
	}

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

void AttributeEditorWidget::setWidgetRTVal( std::string name, FabricCore::RTVal param , bool silent )
{
	// we want to make sure our args stay in sync
  FABRIC_TRY("AttributeEditorWidget::setWidgetRTVal", 

  	// in case we are displaying the UI we want to update it too 
  	AEWidget * widget = m_widgetMap[name];
  	if (widget)
  	{
  		// we set the internal value silently as no to triger more updates
  		widget->setSilentUpdate(silent);
  		widget->setRTVal( param );
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
  	FabricCore::RTVal value = m_nodeData->getArgs().callMethod("Parameter", "getChildByName", 1, &nameVal);
  	if(!value.isNullObject())
  		widget->setRTVal( value );

  );
	
	widget->setSilentUpdate(false);	
}

FabricCore::RTVal AttributeEditorWidget::getAttributeValue( std::string attributeName )
{
  FABRIC_TRY_RETURN("AttributeEditorWidget::getAttributeValue", FabricCore::RTVal(),
  
  	FabricCore::RTVal currentArgs = m_nodeData->getArgs();	
  	FabricCore::RTVal dirtyArgs = m_nodeData->getDirtyArgs();
    FabricCore::RTVal nameVal = constructStringRTVal(attributeName.c_str());
  	FabricCore::RTVal param = dirtyArgs.callMethod("Parameter", "getChildByName", 1, &nameVal);
  	if(!param.isNullObject())	
  	{
  		WidgetMap::iterator it = m_widgetMap.find(attributeName);
      FabricCore::RTVal currentParam = it->second->getRTVal();
  		currentArgs.callMethod("", "addChild", 1, &currentParam);
  		dirtyArgs.callMethod("", "deleteChildByName", 1, &nameVal);
  	}

    return currentArgs.callMethod("Parameter", "getChildByName", 1, &nameVal);

   );
}

void AttributeEditorWidget::printArgs()
{
  FABRIC_TRY("AttributeEditorWidget::printArgs", 

  	getArgs().callMethod("", "printDescription", 0, 0);

  );
}

void AttributeEditorWidget::updateAllWidget()
{
	FabricCore::RTVal args = m_nodeData->getArgs();

	for (WidgetMap::iterator it = m_widgetMap.begin() ; it != m_widgetMap.end() ; it++ ) 
	{
    FABRIC_TRY("AttributeEditorWidget::updateAllWidget", 

      FabricCore::RTVal nameVal = constructStringRTVal(it->first.c_str());
      FabricCore::RTVal child = args.callMethod("Parameter", "getChildByName", 1, &nameVal);
  		it->second->setRTVal( child );

    );
	}
}
