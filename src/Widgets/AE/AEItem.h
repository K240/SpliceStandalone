#ifndef __AEITEM_H__
#define __AEITEM_H__

#include <QtCore/QVariant>
#include <FabricCore.h>

/// \brief Simple wrapper around a FabricCore::RTVal to expose it to Qt
struct QParameter
{
	FabricCore::RTVal parameter;
};

Q_DECLARE_METATYPE(QParameter)

namespace FabricSplice
{

  /// \brief basic Model item that hold an AE Description VariantMap
  class AEItem
  {
  	public:
  		AEItem(FabricCore::RTVal param , AEItem *parent = 0);
  		virtual ~AEItem();
  	
  		void appendChild(AEItem *child);
  		
  		virtual AEItem *child(int row);
  		virtual int childCount() const;
  		int columnCount() const;
  		
  		QVariant data(int column) const;
  		QVariant data(int column , int role) const;
  		int row() const;
  		AEItem *parent();
  	
  		FabricCore::RTVal getParameter() { return m_param;}
  		
  	private:
  		QList<AEItem*> childItems;
  		
  		mutable FabricCore::RTVal m_param;
  		AEItem *m_parentItem;
  	
  };

};

#endif // __AEITEM_H__
