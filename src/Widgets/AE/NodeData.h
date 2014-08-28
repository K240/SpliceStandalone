#ifndef __NODEDATA_H__
#define __NODEDATA_H__
#define __NODEDATA_FWD__

#include <map>
#include <set>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <FabricCore.h>
#include <FabricSplice.h>

namespace FabricSplice
{
	class NodeData;
	typedef boost::shared_ptr<NodeData> NodeDataPtr;

	/// \brief Represent the Data of a "Node" , currently mostly a GroupParameter object of Data

	class NodeData
	{
		public:
			typedef std::map<std::string, bool> DirtyArgsMap;
      typedef std::vector<FabricSplice::DGPort> DGPortList;

			//! @name Constructors / Destructors
			//@{
			NodeData();
			NodeData(FabricSplice::DGGraph graph);
			~NodeData();
			//@}

			//! @name Setters
			//@{
			/// build up the default args table 
			void setDefaultValues(FabricSplice::DGGraph graph, bool clearArgs = true);

			//! @name Getters
			//@{
			/// return the internal full VariantMap Table
			DGPortList getTable() { return m_fullTable; };

			/// get internal args table \n
			/// the map is not copied so you should not change its value
			DGPortList & getArgs() { return m_args; };
			/// return the table of argument that are dirty ( ie : out of sync )
			DGPortList & getDirtyArgs() { return m_dirtyArgs; };
			//@}
			
			//! @name Dirty or Clean
			//@{
			/// add the arg name in the list of dirty args return false if it was already dirty
			bool invalidateArg(const std::string & name );
			/// simply remove an argument from the list of dirty argument \n
			/// return true if something got clean , false otherwise
			bool cleanArg(const std::string & name );
			/// clean all the arguments 
			void cleanAllArgs();
			//@}
			
			
		protected:
      /// the splice graph
      FabricSplice::DGGraph m_graph;

			/// name of the NodeData
			std::string		m_name;

			/// full args description
			DGPortList m_fullTable;
			
			/// the actual cached args
			DGPortList m_args;

			/// the list if durtyArgs
			DGPortList m_dirtyArgs;

	};
	
	
}


#endif
