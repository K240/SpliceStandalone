#include "AETreeView.h"
#include "AEItem.h"

#include "AEWidget.h"
#include "AEWidgetFactory.h"

#include <QtGui/QLineEdit>
#include <QtCore/QStack>
#include <QtGui/QMenu>
#include <QtCore/QPoint>
#include <QtCore/QSignalMapper>

#include <iostream>

#include "macros.h"

using namespace FabricSplice;

AETreeView::AETreeView(QWidget * parent)
	: QTreeView(parent)
{
	setFocusPolicy(Qt::NoFocus);
	setSelectionMode(QAbstractItemView::NoSelection);
	//setAlternatingRowColors( true );
	setIndentation(4);
	setHeaderHidden(true);
	setMinimumWidth(80);
	connect(this , SIGNAL(expanded ( const QModelIndex & )) , this , SLOT(createChildWidget(const QModelIndex & ) ) );
	

	QPalette myPalette;
	myPalette.setColor(QPalette::Base , QColor(68,68,68));
	setPalette(myPalette);

	setStyleSheet( QString("QTreeView::branch:has-children {"
														   "background: #4e4e4e;"
														   "border: 1px solid #252525;"
														   "border-bottom-color: #252525;"
														   "border-right-color: transparent;"
														   "border-left-color: #616161;"
														   "border-top-color: #616161;"
															"} "
															
						"QTreeView::branch:!has-children {	 background: #444444;"
															"border: 1px solid #252525;"
															"border-bottom-color: transparent;"
															"border-right-color: transparent;"
															"border-left-color: #252525;"
															"border-top-color: transparent;"
															"} "
						"QTreeView::item:has-children {"
															 "background-color: #4e4e4e; "
															 "border: 1px solid #252525;"
															 "border-bottom-color: #252525;"
															 "border-right-color: #252525;"
															 "border-left-color: transparent;"
															 "border-top-color: #616161;"
															 "} "
/*
															
						   "QTreeView::item:closed:has-children {"
															"background-image: url(/usr/people/damien-f/AEClosedBranchArrow.png) no-repeat 3 0 ;"
															"background-color: #4e4e4e; "
															"border: 1px solid #252525;"
															"border-bottom-color: #252525;"
															"border-right-color: #252525;"
															"border-left-color: transparent;"
															"border-top-color: #616161;"
															"padding: 2px;"
															"} "
															
							"QTreeView::item:open:has-children {"	
															 "background-image: url(/usr/people/damien-f/AEOpenBranchArrow.png) no-repeat 3 0 ;"
															 "background-color: #4e4e4e; "
															 "border: 1px solid #252525;"
															 "border-bottom-color: #252525;"
															 "border-right-color: #252525;"
															 "border-left-color: transparent;"
															 "border-top-color: #616161;"
															 "padding: 2px;"
															 "} "
															
							"QTreeView::item:!has-children {     background: #444444;"
																"border: 1px solid #252525;"
																"border-bottom-color: transparent;"
																"border-right-color: #616161;"
																"border-left-color: transparent;"
																"border-top-color: transparent;"
																"} "
*/																

						   ));

	
	
}

void	AETreeView::createChildWidget(const QModelIndex & index )
{
	int numChildren = model()->rowCount(index);
  printf("AETreeView::createChildWidget numChildren %d\n", numChildren);
	for (int i= 0;i< numChildren ;i++)
	{
    printf("AETreeView::createChildWidget item %d\n", i);
		setWidgetForItem( model()->index(i,0 , index ) );
	}
	resizeColumnToContents( 0 );
}

void AETreeView::expandRootItem()
{
	QModelIndex firstItem = model()->index(0, 0, rootIndex() );
	expand(firstItem);
	this->update();
}

void AETreeView::expandAllAfterSearch()
{
	QModelIndex current = rootIndex();
	expandChildOf(current);
}


void AETreeView::expandChildOf(const QModelIndex & index)
{
	for (int row = 0; row < model()->rowCount(index); ++row) 
	{
		QModelIndex child = model()->index(row, 0, index);
		if (!child.isValid())
			break;
			
		int numChildren = model()->rowCount(child);
		if ( numChildren  > 0 ) 
			expandChildOf(child);
	}
	expand(index);
	this->update();
}

