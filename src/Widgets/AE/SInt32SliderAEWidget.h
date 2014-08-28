#ifndef __SINT32SLIDERAEWIDGET_H__
#define __SINT32SLIDERAEWIDGET_H__

#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QContextMenuEvent>
#include <map>
#include <set>
#include <vector>
#include <QtGui/QScrollArea>
#include <QtGui/QGridLayout>
#include <QtGui/QSlider>
#include <QtGui/QLineEdit>
#include <QtGui/QIntValidator>

#include "AEWidget.h"

#include "macros.h"
namespace FabricSplice
{
	/// \brief AEWidget to represent a SInt32 Attribute
	class SInt32SliderAEWidget : public AEWidget
	{
		Q_OBJECT

		public:

			SInt32SliderAEWidget( FabricCore::RTVal param ,QWidget* parent = NULL);
			~SInt32SliderAEWidget() {}

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
      virtual void sliderMoved(int pos);
      virtual void textChanged(const QString & text);

		private slots:

		signals:

		protected:


		protected:


		protected:

      typedef QLineEdit ItemWidget;
      std::vector<QSlider *> m_sliders;
		  std::vector<ItemWidget *> m_widgets;
      bool m_notifying;

      QWidget * m_listWidget;
      QIntValidator * m_validator;

	};
	
	
}


#endif
