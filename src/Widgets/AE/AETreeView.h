#ifndef __AETREEVIEW_H__
#define __AETREEVIEW_H__

#include <QtGui/QTreeView>
#include "AEWidget.h"

namespace FabricSplice {

/// \brief Subclass of TreeView that is used to display the list of Attributes
/// 
/// this class implemente a special way to expand item so creation of entry 
/// is lazy and also managed the call to setWidgetForItem in order to create 
/// the customs widgets for each attribute 
class AETreeView : public QTreeView
{
	Q_OBJECT
 
	public:
	    AETreeView(QWidget * parent = 0);
		~AETreeView() {};

	protected:
		/// the menu in case there is presets
		void contextMenuEvent(QContextMenuEvent *event);	
		void swapVisibility(const QModelIndex & index);

	signals:
		/// emitted when a new widget is added to the tree 
		/// the controller of this tree should register to
		/// this signal to keep track the widget changes
		void widgetAdded(AEWidget * widget , std::string name);

	public:
		void	expandAllAfterSearch();
		void	expandRootItem();
				
	public slots:
		void	createChildWidget( const QModelIndex & index );
		void	showAllChildCollection(const QModelIndex & index);
		void	hideAllChildCollection(const QModelIndex & index);
		void	updateIndexHeight(const QModelIndex & index );
		
	private:
		void	setWidgetForItem(const QModelIndex & index );
		void	expandChildOf(const QModelIndex & index);
		
};

}

#endif // __AETREEVIEW_H__
