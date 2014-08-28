#include "SInt32SliderAEWidget.h"

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

AEWidget * SInt32SliderAEWidget::create( FabricCore::RTVal param , QWidget* parent)
{
	return new SInt32SliderAEWidget(param ,parent);
}

SInt32SliderAEWidget::SInt32SliderAEWidget(FabricCore::RTVal param ,QWidget* parent)
    : AEWidget(param,parent)
{
	setLayout(createLabelControlLayout());

  m_notifying = false;

  m_listWidget = new QWidget();
  m_listWidget->setLayout(new QGridLayout(m_listWidget));
	layout()->addWidget(m_listWidget);

  m_validator = new QIntValidator(this);

  setRTVal(param);
			
	// QSpacerItem * spacerItem = new QSpacerItem(20,1,QSizePolicy::Expanding , QSizePolicy::Minimum);
	// list->addItem(spacerItem);

}

FabricCore::RTVal SInt32SliderAEWidget::getValueArray()
{
  FabricCore::RTVal values;

  FABRIC_TRY_RETURN("SInt32SliderAEWidget::getValueArray", FabricCore::RTVal(), 

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

void SInt32SliderAEWidget::setRTVal(FabricCore::RTVal param)
{
  FABRIC_TRY("SInt32SliderAEWidget::setRTVal",
  
    AEWidget::setRTVal(constructObjectRTVal("SInt32Parameter", 1, &param));
    setValueArray(m_param.callMethod("SInt32[]", "getValueArray", 0, 0));

  );
}

void SInt32SliderAEWidget::setValueArray(FabricCore::RTVal values)
{
  FABRIC_TRY("SInt32SliderAEWidget::setValueArray", 

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

      QGridLayout * layout = (QGridLayout*)m_listWidget->layout();

      m_sliders.resize(values.getArraySize());
      m_widgets.resize(values.getArraySize());
      for (unsigned int i = 0; i < values.getArraySize(); ++i)
      {
        m_sliders[i] = new QSlider(m_listWidget);
        m_sliders[i]->setRange(bottom, top);
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
      int value = values.getArrayElement(i).getSInt32();
      m_widgets[i]->setText(QString::number(value));
    }

  );
}

void SInt32SliderAEWidget::uiChanged()
{
  FABRIC_TRY("SInt32SliderAEWidget::uiChanged", 

    FabricCore::RTVal values = getValueArray();
    m_param.callMethod("", "setValueArray", 1, &values);

  );

  AEWidget::uiChanged();
}

void SInt32SliderAEWidget::sliderMoved(int pos)
{ 
  if(m_notifying)
    return;
  m_notifying = true;

  QObject * sender = QObject::sender();
  for(size_t i=0;i<m_sliders.size();i++)
  {
    if(m_sliders[i] != sender)
      continue;

    m_widgets[i]->setText(QString::number(pos));
    break;
  }

  m_notifying = false;
  uiChanged();
}

void SInt32SliderAEWidget::textChanged(const QString & text)
{
  if(m_notifying)
    return;
  m_notifying = true;

  QObject * sender = QObject::sender();
  for(size_t i=0;i<m_widgets.size();i++)
  {
    if(m_widgets[i] != sender)
      continue;

    int value = m_widgets[i]->text().toInt();
    m_sliders[i]->setValue(value);
    break;
  }

  m_notifying = false;
  uiChanged();
}

