#include "AEModel.h"
#include "AEItem.h"
#include "NodeData.h"

#include "macros.h"
#include "SpliceStandalone.h"

using namespace FabricSplice;

#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)

AEModel::AEModel(NodeDataPtr nodeData , QObject *parent)
    : QAbstractItemModel(parent)
{
	//initialize a root parameter.
  FABRIC_TRY("AEModel::AEModel", 

  	m_rootItem = new AEItem(DGPort());
  	m_treeView = qobject_cast<QTreeView *>(parent);

    setupModelData(nodeData);
  );
}

AEModel::~AEModel()
{
    delete m_rootItem;
}

int AEModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<AEItem*>(parent.internalPointer())->columnCount();
    else
        return m_rootItem->columnCount();
}

bool AEModel::setData(const QModelIndex & index, const QVariant & value , int role )
{
    if (!index.isValid())
        return false;

	return false;
	
}

QVariant AEModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

	AEItem *item = static_cast<AEItem*>(index.internalPointer());
	
	if (role == Qt::DisplayRole )
	{
		if (item->childCount() > 0)
			return item->data(index.column(),role);
		else
			return QVariant();
	}
	else if (role == Qt::BackgroundColorRole )
	{

		if (item->childCount() > 0)
		{
			return QVariant(QColor(78, 78, 78));
		}

	}
	else if (role == Qt::DecorationRole )
	{

		if (item->childCount() > 0)
		{
			QString path = QString(STRINGIZE_VALUE_OF(IMAGES_PATH));

			if(m_treeView->isExpanded(index))
			{
				QString imagePath = path + "/down_small_white.png";
				return QVariant(QPixmap(imagePath));
			}
			else
			{
				QString imagePath = path + "/right_small_white.png";
				return QVariant(QPixmap(imagePath));
			}
		}

	}
	else if(role == Qt::SizeHintRole)
	{
		if (item->childCount() > 0)
		{
			return QVariant(QSize(0,22));
		}
	}
	else if (role == Qt::FontRole )
	{
		if (item->childCount() > 0)
		{
			QFont font = QFont("arial",10,2,false);
			font.setBold(true);
			return QVariant(font);
		}
		else
		{
			return QVariant(getApplicationWidgetFont());
		}
	}
	else if (role == 998 )
	{
		return item->data(index.column());
	}
	else if (role == 999 )
	{
		return item->data(index.column(),role);
	}
		

	return QVariant();	
}

Qt::ItemFlags AEModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled ;
	//| Qt::ItemIsSelectable
}

QVariant AEModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return m_rootItem->data(section);

    return QVariant();
}

QModelIndex AEModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    AEItem *parentItem;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<AEItem*>(parent.internalPointer());

    AEItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex AEModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    AEItem *childItem = static_cast<AEItem*>(index.internalPointer());
    AEItem *parentItem = childItem->parent();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int AEModel::rowCount(const QModelIndex &parent) const
{
  AEItem *parentItem;
  if (!parent.isValid())
  {
    parentItem = m_rootItem;
  }
  else
  {
    if(parent.column() > 0)
        return 0;
    parentItem = static_cast<AEItem*>(parent.internalPointer());
  }

  return parentItem->childCount();
}


void AEModel::addModelData(NodeData::DGPortList ports, AEItem *parent)
{
  FABRIC_TRY("AEModel::addModelData",

    for(size_t i=0;i<ports.size();i++)
    {
      AEItem * item = new AEItem(ports[i], parent);
      printf("AEModel::addModelData %s\n", ports[i].getName());
    	parent->appendChild(item);
    }

  );
}

void AEModel::setupModelData(NodeDataPtr nodeData)
{
	addModelData(nodeData->getTable() , m_rootItem);
}	

