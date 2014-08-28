#include "MainWindow.h"
#include "SpliceStandalone.h"

#include <sstream>
#include <string>

#include <FabricSplice.h>
#include <FabricCore.h>

#include "macros.h"

using namespace FabricSplice;

void appLogFunc(const char * message, unsigned int length)
{
	printf("%s\n", message);
	SpliceStandalone * app = static_cast<SpliceStandalone *>(QApplication::instance());
	app->displayMessage(message);
}

void appLogErrorFunc(const char * message, unsigned int length)
{
	printf("%s\n", message);
	SpliceStandalone * app = static_cast<SpliceStandalone *>(QApplication::instance());
	app->displayMessage(message);
}

void appKLReportFunc(const char * message, unsigned int length)
{
	printf("%s\n", message);
	SpliceStandalone * app = static_cast<SpliceStandalone *>(QApplication::instance());
	app->displayMessage(message);
}

void appCompilerErrorFunc(unsigned int row, unsigned int col, const char * file, const char * level, const char * desc)
{
	printf("%d, %d, %s: %s\n", row, col, file, desc);
	SpliceStandalone * app = static_cast<SpliceStandalone *>(QApplication::instance());

	std::ostringstream s;
	s << "[KL Compiler " << level << "]: line:" << row << " op: "<< file << ": "<< desc;
	const std::string message(s.str());

	app->displayMessage(message);
}

void appKLStatusFunc(const char * topic, unsigned int topicLength,  const char * message, unsigned int messageLength)
{
	SpliceStandalone * app = static_cast<SpliceStandalone *>(QApplication::instance());

	std::ostringstream s;
	s << "[KL Status]: " << message ;
	const std::string stringMessage(s.str());

	app->displayMessage(stringMessage);
}


SpliceStandalone::SpliceStandalone(int &argc, char **argv) 
	: QApplication(argc, argv)
{

  m_mainWindow = NULL;

  boost::filesystem::path executablePath = argv[0];
  m_appPath = executablePath.parent_path();

  QPixmap pixmap((m_appPath / "images" / "splash.jpg").string().c_str());
  m_splashScreen = new QSplashScreen(pixmap);
  m_splashScreen->show();

  boost::filesystem::path extsDir = m_appPath / "Exts";
  addExtFolder(extsDir.string().c_str());
	
  Initialize(); 

  constructFabricClient();

	Logging::setLogFunc(appLogFunc);
	Logging::setLogErrorFunc(appLogErrorFunc);
	Logging::setKLReportFunc(appKLReportFunc);

	Logging::setCompilerErrorFunc(appCompilerErrorFunc);
	Logging::setKLStatusFunc(appKLStatusFunc);
}

SpliceStandalone::~SpliceStandalone()
{
	DestroyClient();
	Finalize();
}


void SpliceStandalone::displayMessage(std::string message)
{
	if (m_mainWindow)
		m_mainWindow->displayMessage(message+"\n");
}

SpliceGraphWrapper::Ptr SpliceStandalone::addWrapper(const std::string & klPath)
{
	SpliceGraphWrapper::Ptr wrapper = SpliceGraphWrapper::Ptr(new SpliceGraphWrapper(klPath));
	m_wrappers.push_back(wrapper);
	
	return wrapper;

}

const std::vector<SpliceGraphWrapper::Ptr> & SpliceStandalone::wrappers()
{
	return m_wrappers;
}

QFont SpliceStandalone::getWidgetFont()
{
  return QFont("Sans Cherif", 8 );
}

// this will make sure the main window is created and then raise it
void SpliceStandalone::showMainWindow()
{
	Qt::WindowFlags flags = 0;
	m_mainWindow = new MainWindow(0,flags);

	m_mainWindow->resize(1600,1000);


  m_mainWindow->showMaximized();
  m_mainWindow->raise();

  m_splashScreen->finish(m_mainWindow);
}

void SpliceStandalone::constructFabricClient()
{
  FABRIC_TRY("SpliceStandalone::constructFabricClient",

    FabricCore::Client client = ConstructClient();

    if(!isLicenseValid())
    {
      quit();
      return;
    }

    client.loadExtension("SpliceStandalone");
    client.loadExtension("UIParameters");
    client.loadExtension("InlineDrawing");
    client.loadExtension("SceneLib");
    client.loadExtension("Animation");

  );
}


// this will make sure the main window is created and then raise it
void SpliceStandalone::clearAll()
{
	// we also probably need to make sure the inline shape register by those nodes are removed 
  if(m_wrappers.size() == 0)
    return;
  
  m_wrappers.clear();
  DestroyClient();
  constructFabricClient();
}

// this will make sure the main window is created and then raise it
void SpliceStandalone::reloadAll()
{
	for (unsigned int i=0; i < m_wrappers.size(); i++ )
		m_wrappers[i]->reloadCode();
}

void SpliceStandalone::needRedraw()
{
	m_mainWindow->redraw();
}

void SpliceStandalone::updateParams()
{
	m_mainWindow->updateParams();
}

void SpliceStandalone::setupFusionLook()
{
	qApp->setStyle(QStyleFactory::create("Fusion"));
	 
	QPalette darkPalette;
	darkPalette.setColor(QPalette::Window, QColor(53,53,53));
	darkPalette.setColor(QPalette::WindowText, Qt::white);
	darkPalette.setColor(QPalette::Base, QColor(25,25,25));
	darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
	darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
	darkPalette.setColor(QPalette::ToolTipText, Qt::white);
	darkPalette.setColor(QPalette::Text, Qt::white);
	darkPalette.setColor(QPalette::Button, QColor(53,53,53));
	darkPalette.setColor(QPalette::ButtonText, Qt::white);
	darkPalette.setColor(QPalette::BrightText, Qt::red);
	darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
	 
	darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
	darkPalette.setColor(QPalette::HighlightedText, Qt::black);
	     
	qApp->setPalette(darkPalette);
	 
	qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

}

