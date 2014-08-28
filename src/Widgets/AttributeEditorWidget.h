#ifndef __ATTRIBUTEEDITOREWIDGET_H__
#define __ATTRIBUTEEDITOREWIDGET_H__

#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QContextMenuEvent>

#include <map>
#include <set>
#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>

#include <boost/shared_ptr.hpp>
#include <FabricCore.h>

#include "SpliceGraphWrapper.h"

/// \brief Simple wrapper around a FabricCore::RTVal to expose it to Qt
struct QSpliceGraphWrapper
{
  FabricSplice::SpliceGraphWrapper::Ptr wrapper;
};

Q_DECLARE_METATYPE(QSpliceGraphWrapper)

namespace FabricSplice
{
#ifndef __NODEDATA_FWD__
#define __NODEDATA_FWD__

  class NodeData;
  typedef boost::shared_ptr<NodeData> NodeDataPtr;

#endif


	class AEWidget;	
	class AEWidgetFactory;
	class AETreeView;
	class AEModel;
	class AEFilterModel;
	
	/// \brief Top level Widget for the Attribute Editor.
	/// 
	/// This class act as booth the Logic controller and the Display widget.\n
	/// it contain all the Attributes displaying the values of the Table
	class AttributeEditorWidget : public QWidget
	{
		Q_OBJECT

		public:

			/// FilterMode
			typedef enum {
				AEFilterModeRegEx = 0, /*!< use Regular Expression to filter the attributes. */ 
				AEFilterModeWildcard,  /*!< use simple Wildcard to filter the attributes. */ 
				AEFilterModeString,	   /*!< simply serach for the string equivalent to searching for "*myString*" */
			} AEFilterMode;
			
			typedef boost::shared_ptr<AttributeEditorWidget> Ptr;

			/// contructor that create the child UI Elements
			AttributeEditorWidget(QWidget* parent = NULL);
			~AttributeEditorWidget();

			//*{ name accessors
			/// get the internal tree 
			AETreeView * 	getTreeView() 	{return m_treeView;}
			/// get the internal model 
			AEModel    *	getModel() 		{return m_model;}
			/// get the internal NodeData
			NodeDataPtr 	getNodeData() 	{return m_nodeData;}
			//*}

      /// set the internal data from a wrapper
      void setWrapper(SpliceGraphWrapper::Ptr wrapper);

			/// set the internal nodegraph from the groups
			void setParams(FabricCore::RTVal nodeData);
			
			/// this is a convenience function that give you direct 
			/// access to the internal args table 
			FabricCore::RTVal getArgs();
			
			/// update the args and also make sure the widget is reflecting this new value 
			void setWidgetRTVal( std::string name, FabricCore::RTVal value, bool silent = true );
			
			/// get the internal Value for the Attribute name 'attributeName'
			FabricCore::RTVal getAttributeValue(std::string attributeName);
	
			/// initial brute for setting up the widget to their current value
			void updateAllWidget();
	
			/// return a the widget for that name 
			AEWidget * getWidget(std::string argName);
			
			/// if the currentModel is the filter one
			/// this will reset the filter to be the normal model
			void removeProxyFilter();
	
			/// if needed create a proxy filter and attached it
			///	in between the model and the view \n
			/// use the slot filterTextChanged(QString) to change the text )\n
			/// and the slot filterTypeChanged(int) to change the mode \n
			void addProxyFilter();
			
			/// set the locked state of a named Widget in the AE
			void setWidgetLocked(std::string name , bool value);

			/// set the connected state of a named Widget in the AE
			void setWidgetConnected(std::string name , bool value);

			/// mostly exposed for debuging at the moment should be protected 
			void clearTree();

      /// refresh
      void updateParams();

		public slots:
			/// debug function to print the containt of m_nodeData
			void printArgs();
			
			/// this get hock up with the Tree so we can track AEWidget
			virtual void widgetAdded(AEWidget * widget , std::string name);
	
			/// this need to connected to the uiChanged methods of AEWidget
			/// added to the tree 
			virtual void widgetValueChanged(std::string attributeName);
	
			/// connect this to a signal that will control the text
			/// used by the filter , this will be using the current mode
			/// set by using : filterModeChanged(int mode);
			void filterTextChanged(QString text);
			
			///
			void filterModeChanged(AEFilterMode mode);
			
		private slots:
			/// mostly for debuging , activate the printArgs button when the args are dirty
			/// void activatePrintButton();
					
		signals:
			/// notify that one of the child  data could be wrong
			void valueForWidgetChanged( std::string attributeName );
	
			/// this is emited anytime of the the args is invalidate
			void attributeChanged( QSpliceGraphWrapper wrapper, std::string attributeName );
			
		protected:
						
			//void buildCollectionVisibilityMenu();
			//void updateMenuForCollection(CollectionWidget * collection , QMenu * menu);
			//void contextMenuEvent(QContextMenuEvent *event);
			
		protected:

			//! @name UI Elements
			//@{
			QScrollArea	* m_mainScollArea;
			QVBoxLayout * m_scrollLayout;
			QFrame		* m_mainFrame;

			/// this is a potential debug button ( normally not added or created )
			QPushButton * m_printArgsButton;
			/// in the current build this is not created because of a small bug in
			/// the actual display of the filtered attribute
			QLineEdit   * m_filterLine;
			//@}
			
			/// Data Elements
			NodeDataPtr m_nodeData;

			/// Data Elements
			FabricCore::RTVal m_groupData;
		
			/// the model for this AttributeEditor
			AEModel		* m_model;
			/// the tree view for this AttributeEditor
			AETreeView * m_treeView;
		
			/// in case we want to use a filter 
			AEFilterModel * m_proxyModel;
			
			/// the current text used by the filter
			QString m_filterText;
			
			/// the current mode used by the filter
			AEFilterMode m_filterMode;
			
			/// a map of widget for names
			typedef std::map< std::string , AEWidget * > WidgetMap;
			WidgetMap m_widgetMap;

      SpliceGraphWrapper::Ptr m_wrapper;
	};
	
}


#endif
