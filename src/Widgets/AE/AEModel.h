#ifndef __AEMODEL_H__
#define __AEMODEL_H__

#include <QtGui/QTreeView>
#include <QtCore/QAbstractItemModel>

#include <QtCore/QModelIndex>
#include <QtCore/QVariant>

#include "AEItem.h"
#include <boost/shared_ptr.hpp>

#include <FabricSplice.h>

namespace FabricSplice {

class AEItem;

#ifndef __NODEDATA_FWD__
#define __NODEDATA_FWD__

class NodeData;
typedef boost::shared_ptr<NodeData> NodeDataPtr;

#endif

/// \brief Subclass of QAbstractItemModel that is used model a NodeData object
/// 
/// this is a mostly standart model for TreeView , it used the NodeData input table
/// to populate the tree structures 
/// some of the DisplayRole are tweak to return description for row
class AEModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    AEModel(NodeDataPtr nodeData, QObject *parent = 0);
    ~AEModel();

    QVariant data(const QModelIndex &index, int role) const;
  	bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
	
	  Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	
    QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
	
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

private:
    void setupModelData( NodeDataPtr nodeData  );
	  void addModelData(std::vector<FabricSplice::DGPort> ports, AEItem *parent);
		
    AEItem *m_rootItem;
    QTreeView *m_treeView;
};

}


#endif // __AEMODEL_H__
