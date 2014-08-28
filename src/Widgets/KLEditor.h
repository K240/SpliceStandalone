#ifndef __KLEDITOR_H__
#define __KLEDITOR_H__

#include <QtGui/QWidget>
#include <QtGui/QBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QGroupBox>
#include <QtGui/QTabWidget>

#include "SpliceGraphWrapper.h"
#include "KLSourceCodeWidget.h"

namespace FabricSplice
{

  class KLEditor : public QWidget
  {
  	Q_OBJECT

  public:
  	KLEditor(QWidget* pParent = NULL);
  	
    void clear();
  	void setWrapper(SpliceGraphWrapper::Ptr wrapper);

  public slots:
    void compilePressed();

  private:

    void saveEditorCodeToDisk(std::string path);

  	KLSourceCodeWidget * m_sourceCodeWidget;
  	SpliceGraphWrapper::Ptr m_editorWrapper;	

  };
};

#endif
