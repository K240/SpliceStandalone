#include "SInt32AEWidget.h"

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

AEWidget * SInt32AEWidget::create( FabricCore::RTVal param , QWidget* parent)
{
	return new SInt32AEWidget(param ,parent);
}

SInt32AEWidget::SInt32AEWidget(FabricCore::RTVal param ,QWidget* parent)
    : AEWidget(param,parent)
{
	setLayout(createLabelControlLayout());

  m_listWidget = new QWidget();
  m_listWidget->setLayout(new QVBoxLayout(m_listWidget));
	layout()->addWidget(m_listWidget);

  m_validator = new QIntValidator(this);

  setRTVal(param);
			
	// QSpacerItem * spacerItem = new QSpacerItem(20,1,QSizePolicy::Expanding , QSizePolicy::Minimum);
	// list->addItem(spacerItem);

}

FabricCore::RTVal SInt32AEWidget::getValueArray()
{
  FabricCore::RTVal values;

  FABRIC_TRY_RETURN("SInt32AEWidget::getValueArray", FabricCore::RTVal(), 

  	values = constructRTVal("SInt32[]");
    values.setArraySize(m_widgets.size());
  	for (unsigned int i = 0; i < m_widgets.size(); ++i)
    {
      QString value = m_widgets[i]->text();
      values.setArrayElement(i, constructSInt32RTVal(value.toInt()));
    }
  );
	return values;
}

void SInt32AEWidget::setRTVal(FabricCore::RTVal param)
{
  FABRIC_TRY("SInt32AEWidget::setRTVal",
  
    AEWidget::setRTVal(constructObjectRTVal("SInt32Parameter", 1, &param));
    setValueArray(m_param.callMethod("SInt32[]", "getValueArray", 0, 0));

  );
}

void SInt32AEWidget::setValueArray(FabricCore::RTVal values)
{
  FABRIC_TRY("SInt32AEWidget::setValueArray", 

    // clear the layout
    if(values.getArraySize() != m_widgets.size())
    {
      QLayoutItem* item;
      while ( ( item = m_listWidget->layout()->takeAt( 0 ) ) != NULL )
      {
        delete item->widget();
        delete item;
      }

      int bottom = m_param.callMethod("SInt32", "getMin", 0, 0).getSInt32();
      int top = m_param.callMethod("SInt32", "getMax", 0, 0).getSInt32();

      m_validator->setRange(bottom, top);

      m_widgets.resize(values.getArraySize());
      for (unsigned int i = 0; i < values.getArraySize(); ++i)
      {
        m_widgets[i] = new ItemWidget(m_listWidget);
        m_widgets[i]->setFont( getApplicationWidgetFont() );
        m_widgets[i]->setValidator(m_validator);
    
        // connect the color box to uiChanged()
        connect( m_widgets[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        m_listWidget->layout()->addWidget( m_widgets[i]);
      }
    }

    for (unsigned int i = 0; i < values.getArraySize(); ++i)
    {
      int value = values.getArrayElement(i).getSInt32();
      m_widgets[i]->setText(QString::number(value));
    }

  );
}

void SInt32AEWidget::uiChanged()
{
  FABRIC_TRY("SInt32AEWidget::uiChanged", 

    FabricCore::RTVal values = getValueArray();
    m_param.callMethod("", "setValueArray", 1, &values);

  );

  AEWidget::uiChanged();
}
