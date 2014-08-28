#include <iostream>

#include "AEFilterModel.h"

using namespace FabricSplice;

AEFilterModel::AEFilterModel(QObject *parent)
 : QSortFilterProxyModel(parent)
{
	setFilterCaseSensitivity(Qt::CaseInsensitive);
	setDynamicSortFilter(true);
}

bool AEFilterModel::filterAcceptsRow(int sourceRow,const QModelIndex &sourceParent) const
{
	QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
	if ( sourceModel()->hasChildren( index ) )
	{
		int numChildren = sourceModel()->rowCount(index);
		for(int i=0;i<numChildren;i++)
		{
			bool value = filterAcceptsRow(i,index);
			if (value)
				return true;
		}
		return false;
	}
	
	QString name = sourceModel()->data( index , filterRole() ).toString();
	bool value = name.contains(filterRegExp());
	
	return value;
}
