#ifndef __AEFILTERMODEL_H__
#define __AEFILTERMODEL_H__

#include <QtCore/QDate>
#include <QtGui/QSortFilterProxyModel>

namespace FabricSplice {

/// \brief very basic subclass that re-implement filterAcceptsRow 
class AEFilterModel : public QSortFilterProxyModel
{
	Q_OBJECT

	public:
		AEFilterModel(QObject *parent = 0);

	protected:
		/// this will filter any row the normal way execpt that it will also deal with children
		/// so if any children would not be filtered the the item is not filter \n
		/// that enable parent row that dont match the filter to still be present to show their childs
		bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

	private:
};

}
#endif
