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

    std::vector<FabricCore::RTVal> args(3);
    args[0] = constructStringRTVal("NodeData");
    args[1] = constructStringRTVal("Empty NodeData");
    args[2] = constructStringRTVal("empty nodeData");
  	m_fullTable = constructObjectRTVal("GroupParameter", args.size(), &args[0]);

  	m_name = "Empty NodeData";
  	m_type = "emptyNodeData";
  	m_description = "Empty Node Data.";

    args[0] = constructStringRTVal("dirtyArgs");
    args[1] = constructStringRTVal("Dirty Args");
    args[2] = constructStringRTVal("dirty input arguments");
    m_dirtyArgs = constructObjectRTVal("GroupParameter", args.size(), &args[0]);

    args[0] = constructStringRTVal("args");
    args[1] = constructStringRTVal("Args");
    args[2] = constructStringRTVal("input arguments");
    m_args = constructObjectRTVal("GroupParameter", args.size(), &args[0]);
  );
	setDefaultValues(m_fullTable);
}

NodeData::NodeData( FabricCore::RTVal nodeTable )
{
  FABRIC_TRY("NodeData::NodeData",

  	m_fullTable		= nodeTable;
  	m_name 			= nodeTable.callMethod("String", "getLabel", 0, 0).getStringCString();
  	m_type 			= nodeTable.callMethod("String", "getType", 0, 0).getStringCString();
  	m_description 	= nodeTable.callMethod("String", "getDescription", 0, 0).getStringCString();

    m_name = "Empty NodeData";
    m_type = "emptyNodeData";
    m_description = "Empty Node Data.";

    std::vector<FabricCore::RTVal> args(3);
    args[0] = constructStringRTVal("dirtyArgs");
    args[1] = constructStringRTVal("Dirty Args");
    args[2] = constructStringRTVal("dirty input arguments");
    m_dirtyArgs = constructObjectRTVal("GroupParameter", args.size(), &args[0]);

    args[0] = constructStringRTVal("args");
    args[1] = constructStringRTVal("Args");
    args[2] = constructStringRTVal("input arguments");
    m_args = constructObjectRTVal("GroupParameter", args.size(), &args[0]);
  );

	setDefaultValues(m_fullTable);
}

NodeData::~NodeData()
{
}

void NodeData::setDefaultValues(FabricCore::RTVal newArgs, bool clearArgs)
{
  FABRIC_TRY("NodeData::setDefaultValues", 
  
  	if(clearArgs)
  	{
  		//reset data
      m_fullTable   = newArgs;
      m_name      = newArgs.callMethod("String", "getLabel", 0, 0).getStringCString();
      m_type      = newArgs.callMethod("String", "getType", 0, 0).getStringCString();
      m_description   = newArgs.callMethod("String", "getDescription", 0, 0).getStringCString();
  		m_args.callMethod("", "clear", 0, 0);
  	}

  	//create our default flatten args map.
    FabricCore::RTVal children = newArgs.callMethod("Parameter[]", "getChildren", 0, 0);
  	for(size_t i = 0; i < children.getArraySize(); ++i)
  	{
      FabricCore::RTVal child = children.getArrayElement(i);
      if(std::string(child.callMethod("String", "type", 0, 0).getStringCString()) != "GroupParameter")
  		{
  			m_args.callMethod("", "addChild", 1, &child);
  		}
  		else
  		{
        FabricCore::RTVal grpChild = constructObjectRTVal("GroupParameter", 1, &child);
  			setDefaultValues(grpChild, false);
  		}
  	}

  );
}

bool NodeData::invalidateArg(const std::string & name )
{
  FABRIC_TRY_RETURN("NodeData::invalidateArg", false, 

    FabricCore::RTVal nameVal = constructStringRTVal(name.c_str());
  	FabricCore::RTVal param = m_dirtyArgs.callMethod("Parameter", "getChildByName", 1, &nameVal);
  	if ( !param.isNullObject() )
  		return false;
    FabricCore::RTVal invalidParam = m_args.callMethod("Parameter", "getChildByName", 1, &nameVal);
    m_dirtyArgs.callMethod("", "addChild", 1, &invalidParam);
  );
	return true;
}

bool NodeData::cleanArg(const std::string & name )
{
  FABRIC_TRY_RETURN("NodeData::cleanArg", false, 

    FabricCore::RTVal nameVal = constructStringRTVal(name.c_str());
    FabricCore::RTVal param = m_dirtyArgs.callMethod("Parameter", "getChildByName", 1, &nameVal);
    if ( param.isNullObject() )
      return false;
    m_dirtyArgs.callMethod("", "deleteChildByName", 1, &nameVal);

  );
	return true;
	
}

void NodeData::cleanAllArgs()
{
  FABRIC_TRY("NodeData::cleanAllArgs", 

    m_dirtyArgs.callMethod("", "clear", 0, 0);

  );
}


			
 
