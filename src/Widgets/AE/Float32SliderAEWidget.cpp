#include "Float32SliderAEWidget.h"

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

AEWidget * Float32SliderAEWidget::create( FabricCore::RTVal param , QWidget* parent)
{
	return new Float32SliderAEWidget(param ,parent);
}

Float32SliderAEWidget::Float32SliderAEWidget(FabricCore::RTVal param ,QWidget* parent)
    : AEWidget(param,parent)
{
	setLayout(createLabelControlLayout());

  m_notifying = false;

  m_listWidget = new QWidget();
  m_listWidget->setLayout(new QGridLayout(m_listWidget));
	layout()->addWidget(m_listWidget);

  m_validator = new DoubleValidator(this);

  setRTVal(param);
			
	// QSpacerItem * spacerItem = new QSpacerItem(20,1,QSizePolicy::Expanding , QSizePolicy::Minimum);
	// list->addItem(spacerItem);

}

FabricCore::RTVal Float32SliderAEWidget::getValueArray()
{
  FabricCore::RTVal values;

  FABRIC_TRY_RETURN("Float32SliderAEWidget::getValueArray", FabricCore::RTVal(), 

  	values = constructRTVal("Float32[]");
    values.setArraySize(m_widgets.size());
  	for (unsigned int i = 0; i < m_widgets.size(); ++i)
    {
      QString value = m_widgets[i]->text();
      values.setArrayElement(i, constructFloat32RTVal(value.toFloat()));
    }
  );
	return values;
}

void Float32SliderAEWidget::setRTVal(FabricCore::RTVal param)
{
  FABRIC_TRY("Float32SliderAEWidget::setRTVal",
  
    AEWidget::setRTVal(constructObjectRTVal("Float32Parameter", 1, &param));
    setValueArray(m_param.callMethod("Float32[]", "getValueArray", 0, 0));

  );
}

void Float32SliderAEWidget::setValueArray(FabricCore::RTVal values)
{
  FABRIC_TRY("Float32SliderAEWidget::setValueArray", 

    unsigned int precision = m_param.callMethod("UInt32", "getPrecision", 0, 0).getUInt32();

    // clear the layout
    if(values.getArraySize() != m_widgets.size())
    {
      QLayoutItem* item;
      while ( ( item = m_listWidget->layout()->takeAt( 0 ) ) != NULL )
      {
        delete item->widget();
        delete item;
      }

      float bottom = m_param.callMethod("Float32", "getMin", 0, 0).getFloat32();
      float top = m_param.callMethod("Float32", "getMax", 0, 0).getFloat32();

      m_validator->setRange(bottom, top, precision);
      m_validator->setNotation(QDoubleValidator::StandardNotation);

      QGridLayout * layout = (QGridLayout*)m_listWidget->layout();

      m_sliders.resize(values.getArraySize());
      m_widgets.resize(values.getArraySize());
      for (unsigned int i = 0; i < values.getArraySize(); ++i)
      {
        m_sliders[i] = new QSlider(m_listWidget);
        m_sliders[i]->setRange(bottom * 1000.0f, top * 1000.0f);
        m_sliders[i]->setOrientation(Qt::Horizontal);

        m_widgets[i] = new ItemWidget(m_listWidget);
        m_widgets[i]->setFont( getApplicationWidgetFont() );
        m_widgets[i]->setValidator(m_validator);
        m_widgets[i]->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    
        connect( m_sliders[i], SIGNAL( sliderMoved(int) ), this , SLOT( sliderMoved(int) ) );
        connect( m_widgets[i], SIGNAL( textChanged(const QString &) ), this , SLOT( textChanged(const QString &) ) );

        layout->addWidget( m_sliders[i], i, 0);
        layout->addWidget( m_widgets[i], i, 1);
      }
    }

    for (unsigned int i = 0; i < values.getArraySize(); ++i)
    {
      float value = values.getArrayElement(i).getFloat32();
      m_widgets[i]->setText(QString::number(value, 'g', precision));
    }

  );
}

void Float32SliderAEWidget::uiChanged()
{
  FABRIC_TRY("Float32SliderAEWidget::uiChanged", 

    FabricCore::RTVal values = getValueArray();
    m_param.callMethod("", "setValueArray", 1, &values);

  );

  AEWidget::uiChanged();
}

void Float32SliderAEWidget::sliderMoved(int pos)
{ 
  if(m_notifying)
    return;
  m_notifying = true;

  QObject * sender = QObject::sender();
  for(size_t i=0;i<m_sliders.size();i++)
  {
    if(m_sliders[i] != sender)
      continue;

    double value = double(pos) / 1000.0;
    m_widgets[i]->setText(QString::number(value, 'g', m_validator->decimals()));
    break;
  }

  m_notifying = false;
  uiChanged();
}

void Float32SliderAEWidget::textChanged(const QString & text)
{
  if(m_notifying)
    return;
  m_notifying = true;

  QObject * sender = QObject::sender();
  for(size_t i=0;i<m_widgets.size();i++)
  {
    if(m_widgets[i] != sender)
      continue;

    float value = m_widgets[i]->text().toFloat();
    m_sliders[i]->setValue(value * 1000.0);
    break;
  }

  m_notifying = false;
  uiChanged();
}

