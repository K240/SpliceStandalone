#include <fstream>
#include <string>

#include <QtGui/QLabel>
#include <QtGui/QFileDialog>
#include <QtGui/QShortcut>
#include <QtGui/QKeySequence>

#include <QtCore/QDir>

#include "KLEditor.h"
#include "SpliceStandalone.h"

using namespace FabricSplice;

KLEditor::KLEditor(QWidget* pParent)
: QWidget(pParent)
{
	QVBoxLayout* layout = new QVBoxLayout();
	this->setLayout(layout);

	// Add a KL Editor for the file
	QWidget * editorWidget = new QWidget(this);
	layout->addWidget(editorWidget);

	editorWidget->setLayout(new QVBoxLayout());
	editorWidget->layout()->setContentsMargins(2, 2, 2, 2);

	//QLabel *label = new QLabel("KL SourceCode", editorWidget);
	//editorWidget->layout()->addWidget(label);

	int fontSize = 10;
	m_sourceCodeWidget = new KLSourceCodeWidget(editorWidget, fontSize);
	editorWidget->layout()->addWidget(m_sourceCodeWidget);

	QWidget * klButtons = new QWidget(editorWidget);
	editorWidget->layout()->addWidget(klButtons);

	klButtons->setLayout(new QHBoxLayout());
	klButtons->layout()->setContentsMargins(0, 0, 0, 0);

	QPushButton * compileButton = new QPushButton("Compile", klButtons);
	klButtons->layout()->addWidget(compileButton);
  connect(compileButton, SIGNAL(clicked()), this, SLOT(compilePressed()));

	QPushButton * updateParams = new QPushButton("UpdateParams", klButtons);
	klButtons->layout()->addWidget(updateParams);
  connect(updateParams, SIGNAL(clicked()), this, SLOT(updateParamsPressed()));

	// QPushButton* saveButton = new QPushButton("Export", klButtons);
	// klButtons->layout()->addWidget(saveButton);

	// setup a shortCut for compile
	new QShortcut(QKeySequence(Qt::ALT + Qt::Key_R), this, SLOT(compilePressed()));

}

void KLEditor::clear()
{
  m_sourceCodeWidget->setSourceCode("", "");
}

void KLEditor::setWrapper(SpliceGraphWrapper::Ptr wrapper)
{
	m_editorWrapper = wrapper;

  if(m_editorWrapper)
  {
  	std::ifstream t(m_editorWrapper->klPath().c_str());
	  std::string wrapperCode((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
  	m_sourceCodeWidget->setSourceCode("render", wrapperCode);
  }
}

void KLEditor::saveEditorCodeToDisk(std::string path)
{
    std::string code = m_sourceCodeWidget->getSourceCode();

    FILE * file = fopen(path.c_str(), "wb");
    fwrite(code.c_str(), code.length(), 1, file);
    fclose(file);
}

void KLEditor::compilePressed()
{
	std::string currentPath = m_editorWrapper->klPath();
	saveEditorCodeToDisk(currentPath);
	m_editorWrapper->reloadCode();
	
	SpliceStandalone * app = static_cast<SpliceStandalone *>(QApplication::instance());
	app->needRedraw();

}

void KLEditor::updateParamsPressed()
{
	std::string currentPath = m_editorWrapper->klPath();
	saveEditorCodeToDisk(currentPath);
  m_editorWrapper->reloadCode();
	m_editorWrapper->reloadParams();
	
	SpliceStandalone * app = static_cast<SpliceStandalone *>(QApplication::instance());
	app->updateParams();
}


// void KLEditor::savePressed()
// {
// 	QString klFileName = QFileDialog::getSaveFileName( 
// 		this, 
// 		tr("Save KL file"), 
// 		QDir::currentPath(), 
// 		tr("KL files (*.kl);;All files (*.*)")
// 	);

// 	if( !klFileName.isNull() )
// 	{
// 		std::string path = klFileName.toUtf8().constData();
// 		saveEditorCodeToDisk(path);
// 	}  
// }

