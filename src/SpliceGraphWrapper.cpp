#include "SpliceGraphWrapper.h"
#include "macros.h"

#define FABRIC_WARN(msg) { const char *file=__FILE__; const int line=__LINE__;                                    \
	std::cerr << "Fabric WARNING: (file " << file << ", line " << line << "): " << msg << std::endl;  }           \

using namespace FabricSplice;

SpliceGraphWrapper::SpliceGraphWrapper(const std::string & path) :
	m_path(path) , m_operatorIsValid(false)
{ 
  FABRIC_TRY("SpliceGraphWrapper::SpliceGraphWrapper", 

    m_dgGraph = DGGraph(path.c_str());
    m_isTimeDependent = -1;

  );

  if (m_path != "")
  {
    reload();
  }

}

std::string SpliceGraphWrapper::name()
{
	return m_path;
}

void SpliceGraphWrapper::transferParamsToSplice(FabricCore::RTVal params)
{
	m_dgGraph.getDGPort("params").setRTVal(params);
}

FabricSplice::DGGraph SpliceGraphWrapper::getGraph()
{
  return m_dgGraph;
}

bool SpliceGraphWrapper::reload()
{
  FABRIC_TRY_RETURN("SpliceGraphWrapper::reload", false,

		// build the operator
		m_dgGraph.clear();
    m_dgGraph.loadFromFile(m_path.c_str());

  );

	m_operatorIsValid = true;
  m_isTimeDependent = -1;

	return evaluate();
}

bool SpliceGraphWrapper::isValid() const
{
	return m_operatorIsValid;
}

void SpliceGraphWrapper::setPath(const std::string & path)
{
	m_path = path;
	if (m_path != "")
	{
		reload();
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


