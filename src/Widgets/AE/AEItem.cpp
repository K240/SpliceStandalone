#include "AEItem.h"
#include "macros.h"

#include <iostream>
		
using namespace FabricSplice;

AEItem::AEItem(FabricCore::RTVal param , AEItem *parent)
	: m_param(param) , m_parentItem(parent)
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
		QParameter qParam;
		qParam.parameter = m_param;
		QVariant var;
		var.setValue(qParam);
		return var;
	}
	else 
	{
    FABRIC_TRY_RETURN("AEItem::data", QVariant(),

  		std::string name = m_param.callMethod("String", "getLabel", 0, 0).getStringCString();
	 	  return QVariant(QString("     ")+QString(name.c_str()).trimmed() );

    );
	}
}

QVariant AEItem::data(int column) const
{
  FABRIC_TRY_RETURN("AEItem::data", QVariant(),

    std::string name = m_param.callMethod("String", "getLabel", 0, 0).getStringCString();
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
