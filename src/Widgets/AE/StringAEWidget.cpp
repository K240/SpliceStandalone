#include "StringAEWidget.h"

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
#include "utility.h"
#include "SpliceStandalone.h"

using namespace FabricSplice;

AEWidget * StringAEWidget::create( FabricCore::RTVal param , QWidget* parent)
{
	return new StringAEWidget(param ,parent);
}

StringAEWidget::StringAEWidget(FabricCore::RTVal param ,QWidget* parent)
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

FabricCore::RTVal StringAEWidget::getValueArray()
{
  FabricCore::RTVal values;

  FABRIC_TRY_RETURN("StringAEWidget::getValueArray", FabricCore::RTVal(), 

  	values = constructRTVal("String[]");
    values.setArraySize(m_widgets.size());
  	for (unsigned int i = 0; i < m_widgets.size(); ++i)
    {
      std::string value = stdStringFromQString(m_widgets[i]->text());
      values.setArrayElement(i, constructStringRTVal(value.c_str()));
    }
  );
	return values;
}

void StringAEWidget::setRTVal(FabricCore::RTVal param)
{
  FABRIC_TRY("StringAEWidget::setRTVal",
  
    AEWidget::setRTVal(constructObjectRTVal("StringParameter", 1, &param));
    setValueArray(m_param.callMethod("String[]", "getValueArray", 0, 0));

  );
}

void StringAEWidget::setValueArray(FabricCore::RTVal values)
{
  FABRIC_TRY("StringAEWidget::setValueArray", 

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
        connect( m_widgets[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        m_listWidget->layout()->addWidget( m_widgets[i]);
      }
    }

    for (unsigned int i = 0; i < values.getArraySize(); ++i)
    {
      std::string value = values.getArrayElement(i).getStringCString();
      m_widgets[i]->setText(value.c_str());
    }

  );
}

void StringAEWidget::uiChanged()
{
  FABRIC_TRY("StringAEWidget::uiChanged", 

    FabricCore::RTVal values = getValueArray();
    m_param.callMethod("", "setValueArray", 1, &values);

  );

  AEWidget::uiChanged();
}
