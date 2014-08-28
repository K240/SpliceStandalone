#ifndef __BOOLEANAEWIDGET_H__
#define __BOOLEANAEWIDGET_H__

#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QContextMenuEvent>
#include <map>
#include <set>
#include <vector>
#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>
#include <QtGui/QCheckBox>

#include "AEWidget.h"

#include "macros.h"
namespace FabricSplice
{
	/// \brief AEWidget to represent a Boolean Attribute
	class BooleanAEWidget : public AEWidget
	{
		Q_OBJECT

		public:

			BooleanAEWidget( FabricCore::RTVal param ,QWidget* parent = NULL);
			~BooleanAEWidget() {}

			// creation function
			static AEWidget * create( FabricCore::RTVal param ,QWidget* parent = NULL);

			// convenient function to get the value of the check as a bool
			FabricCore::RTVal getValueArray();

      // sets the wrapped RTVal
      virtual void setRTVal(FabricCore::RTVal param);

		public slots:

			void setValueArray(FabricCore::RTVal values);

    protected slots:
      virtual void uiChanged();

		private slots:

		signals:

		protected:


		protected:


		protected:

      typedef QCheckBox ItemWidget;
		  std::vector<ItemWidget *> m_widgets;

      QWidget * m_listWidget;

	};
	
	
}


#endif