void	AETreeView::setWidgetForItem(const QModelIndex & index )
{
  FABRIC_TRY("AETreeView::setWidgetForItem", 

  	QVariant var = index.data(999);
  	QDGPort qPort = var.value<QDGPort>();
    FabricSplice::DGPort port = qPort.port;
		std::string name = port.getName();

    printf("AETreeView::setWidgetForItem %s\n", name.c_str());

    // filter out time
    if(name == "time")
      return;

    AEWidget* newWidget = AEWidgetFactory::widgetFactory().create( port , this );
    if (!newWidget)
    {
      newWidget = AEWidget::create( port , this );
      newWidget->missingRegistration();
    }
		emit widgetAdded(newWidget , name);
		setIndexWidget( index , newWidget );

  );
}

void AETreeView::updateIndexHeight(const QModelIndex & index )
{
	setRowHidden ( index.row() , index.parent() , true);
	setRowHidden ( index.row() , index.parent() , false);
}

void AETreeView::showAllChildCollection(const QModelIndex & index)
{
	for (int i=0;i<model()->rowCount(index) ;i++)
	{
		QModelIndex childIndex = model()->index(i,0,index);
		if ( model()->hasChildren(childIndex) )
		{
			setRowHidden (childIndex.row(), childIndex.parent() , false);
		}
	}
}

void AETreeView::hideAllChildCollection(const QModelIndex & index)
{
	for (int i=0;i<model()->rowCount(index) ;i++)
	{
		QModelIndex childIndex = model()->index(i,0,index);
		if ( model()->hasChildren(childIndex) )
		{
			setRowHidden(childIndex.row(), childIndex.parent() , true);
		}
	}
}



void AETreeView::swapVisibility(const QModelIndex & index)
{
	//setVisibility(!m_visible);
	setRowHidden (index.row(), index.parent() , !isRowHidden( index.row(), index.parent() ) );	
}

void AETreeView::contextMenuEvent(QContextMenuEvent *event)
{
	QPoint loc = event->pos();
	
	QModelIndex index = indexAt(loc);
	if ( model()->hasChildren(index) )
	{
		QMenu * menu = new QMenu(this);

		/// check box for visibilies 
		QAction * act = new QAction( model()->data(index,Qt::DisplayRole).toString() ,this);
		act->setCheckable(true);
		act->setChecked( !isRowHidden( index.row(), index.parent() ) );

    // hmathee::: todoooooo		
		// mpcUI::SignalHandler0::fn0 fn = boost::bind(&AETreeView::swapVisibility, this, index );
		// mpcUI::SignalHandler0::connect(act, SIGNAL(triggered()), fn );
		
		menu->addAction(act);
		
		menu->addSeparator();
		
		for (int i=0;i<model()->rowCount(index) ;i++)
		{
			QModelIndex childIndex = model()->index(i,0,index);
			if ( model()->hasChildren(childIndex) )
			{
				QAction * act = new QAction(  model()->data(childIndex,Qt::DisplayRole).toString() ,this);
				act->setCheckable(true);
				act->setChecked( !isRowHidden( childIndex.row(), childIndex.parent() ) );

        // hmathee::: todoooooo   
				// fn = boost::bind(&AETreeView::swapVisibility, this, childIndex );
				// mpcUI::SignalHandler0::connect(act, SIGNAL(triggered()), fn );

				menu->addAction(act);
			}
		}
		
		menu->addSeparator();
	
		QAction * showAllAction = new QAction( "Show All" ,this);

    // hmathee::: todoooooo   
		// fn = boost::bind(&AETreeView::showAllChildCollection, this, index );
		// mpcUI::SignalHandler0::connect(showAllAction, SIGNAL(triggered()), fn );
		menu->addAction(showAllAction);
	
		QAction * hideAllAction = new QAction( "Hide All" ,this);
    // hmathee::: todoooooo   
		// fn = boost::bind(&AETreeView::hideAllChildCollection, this, index );
		// mpcUI::SignalHandler0::connect(hideAllAction, SIGNAL(triggered()), fn );
		menu->addAction(hideAllAction);
		
		menu->exec(event->globalPos());
	}


	
}



