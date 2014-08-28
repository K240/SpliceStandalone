#include "FilePathAEWidget.h"

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
#include <QtGui/QFileDialog>

#include "macros.h"
#include "utility.h"
#include "SpliceStandalone.h"

using namespace FabricSplice;

AEWidget * FilePathAEWidget::create( FabricCore::RTVal param , QWidget* parent)
{
	return new FilePathAEWidget(param ,parent);
}

FilePathAEWidget::FilePathAEWidget(FabricCore::RTVal param ,QWidget* parent)
    : AEWidget(param,parent)
{
	setLayout(createLabelControlLayout());

  m_listWidget = new QWidget();
  m_listWidget->setLayout(new QGridLayout(m_listWidget));
	layout()->addWidget(m_listWidget);

  setRTVal(param);
			
	// QSpacerItem * spacerItem = new QSpacerItem(20,1,QSizePolicy::Expanding , QSizePolicy::Minimum);
	// list->addItem(spacerItem);

}

FabricCore::RTVal FilePathAEWidget::getValueArray()
{
  FabricCore::RTVal values;

  FABRIC_TRY_RETURN("FilePathAEWidget::getValueArray", FabricCore::RTVal(), 

  	values = constructRTVal("FilePath[]");
    values.setArraySize(m_widgets.size());
  	for (unsigned int i = 0; i < m_widgets.size(); ++i)
    {
      std::string path = stdStringFromQString(m_widgets[i]->text());
      FabricCore::RTVal pathVal = constructStringRTVal(path.c_str());
      values.setArrayElement(i, constructObjectRTVal("FilePath", 1, &pathVal));
    }
  );
	return values;
}

void FilePathAEWidget::setRTVal(FabricCore::RTVal param)
{
  FABRIC_TRY("FilePathAEWidget::setRTVal",
  
    AEWidget::setRTVal(constructObjectRTVal("FilePathParameter", 1, &param));
    setValueArray(m_param.callMethod("FilePath[]", "getValueArray", 0, 0));

  );
}

void FilePathAEWidget::setValueArray(FabricCore::RTVal values)
{
  FABRIC_TRY("FilePathAEWidget::setValueArray", 

    // clear the layout
    if(values.getArraySize() != m_widgets.size())
    {
      QLayoutItem* item;
      while ( ( item = m_listWidget->layout()->takeAt( 0 ) ) != NULL )
      {
        delete item->widget();
        delete item;
      }

      QGridLayout * layout = (QGridLayout *)m_listWidget->layout();

      m_widgets.resize(values.getArraySize());
      m_buttons.resize(values.getArraySize());
      for (unsigned int i = 0; i < values.getArraySize(); ++i)
      {
        m_widgets[i] = new ItemWidget(m_listWidget);
        m_widgets[i]->setFont( getApplicationWidgetFont() );

        m_buttons[i] = new QPushButton("...", m_listWidget);
        m_buttons[i]->setFont( getApplicationWidgetFont() );
        m_buttons[i]->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        connect( m_buttons[i], SIGNAL( clicked() ), this , SLOT( browseClicked() ) );
    
        // connect the color box to uiChanged()
        connect( m_widgets[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        layout->addWidget( m_widgets[i], i, 0);
        layout->addWidget( m_buttons[i], i, 1);
      }
    }

    for (unsigned int i = 0; i < values.getArraySize(); ++i)
    {
      FabricCore::RTVal pathObj = values.getArrayElement(i);
      FabricCore::RTVal pathStr = pathObj.callMethod("String", "string", 0, 0);
      m_widgets[i]->setText(pathStr.getStringCString());
    }

  );
}

void FilePathAEWidget::uiChanged()
{
  FABRIC_TRY("FilePathAEWidget::uiChanged", 

    FabricCore::RTVal values = getValueArray();
    m_param.callMethod("", "setValueArray", 1, &values);

  );

  AEWidget::uiChanged();
}

void FilePathAEWidget::browseClicked()
{
  QObject * sender = QObject::sender();
  for(size_t i=0;i<m_buttons.size();i++)
  {
    if(m_buttons[i] != sender)
      continue;

    boost::filesystem::path path = stdStringFromQString(m_widgets[i]->text());
    std::string directory;
    std::string filter;
    if(!path.has_extension())
    {
      directory = path.string();
      filter = "All files (*.*)";
    }
    else
    {
      directory = path.parent_path().string();
      std::string ext = path.extension().string().substr(1, 1000);
      filter = ext + " files (*." + ext + ");;All files (*.*)";
    }

    std::string name;
    std::string mode;
    FABRIC_TRY("FilePathAEWidget::browseClicked", 
      name = m_param.callMethod("String", "getLabel", 0, 0).getStringCString();
      mode = m_param.callMethod("String", "getMode", 0, 0).getStringCString();
    );

    QString filePath;

    if(mode == "in")
      filePath = QFileDialog::getOpenFileName(this, name.c_str(), directory.c_str(), filter.c_str());
    else
      filePath = QFileDialog::getSaveFileName(this, name.c_str(), directory.c_str(), filter.c_str());
    if(filePath.isEmpty())
      return;

    m_widgets[i]->setText(filePath);
    break;
  }
}
