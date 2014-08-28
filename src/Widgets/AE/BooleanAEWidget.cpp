#include "BooleanAEWidget.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QPainter>
#include <QtGui/QMenu>
#include <QtGui/QScrollBar>
#include <QtGui/QDialog>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QSizePolicy>

#include "macros.h"
#include "SpliceStandalone.h"

using namespace FabricSplice;

AEWidget * BooleanAEWidget::create( FabricCore::RTVal param , QWidget* parent)
{
	return new BooleanAEWidget(param ,parent);
}

BooleanAEWidget::BooleanAEWidget(FabricCore::RTVal param ,QWidget* parent)
    : AEWidget(param,parent)
{
	setLayout(createLabelControlLayout());

  m_listWidget = new QWidget();
  m_listWidget->setLayout(new QVBoxLayout(m_listWidget));
	layout()->addWidget(m_listWidget);

  setRTVal(param);
			
	// QSpacerItem * spacerItem = new QSpacerItem(20,1,QSizePolicy::Expanding , QSizePolicy::Minimum);
	// list->addItem(spacerItem);

}

FabricCore::RTVal BooleanAEWidget::getValueArray()
{
  FabricCore::RTVal values;

  FABRIC_TRY_RETURN("BooleanAEWidget::getValueArray", FabricCore::RTVal(), 

  	values = constructRTVal("Boolean[]");
    values.setArraySize(m_widgets.size());
  	for (unsigned int i = 0; i < m_widgets.size(); ++i)
      values.setArrayElement(i, constructBooleanRTVal(m_widgets[i]->checkState() == Qt::Checked));

  );
	return values;
}

void BooleanAEWidget::setRTVal(FabricCore::RTVal param)
{
  FABRIC_TRY("BooleanAEWidget::setRTVal",
  
    AEWidget::setRTVal(constructObjectRTVal("BooleanParameter", 1, &param));
    setValueArray(m_param.callMethod("Boolean[]", "getValueArray", 0, 0));

  );
}

void BooleanAEWidget::setValueArray(FabricCore::RTVal values)
{
  FABRIC_TRY("BooleanAEWidget::setValueArray", 

    // clear the layout
    if(values.getArraySize() != m_widgets.size())
    {
      QLayoutItem* item;
      while ( ( item = m_listWidget->layout()->takeAt( 0 ) ) != NULL )
      {
        delete item->widget();
        delete item;
      }

      m_widgets.resize(values.getArraySize());
      for (unsigned int i = 0; i < values.getArraySize(); ++i)
      {
        m_widgets[i] = new ItemWidget(m_listWidget);
        m_widgets[i]->setFont( getApplicationWidgetFont() );
    
        // connect the color box to uiChanged()
        connect( m_widgets[i], SIGNAL( stateChanged(int) ), this , SLOT( uiChanged() ) );
        m_listWidget->layout()->addWidget( m_widgets[i]);
      }
    }

    for (unsigned int i = 0; i < values.getArraySize(); ++i)
    {
      if ( values.getArrayElement(i).getBoolean() )
        m_widgets[i]->setCheckState( Qt::Checked );
      else
        m_widgets[i]->setCheckState( Qt::Unchecked );
    }

  );
}

void BooleanAEWidget::uiChanged()
{
  FABRIC_TRY("BooleanAEWidget::uiChanged", 

    FabricCore::RTVal values = getValueArray();
    m_param.callMethod("", "setValueArray", 1, &values);

  );
  AEWidget::uiChanged();
}
