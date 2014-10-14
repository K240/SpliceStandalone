#include "MainWindow.h"
#include "SpliceStandalone.h"

#include <string>

#include <FabricSplice.h>
#include <FabricCore.h>

#include "macros.h"

using namespace FabricSplice;

SpliceStandalone * gApplication = NULL;

void appLogFunc(const char * message, unsigned int length)
{
  printf("%s\n", message);
  if(gApplication)
    gApplication->displayMessage(message);
}

void appLogErrorFunc(const char * message, unsigned int length)
{
  printf("%s\n", message);
  if(gApplication)
    gApplication->displayMessage(message);
}

void appKLReportFunc(const char * message, unsigned int length)
{
  printf("%s\n", message);
  if(gApplication)
    gApplication->displayMessage(message);
}

void appCompilerErrorFunc(unsigned int row, unsigned int col, const char * file, const char * level, const char * desc)
{
  printf("%d, %d, %s: %s\n", row, col, file, desc);
  
  char buf[128];
#ifdef _WIN32
  itoa(row, buf, 10);
#else
  snprintf(buf, 128, "%d", row);
#endif

  std::string stringMessage;
  stringMessage += "[KL Compiler ";
  stringMessage += level;
  stringMessage += "]: line: ";
  stringMessage += buf;
  stringMessage += " op: ";
  stringMessage += file;
  stringMessage += " : ";
  stringMessage += desc;

  if(gApplication)
    gApplication->displayMessage(stringMessage);
}

void appKLStatusFunc(const char * topic, unsigned int topicLength,  const char * message, unsigned int messageLength)
{
  std::string stringMessage;
  stringMessage += "[KL Status]: ";
  stringMessage += message;

  if(gApplication)
    gApplication->displayMessage(stringMessage);
}

SpliceStandalone * SpliceStandalone::getInstance()
{
  return gApplication;
}

SpliceStandalone::SpliceStandalone(int &argc, char **argv, boost::filesystem::path fabricDir, std::string spliceFilePath) 
  : QApplication(argc, argv)
{

  gApplication = this;

  m_mainWindow = NULL;
  m_fabricPath = fabricDir;

  QPixmap pixmap((m_fabricPath / "Resources" / "splice_splash.jpg").string().c_str());
  m_splashScreen = new QSplashScreen(pixmap);
  m_splashScreen->show();
  
  Initialize(); 

  constructFabricClient();

  Logging::setLogFunc(appLogFunc);
  Logging::setLogErrorFunc(appLogErrorFunc);
  Logging::setKLReportFunc(appKLReportFunc);

  Logging::setCompilerErrorFunc(appCompilerErrorFunc);
  Logging::setKLStatusFunc(appKLStatusFunc);

  if(spliceFilePath.length())
    addWrapper(spliceFilePath);
}

SpliceStandalone::~SpliceStandalone()
{
  gApplication = NULL;
  DestroyClient();
  Finalize();
}


void SpliceStandalone::displayMessage(std::string message)
{
  if (m_mainWindow)
  {
    m_mainWindow->displayMessage(message+"\n");
    processEvents();
  }
}

SpliceGraphWrapper::Ptr SpliceStandalone::addWrapper(const std::string & splicePath)
{
  if(m_mainWindow)
    m_mainWindow->setGlViewEnabled(false);

  if(m_splashScreen)
  {
    m_splashScreen->finish(m_mainWindow);
    m_splashScreen = NULL;
  }

  QPixmap pixmap((m_fabricPath / "Resources" / "splice_loading.jpg").string().c_str());
  m_splashScreen = new QSplashScreen(pixmap);
  m_splashScreen->show();

  SpliceGraphWrapper::Ptr wrapper = SpliceGraphWrapper::Ptr(new SpliceGraphWrapper(splicePath));
  m_wrappers.push_back(wrapper);

  // setup the evaluation context
  FabricCore::RTVal context = wrapper->getGraph().getEvalContext();
  context.setMember("host", FabricSplice::constructStringRTVal("Splice Standalone"));
  context.setMember("graph", FabricSplice::constructStringRTVal(splicePath.c_str()));

  if(m_mainWindow)
    m_mainWindow->updateViews();

  if(m_splashScreen)
  {
    m_splashScreen->finish(m_mainWindow);
    m_splashScreen = NULL;
  }

  if(m_mainWindow)
  {
    m_mainWindow->setGlViewEnabled(true);
    m_mainWindow->redraw();
  }
  
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

  if(m_splashScreen)
  {
    m_splashScreen->finish(m_mainWindow);
    m_splashScreen = NULL;
  }
}

MainWindow * SpliceStandalone::getMainWindow()
{
  return m_mainWindow;
}

void SpliceStandalone::constructFabricClient()
{
  FABRIC_TRY("SpliceStandalone::constructFabricClient",

    FabricCore::Client client = ConstructClient();

    client.loadExtension("Animation", "", false);
    client.loadExtension("InlineDrawing", "", false);
    client.loadExtension("SpliceStandalone", "", false);
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
    m_wrappers[i]->reload();
}

void SpliceStandalone::needRedraw()
{
  m_mainWindow->redraw();
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

