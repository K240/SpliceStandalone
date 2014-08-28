#include "XfoAEWidget.h"

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

AEWidget * XfoAEWidget::create( FabricCore::RTVal param , QWidget* parent)
{
	return new XfoAEWidget(param ,parent);
}

XfoAEWidget::XfoAEWidget(FabricCore::RTVal param ,QWidget* parent)
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

FabricCore::RTVal XfoAEWidget::getValueArray()
{
  FabricCore::RTVal values;

  FABRIC_TRY_RETURN("XfoAEWidget::getValueArray", FabricCore::RTVal(), 

  	values = constructRTVal("Xfo[]");
    values.setArraySize(m_widgetsTrX.size());

    std::vector<FabricCore::RTVal> args(3);
    for (unsigned int i = 0; i < m_widgetsTrX.size(); ++i)
    {
      QString trX = m_widgetsTrX[i]->text();
      QString trY = m_widgetsTrY[i]->text();
      QString trZ = m_widgetsTrZ[i]->text();
      args[0] = constructFloat32RTVal(trX.toFloat());
      args[1] = constructFloat32RTVal(trY.toFloat());
      args[2] = constructFloat32RTVal(trZ.toFloat());
      FabricCore::RTVal tr = constructRTVal("Vec3", 3, &args[0]);

      QString rotX = m_widgetsRotX[i]->text();
      QString rotY = m_widgetsRotY[i]->text();
      QString rotZ = m_widgetsRotZ[i]->text();
      args[0] = constructFloat32RTVal(rotX.toFloat());
      args[1] = constructFloat32RTVal(rotY.toFloat());
      args[2] = constructFloat32RTVal(rotZ.toFloat());
      FabricCore::RTVal euler = constructRTVal("Euler", 3, &args[0]);
      FabricCore::RTVal ori = constructRTVal("Quat", 1, &euler);

      QString sclX = m_widgetsSclX[i]->text();
      QString sclY = m_widgetsSclY[i]->text();
      QString sclZ = m_widgetsSclZ[i]->text();
      args[0] = constructFloat32RTVal(sclX.toFloat());
      args[1] = constructFloat32RTVal(sclY.toFloat());
      args[2] = constructFloat32RTVal(sclZ.toFloat());
      FabricCore::RTVal scl = constructRTVal("Vec3", 3, &args[0]);

      args[0] = tr;
      args[1] = ori;
      args[2] = scl;

      values.setArrayElement(i, constructRTVal("Xfo", 3, &args[0]));
    }
  );
	return values;
}

void XfoAEWidget::setRTVal(FabricCore::RTVal param)
{
  FABRIC_TRY("XfoAEWidget::setRTVal",
  
    AEWidget::setRTVal(constructObjectRTVal("XfoParameter", 1, &param));
    setValueArray(m_param.callMethod("Xfo[]", "getValueArray", 0, 0));

  );
}

