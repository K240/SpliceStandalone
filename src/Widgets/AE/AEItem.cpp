#include "AEItem.h"
#include "macros.h"

#include <iostream>
		
using namespace FabricSplice;

AEItem::AEItem(FabricSplice::DGPort port , AEItem *parent)
	: m_port(port) , m_parentItem(parent)
{
}

AEItem::~AEItem()
{
}

void AEItem::appendChild(AEItem *item)
{
    childItems.append(item);
}

AEItem *AEItem::child(int row)
{
    return childItems.value(row);
}

int AEItem::childCount() const
{
    return childItems.count();
}

int AEItem::columnCount() const
{
    return 1;
}

QVariant AEItem::data(int column , int role) const
{
	if (childItems.size() == 0 || role == 999)
	{
		QDGPort qPort;
		qPort.port = m_port;
		QVariant var;
		var.setValue(qPort);
		return var;
	}
	else 
	{
    FABRIC_TRY_RETURN("AEItem::data", QVariant(),

  		std::string name = m_port.getName();
      if(name == "")
        name = "Ports";
	 	  return QVariant(QString("     ")+QString(name.c_str()).trimmed() );

    );
	}
}

QVariant AEItem::data(int column) const
{
  FABRIC_TRY_RETURN("AEItem::data", QVariant(),

    std::string name = m_port.getName();
    if(name == "")
      name = "Ports";
    return QVariant(QString("     ")+QString(name.c_str()).trimmed() );

  );
}

AEItem *AEItem::parent()
{
    return m_parentItem;
}

int AEItem::row() const
{
    if (m_parentItem)
        return m_parentItem->childItems.indexOf(const_cast<AEItem*>(this));
	
    return 0;
}
