#include "AEWidget.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QPainter>
#include <QtGui/QMenu>
#include <QtGui/QScrollBar>
#include <QtGui/QDialog>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>

#include "macros.h"
#include "SpliceStandalone.h"

using namespace FabricSplice;

AEWidget * AEWidget::create( FabricCore::RTVal param ,QWidget* parent)
{
	return new AEWidget(param ,parent);
}

QString AEWidget::getLabel()
{
	std::string labelText;

  FABRIC_TRY_RETURN("AEWidget::getLabel", QString(),
  
    labelText = m_param.callMethod("String", "getLabel", 0, 0).getStringCString();

  );
	
  labelText +="   ";
	return QString(labelText.c_str() );
}


QHBoxLayout * AEWidget::createLabelControlLayout()
{
  QHBoxLayout * controlLayout = new QHBoxLayout;

	m_defaultLabelWidget = new QLabel( getLabel() );

  FABRIC_TRY_IGNORE("AEWidget::createLabelControlLayout",
	
    m_defaultLabelWidget->setToolTip( m_param.callMethod("String", "getDescription", 0, 0).getStringCString() );

  );
	m_defaultLabelWidget->setMinimumWidth( 50 );
	m_defaultLabelWidget->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_defaultLabelWidget->setFixedHeight(18);
	m_defaultLabelWidget->setFont(getApplicationWidgetFont());
	controlLayout->setMargin( 2 );
	controlLayout->setSpacing( 1 );
	controlLayout->addWidget(m_defaultLabelWidget);

	return controlLayout;
}

void AEWidget::missingRegistration()
{
	QHBoxLayout * layout = createLabelControlLayout();

	QLabel * label = new QLabel( "Unknown Type" );
	label->setMinimumWidth( 50 );
	label->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
	layout->addWidget(label);
	layout->insertSpacing( -1 , 1000 );

	setLayout(layout);
}

AEWidget::AEWidget(FabricCore::RTVal param , QWidget* parent)
    : QWidget(parent)
{
		
	m_param  = param;

  FABRIC_TRY("AEWidget::AEWidget", 
    
    m_attributeName = m_param.callMethod("String", "getName", 0, 0).getStringCString();
  
  );
	
	// will be null in case no preset is setup .
	//m_presets		= m_description->getEntry<VariantMap>( "presets" );
	
	m_lock = false;
	m_connected = false;

  setMinimumWidth(40);
}

void AEWidget::switchLock()
{
	setLocked(!m_lock);
}

void AEWidget::setEnabledOnChildren(bool value)
{
	foreach (QObject * child , children())
	{
		if ( child->isWidgetType() )
		{
			const QMetaObject *	meta = child->metaObject();
			std::string className = meta->className();
			
			if ( className != "QLabel" && className != "QMenu" )
			{
				QWidget * widget = qobject_cast<QWidget*>(child);
				widget->setEnabled(value);
			}
		}
	}
}

void AEWidget::setLocked(bool value)
{
	m_lock = value;
	QPalette myPalette;
	if (m_lock)
	{
		setStyleSheet(
					"QSpinBox , QDoubleSpinBox , QLineEdit , QComboBox , QSlider "
					"{ background-color: #666666 ; color:#999999; } "
					"QCheckBox { color: #444444;}"
					);
		setEnabledOnChildren(false);
	}
	else
	{
		setStyleSheet("");
		setEnabledOnChildren(true);
	}
	setPalette(myPalette);
	
}

void AEWidget::setConnected(bool value)
{
	m_connected = value;
	if (m_connected)
	{
		setStyleSheet(
					"QSpinBox , QDoubleSpinBox , QLineEdit , QComboBox , QSlider "
					"{ background-color: #de727a ; color:#666666; } "
					"QCheckBox { color: #de727a;}"
					);
	}
	else
	{
		setStyleSheet("");
	}
	
}

void AEWidget::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu * menu = new QMenu(this);

	QString lockLabel = m_lock ? "Unlock Attribute" : "Lock Attribute";
	
	QAction * lock = new QAction( lockLabel ,this);
	connect(lock , SIGNAL(triggered()) , this , SLOT(switchLock()) );
	menu->addAction(lock);
	
	// add the action to the menu .
	// this is potentialy empty 
	menu->addActions(m_extraMenuActions);

	// show the menu 
	menu->exec(event->globalPos());
	
}

/*
void AEWidget::setPreset(VariantPtr value)
{
	setInternalValue(value);
}
*/

AEWidget::~AEWidget()
{
}	

void AEWidget::valueChanged()
{
	///emit AEWigetInternalChanged(m_attributeName);
}
	
void AEWidget::uiChanged()
{
	if (!m_silentUpdate)
	{
		emit AEWigetValueChanged(m_attributeName);
	}	
}

