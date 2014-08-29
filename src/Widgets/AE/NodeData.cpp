#include "NodeData.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QPainter>
#include <QtGui/QMenu>
#include <QtGui/QScrollBar>
#include <QtGui/QDialog>
#include <QtGui/QPushButton>

#include <FabricSplice.h>
#include <vector>

#include "macros.h"

using namespace FabricSplice;

NodeData::NodeData()
{
  FABRIC_TRY("NodeData::NodeData",

  	m_fullTable = DGPortList();

    m_graph = FabricSplice::DGGraph();
  	m_name = "Empty NodeData";

    m_dirtyArgs = DGPortList();
    m_args = DGPortList();
  );
	setDefaultValues(m_graph);
}

NodeData::NodeData( FabricSplice::DGGraph graph )
{
  FABRIC_TRY("NodeData::NodeData",

    m_graph = graph;
  	m_name 			= graph.getName();
    m_fullTable   = DGPortList();
    m_dirtyArgs = DGPortList();
    m_args = DGPortList();

  );

	setDefaultValues(graph, true);
}

NodeData::~NodeData()
{
}

void NodeData::setDefaultValues(FabricSplice::DGGraph graph, bool clearArgs)
{
  FABRIC_TRY("NodeData::setDefaultValues", 
  
  	if(clearArgs)
  	{
      m_graph = graph;
      m_name      = graph.getName();
      m_fullTable   = DGPortList();

      for(unsigned int i=0;i<graph.getDGPortCount();i++)
      {
        FabricSplice::DGPort port = graph.getDGPort(i);
        if(std::string(port.getName()) == "time")
          continue;
        m_fullTable.push_back(port);
      }

      m_dirtyArgs = DGPortList();
  	}

  	//create our default flatten args map.
    m_args = DGPortList();
    for(size_t i=0;i<m_fullTable.size();i++)
    {
      m_args.push_back(m_fullTable[i]);
    }

  );
}

bool NodeData::invalidateArg(const std::string & name )
{
  FABRIC_TRY_RETURN("NodeData::invalidateArg", false, 

    for(size_t i=0;i<m_dirtyArgs.size();i++)
    {
      if(m_dirtyArgs[i].getName() == name)
        return false;
    }

    for(size_t i=0;i<m_args.size();i++)
    {
      if(m_args[i].getName() == name)
      {
        m_dirtyArgs.push_back(m_args[i]);
        return true;
      }
    }
  );
	return false;
}

bool NodeData::cleanArg(const std::string & name )
{
  FABRIC_TRY_RETURN("NodeData::cleanArg", false, 

    bool found = false;
    DGPortList newDirtyArgs;
    for(size_t i=0;i<m_dirtyArgs.size();i++)
    {
      if(m_dirtyArgs[i].getName() == name)
      {
        found = true;
      }
      else
      {
        newDirtyArgs.push_back(m_dirtyArgs[i]);
      }
    }

    if(!found)
      return false;
    m_dirtyArgs = newDirtyArgs;
  );
	return true;
	
}

void NodeData::cleanAllArgs()
{
  FABRIC_TRY("NodeData::cleanAllArgs", 

    m_dirtyArgs.clear();

  );
}


			
 
