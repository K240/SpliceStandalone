#ifndef __NODEDATA_H__
#define __NODEDATA_H__
#define __NODEDATA_FWD__

#include <map>
#include <set>
#include <boost/shared_ptr.hpp>

#include <FabricCore.h>

namespace FabricSplice
{
	class NodeData;
	typedef boost::shared_ptr<NodeData> NodeDataPtr;

	/// \brief Represent the Data of a "Node" , currently mostly a GroupParameter object of Data

	class NodeData
	{
		public:
			typedef std::map<std::string, bool> DirtyArgsMap;
			//! @name Constructors / Destructors
			//@{
			NodeData();
			NodeData(FabricCore::RTVal nodeTable);
			~NodeData();
			//@}

			//! @name Setters
			//@{
			/// build up the default args table 
			void setDefaultValues(FabricCore::RTVal newParams, bool clearArgs = true);

			/// replace the internal args data with a new GroupParameter \n
			/// note that is does not do any check on the validity of the table
			/// for example that it match the input table 
			void	setArgs(FabricCore::RTVal newArgs) { m_args = newArgs;}
			//@}
			
			//! @name Getters
			//@{
			/// return the internal full VariantMap Table
			FabricCore::RTVal	getTable() { return m_fullTable; };

			/// get internal args table \n
			/// the map is not copied so you should not change its value
			FabricCore::RTVal	getArgs() { return m_args; };
			/// return the table of argument that are dirty ( ie : out of sync )
			FabricCore::RTVal	getDirtyArgs() { return m_dirtyArgs; };
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
			/// name of the NodeData
			std::string		m_name;
			/// type generaly descibe the what kind of node this is 
			/// also this is not enforce and is mostly a hint string
			std::string		m_type;
			/// description is used to describe the purpose of this node
			/// could link to external docs 
			std::string		m_description;

			/// full args description
			FabricCore::RTVal	m_fullTable;
			
			/// the actual cached args
			FabricCore::RTVal m_args;

			/// the list if durtyArgs
			FabricCore::RTVal m_dirtyArgs;

	};
	
	
}


#endif
