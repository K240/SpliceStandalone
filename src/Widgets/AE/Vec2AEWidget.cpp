#include "Vec2AEWidget.h"

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

AEWidget * Vec2AEWidget::create( FabricCore::RTVal param , QWidget* parent)
{
	return new Vec2AEWidget(param ,parent);
}

Vec2AEWidget::Vec2AEWidget(FabricCore::RTVal param ,QWidget* parent)
    : AEWidget(param,parent)
{
	setLayout(createLabelControlLayout());

  m_listWidget = new QWidget();
  m_listWidget->setLayout(new QGridLayout(m_listWidget));
	layout()->addWidget(m_listWidget);

  m_validator = new QDoubleValidator(this);

  setRTVal(param);
			
	// QSpacerItem * spacerItem = new QSpacerItem(20,1,QSizePolicy::Expanding , QSizePolicy::Minimum);
	// list->addItem(spacerItem);

}

FabricCore::RTVal Vec2AEWidget::getValueArray()
{
  FabricCore::RTVal values;

  FABRIC_TRY_RETURN("Vec2AEWidget::getValueArray", FabricCore::RTVal(), 

  	values = constructRTVal("Vec2[]");
    values.setArraySize(m_widgetsX.size());

    std::vector<FabricCore::RTVal> args(2);
    for (unsigned int i = 0; i < m_widgetsX.size(); ++i)
    {
      QString x = m_widgetsX[i]->text();
      QString y = m_widgetsY[i]->text();
      args[0] = constructFloat32RTVal(x.toFloat());
      args[1] = constructFloat32RTVal(y.toFloat());
      values.setArrayElement(i, constructRTVal("Vec2", 2, &args[0]));
    }
  );
	return values;
}

void Vec2AEWidget::setRTVal(FabricCore::RTVal param)
{
  FABRIC_TRY("Vec2AEWidget::setRTVal",
  
    AEWidget::setRTVal(constructObjectRTVal("Vec2Parameter", 1, &param));
    setValueArray(m_param.callMethod("Vec2[]", "getValueArray", 0, 0));

  );
}

void Vec2AEWidget::setValueArray(FabricCore::RTVal values)
{
  FABRIC_TRY("Vec2AEWidget::setValueArray", 

    unsigned int precision = m_param.callMethod("UInt32", "getPrecision", 0, 0).getUInt32();

    // clear the layout
    if(values.getArraySize() != m_widgetsX.size())
    {
      QLayoutItem* item;
      while ( ( item = m_listWidget->layout()->takeAt( 0 ) ) != NULL )
      {
        delete item->widget();
        delete item;
      }

      m_validator->setDecimals(precision);
      m_validator->setNotation(QDoubleValidator::StandardNotation);

      QGridLayout * layout = (QGridLayout*)m_listWidget->layout();

      m_widgetsX.resize(values.getArraySize());
      m_widgetsY.resize(values.getArraySize());
      for (unsigned int i = 0; i < values.getArraySize(); ++i)
      {
        m_widgetsX[i] = new ItemWidget(m_listWidget);
        m_widgetsX[i]->setFont( getApplicationWidgetFont() );
        m_widgetsX[i]->setValidator(m_validator);
        m_widgetsY[i] = new ItemWidget(m_listWidget);
        m_widgetsY[i]->setFont( getApplicationWidgetFont() );
        m_widgetsY[i]->setValidator(m_validator);
    
        // connect the color box to uiChanged()
        connect( m_widgetsX[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsY[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        layout->addWidget( m_widgetsX[i], i, 0);
        layout->addWidget( m_widgetsY[i], i, 1);
      }
    }

    for (unsigned int i = 0; i < values.getArraySize(); ++i)
    {
      FabricCore::RTVal value = values.getArrayElement(i);
      m_widgetsX[i]->setText(QString::number(value.maybeGetMember("x").getFloat32(), 'g', precision));
      m_widgetsY[i]->setText(QString::number(value.maybeGetMember("y").getFloat32(), 'g', precision));
    }

  );
}

void Vec2AEWidget::uiChanged()
{
  FABRIC_TRY("Vec2AEWidget::uiChanged", 

    FabricCore::RTVal values = getValueArray();
    m_param.callMethod("", "setValueArray", 1, &values);

  );

  AEWidget::uiChanged();
}