void XfoAEWidget::setValueArray(FabricCore::RTVal values)
{
  FABRIC_TRY("XfoAEWidget::setValueArray", 

    unsigned int precision = 3;

    // clear the layout
    if(values.getArraySize() != m_widgetsTrX.size())
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

      m_widgetsTrX.resize(values.getArraySize());
      m_widgetsTrY.resize(values.getArraySize());
      m_widgetsTrZ.resize(values.getArraySize());
      m_widgetsRotX.resize(values.getArraySize());
      m_widgetsRotY.resize(values.getArraySize());
      m_widgetsRotZ.resize(values.getArraySize());
      m_widgetsSclX.resize(values.getArraySize());
      m_widgetsSclY.resize(values.getArraySize());
      m_widgetsSclZ.resize(values.getArraySize());
      for (unsigned int i = 0; i < values.getArraySize(); ++i)
      {
        m_widgetsTrX[i] = new ItemWidget(m_listWidget);
        m_widgetsTrX[i]->setFont( getApplicationWidgetFont() );
        m_widgetsTrX[i]->setValidator(m_validator);
        m_widgetsTrY[i] = new ItemWidget(m_listWidget);
        m_widgetsTrY[i]->setFont( getApplicationWidgetFont() );
        m_widgetsTrY[i]->setValidator(m_validator);
        m_widgetsTrZ[i] = new ItemWidget(m_listWidget);
        m_widgetsTrZ[i]->setFont( getApplicationWidgetFont() );
        m_widgetsTrZ[i]->setValidator(m_validator);
        m_widgetsRotX[i] = new ItemWidget(m_listWidget);
        m_widgetsRotX[i]->setFont( getApplicationWidgetFont() );
        m_widgetsRotX[i]->setValidator(m_validator);
        m_widgetsRotY[i] = new ItemWidget(m_listWidget);
        m_widgetsRotY[i]->setFont( getApplicationWidgetFont() );
        m_widgetsRotY[i]->setValidator(m_validator);
        m_widgetsRotZ[i] = new ItemWidget(m_listWidget);
        m_widgetsRotZ[i]->setFont( getApplicationWidgetFont() );
        m_widgetsRotZ[i]->setValidator(m_validator);
        m_widgetsSclX[i] = new ItemWidget(m_listWidget);
        m_widgetsSclX[i]->setFont( getApplicationWidgetFont() );
        m_widgetsSclX[i]->setValidator(m_validator);
        m_widgetsSclY[i] = new ItemWidget(m_listWidget);
        m_widgetsSclY[i]->setFont( getApplicationWidgetFont() );
        m_widgetsSclY[i]->setValidator(m_validator);
        m_widgetsSclZ[i] = new ItemWidget(m_listWidget);
        m_widgetsSclZ[i]->setFont( getApplicationWidgetFont() );
        m_widgetsSclZ[i]->setValidator(m_validator);
    
        // connect the color box to uiChanged()
        connect( m_widgetsTrX[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsTrY[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsTrZ[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsRotX[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsRotY[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsRotZ[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsSclX[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsSclY[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsSclZ[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );

        layout->addWidget( m_widgetsTrX[i], i * 3 + 0, 0);
        layout->addWidget( m_widgetsTrY[i], i * 3 + 0, 1);
        layout->addWidget( m_widgetsTrZ[i], i * 3 + 0, 2);
        layout->addWidget( m_widgetsRotX[i], i * 3 + 1, 0);
        layout->addWidget( m_widgetsRotY[i], i * 3 + 1, 1);
        layout->addWidget( m_widgetsRotZ[i], i * 3 + 1, 2);
        layout->addWidget( m_widgetsSclX[i], i * 3 + 2, 0);
        layout->addWidget( m_widgetsSclY[i], i * 3 + 2, 1);
        layout->addWidget( m_widgetsSclZ[i], i * 3 + 2, 2);
      }
    }

    for (unsigned int i = 0; i < values.getArraySize(); ++i)
    {
      FabricCore::RTVal value = values.getArrayElement(i);
      FabricCore::RTVal tr = value.maybeGetMember("tr");
      FabricCore::RTVal ori = value.maybeGetMember("ori");
      FabricCore::RTVal euler = ori.callMethod("Vec3", "toEulerAngles", 0, 0);
      FabricCore::RTVal scl = value.maybeGetMember("sc");
      m_widgetsTrX[i]->setText(QString::number(tr.maybeGetMember("x").getFloat32(), 'g', precision));
      m_widgetsTrY[i]->setText(QString::number(tr.maybeGetMember("y").getFloat32(), 'g', precision));
      m_widgetsTrZ[i]->setText(QString::number(tr.maybeGetMember("z").getFloat32(), 'g', precision));
      m_widgetsRotX[i]->setText(QString::number(euler.maybeGetMember("x").getFloat32(), 'g', precision));
      m_widgetsRotY[i]->setText(QString::number(euler.maybeGetMember("y").getFloat32(), 'g', precision));
      m_widgetsRotZ[i]->setText(QString::number(euler.maybeGetMember("z").getFloat32(), 'g', precision));
      m_widgetsSclX[i]->setText(QString::number(scl.maybeGetMember("x").getFloat32(), 'g', precision));
      m_widgetsSclY[i]->setText(QString::number(scl.maybeGetMember("y").getFloat32(), 'g', precision));
      m_widgetsSclZ[i]->setText(QString::number(scl.maybeGetMember("z").getFloat32(), 'g', precision));
    }

  );
}

void XfoAEWidget::uiChanged()
{
  FABRIC_TRY("XfoAEWidget::uiChanged", 

    FabricCore::RTVal values = getValueArray();
    m_param.callMethod("", "setValueArray", 1, &values);

  );

  AEWidget::uiChanged();
}
