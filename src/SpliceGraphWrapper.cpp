#include "SpliceGraphWrapper.h"
#include "macros.h"

#define FABRIC_WARN(msg) { const char *file=__FILE__; const int line=__LINE__;                                    \
	std::cerr << "Fabric WARNING: (file " << file << ", line " << line << "): " << msg << std::endl;  }           \

using namespace FabricSplice;

SpliceGraphWrapper::SpliceGraphWrapper(const std::string & klCodePath) :
	m_klScriptPath(klCodePath) , m_operatorIsValid(false)
{ 
  FABRIC_TRY("SpliceGraphWrapper::SpliceGraphWrapper", 

    m_dgGraph = DGGraph(klCodePath.c_str());
    std::string opName = "wrapperOp";

    // Create a Fabric DGNode
    m_dgGraph.constructDGNode("SpliceGraphWrapper");
    m_dgNode = m_dgGraph.getDGNode("SpliceGraphWrapper");
    
    m_dgGraph.addDGNodeMember("params", "GroupParameter", FabricCore::Variant(), "SpliceGraphWrapper", "");
    m_dgGraph.addDGPort("params", "params", Port_Mode_IO, "SpliceGraphWrapper", true);

    m_dgGraph.constructKLOperator(opName.c_str(), "", opName.c_str(), "SpliceGraphWrapper");

    m_isTimeDependent = -1;

  );

  if (m_klScriptPath != "")
  {
    reloadCode();
  }

}

std::string SpliceGraphWrapper::name()
{
	return m_klScriptPath;
}

void SpliceGraphWrapper::transferParamsToSplice(FabricCore::RTVal params)
{
	m_dgGraph.getDGPort("params").setRTVal(params);
}

FabricCore::RTVal SpliceGraphWrapper::getParameters()
{
	return m_dgGraph.getDGPort("params").getRTVal();
}

bool SpliceGraphWrapper::reloadParams()
{

  FABRIC_TRY_RETURN("SpliceGraphWrapper::reloadParams", false, 
  
	  //we clear the paramters , they will be reloaded next time the scripts run
  	FabricCore::RTVal params = getParameters();
    params.callMethod("", "clear", 0, 0);
  	m_dgGraph.getDGPort("params").setRTVal(params);
    m_dgGraph.evaluate();

    m_isTimeDependent = -1;

  );
	
	return true;
}


bool SpliceGraphWrapper::reloadCode()
{
	std::string opName = "wrapperOp";
	std::cout << "Loading KL from " << m_klScriptPath << std::endl;
	std::ifstream t(m_klScriptPath.c_str());
	std::string opCode((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	// Inject the entry operator code template: 
	opCode += "operator " + opName + "(io GroupParameter params)";
	opCode += "{";
	opCode += "  if(params.getChildren().size() == 0) {";
	opCode += "    params = getParams();";
	opCode += "  }";
  opCode += "  compute(params);";
	opCode += "}";

  FABRIC_TRY_RETURN("SpliceGraphWrapper::reloadCode", false,

		// build the operator
		m_dgGraph.setKLOperatorSourceCode(opName.c_str(), opCode.c_str(), opName.c_str());

  );

	m_operatorIsValid = true;
  m_isTimeDependent = -1;

	return evaluate();
}

bool SpliceGraphWrapper::isValid() const
{
	return m_operatorIsValid;
}

void SpliceGraphWrapper::setKlPath(const std::string & path)
{
	m_klScriptPath = path;
	if (m_klScriptPath != "")
	{
		reloadCode();
	}
}

bool SpliceGraphWrapper::evaluate(bool force)
{
	// Execute the KL operator to generate a dkScene and return an RTVal to it
  FABRIC_TRY_RETURN("SpliceGraphWrapper::evaluate", false,

		if (!isValid())
		{
			std::cout << "wrapperOp operator cannot be evaluated ..." << std::endl;
			return false;
		}

    if(force)
    {
      DGPort paramsPort = m_dgGraph.getDGPort("params");
      paramsPort.setRTVal(paramsPort.getRTVal());
    }

		m_dgGraph.evaluate();

  );

	return true;
}

bool SpliceGraphWrapper::setFrame(int frame) 
{
  if(m_isTimeDependent == -1) {
    FABRIC_TRY_RETURN("SpliceGraphWrapper::setFrame", false,

      FabricCore::RTVal params = m_dgGraph.getDGPort("params").getRTVal();
      FabricCore::RTVal nameVal = constructStringRTVal("frame");
      m_isTimeDependent = params.callMethod("Boolean", "hasChild", 1, &nameVal).getBoolean() ? 1 : 0;

    );
  }
  if(m_isTimeDependent == 0)
    return false;

  DGPort paramsPort = m_dgGraph.getDGPort("params");
  FabricCore::RTVal groupParam = paramsPort.getRTVal();

  FABRIC_TRY_RETURN("SpliceGraphWrapper::setFrame", false,

    FabricCore::RTVal nameVal = constructStringRTVal("frame");
    if(!groupParam.callMethod("Boolean", "hasChild", 1, &nameVal).getBoolean())
      return false;
    FabricCore::RTVal frameParam = groupParam.callMethod("Float32Parameter", "getChildByName", 1, &nameVal);
    FabricCore::RTVal frameVal = constructSInt32RTVal(frame);
    frameParam.callMethod("", "setValue", 1, &frameVal);
    paramsPort.setRTVal(groupParam);

  );
  return evaluate();
}


