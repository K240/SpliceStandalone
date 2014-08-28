#ifndef SpliceGraphWrapper_H
#define SpliceGraphWrapper_H

#include <iostream>
#include <sstream>
#include <istream>
#include <ostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <FabricSplice.h>
#include <FabricCore.h>

namespace FabricSplice
{
  class SpliceGraphWrapper 
  {
  public:
  	typedef boost::shared_ptr<SpliceGraphWrapper> Ptr;

  	SpliceGraphWrapper(const std::string & klCodePath = "");
  	~SpliceGraphWrapper() {};

  	void setKlPath(const std::string & path);
  	std::string klPath() { return m_klScriptPath; }

  	// From SceneLib::Scene interface. Re-run the KL script. Returns false if the evalution failed (due to a KL compile error or runtime failure).
  	bool reloadCode();

  	// the parameters in the code changes they need to be reloaded
  	bool reloadParams();

    // returns true if this node uses the frame param
    bool isTimeDependent();

  	// return the name of the nodes , for now we use the name of the kl File
  	std::string name();
  	
    // returns the GroupParameter
  	FabricCore::RTVal getParameters();

  	// set the Parameter to the Port node
  	void transferParamsToSplice(FabricCore::RTVal params);

  	// Run the KL script to populate the scene. Returns false if the evalution failed (due to a KL compile error or runtime failure).
  	bool evaluate(bool force = false);

    // update an optional "frame" parameter
    bool setFrame(int frame);
  	
  private:
  	// @todo: what is the user supposed to do if this is not true? What does "valid" mean in this context?
  	bool isValid() const;

  private:

  	FabricCore::Client m_client;

  	std::string m_klScriptPath;
  	bool m_operatorIsValid;
    int m_isTimeDependent;

  	DGGraph m_dgGraph;
  	FabricCore::DGNode m_dgNode;
  	DGPort m_dgPort;

  };
};

#endif // SpliceGraphWrapper_H

