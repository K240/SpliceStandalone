#ifndef __AEWIDGET_H__
#define __AEWIDGET_H__

#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QLabel>

#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>
#include <iostream>

#include <FabricCore.h>

#include "macros.h"

namespace FabricSplice
{
	class AttributeEditorWidget;

	/// \brief Base class for all the Attribute Editor Widget
	/// 
	/// any Widget that want to be part of an AE need to have this class as on of its parent \n
	/// note that although this is not an abstract class some of the vitual function really need 
	/// to be re-implemented by the child class for them to function properly \n
	/// please refer to the documentation on creating custom attribute 
	class AEWidget : public QWidget
	{
		Q_OBJECT
		
	public:
		
		//! @name Constructors , Desctructors and Create function
		//@{
		/// default contructor take the descriotion info from the Inputs VariantMap 	
		AEWidget( FabricCore::RTVal param , QWidget* parent = NULL);
		~AEWidget();

		/// creation function
		static AEWidget * create( FabricCore::RTVal param, QWidget* parent = NULL);
		//@}

		//! @name Default Layout and Label Utils
		//@{
		/// basic layout with name/label and place for the control
		QHBoxLayout * createLabelControlLayout();
		/// get the display label based on the rules : label key first if not present name key ( name is mandatory )
		QString getLabel();
				
		/// when the widget is used in compound or array it is sometime useful
		/// to access the QLabel to change a little its text or shape
		QLabel * getDefaultLabel() {return m_defaultLabelWidget;}
    
    // returns the wrapped rtval
    FabricCore::RTVal getRTVal() { return m_param; }

    // sets the wrapped RTVal
    virtual void setRTVal(FabricCore::RTVal param) { m_param = param; }

		//@}

		//! @name Locking and Connection Accessors
		//@{
		/// set the lock state ,this will also update the pallete to the lock state
		void setLocked(bool value);
		/// simply return the lock state value
		bool isLocked() {return m_lock;};

		/// set the lock state ,this will also update the pallete to the lock state
		void setConnected(bool value);
		/// simply return the lock state value
		bool isConnected() {return m_connected;};
		//@}

		/// we can create a empty on for missing registration
		void missingRegistration();

		/// setter for the extra action , see the m_extraMenuActions docs for more detail
		void	setExtraMenuActions(QList<QAction*> actions) {m_extraMenuActions = actions;}
		
		/// when in silentUpdate mode the signal : uiChanged is not emited when setInternalValue is called
		/// useful when you are already reacting to the environement and dont want ot create a loop ...
		void setSilentUpdate(bool silent) {m_silentUpdate = silent;}
		
	signals:
		/// emited each time the value of a widget changed
		void AEWigetValueChanged(std::string name);
		
		/// emited each time the internal value of a widget changed and the ui need updating
		void AEWigetInternalChanged(std::string name);

	protected slots:
		/// this should get connected to the widgets that are connected to the value of the attribute
		virtual void uiChanged();
		
	private slots:
		/// call when the internal data as change invalidating any cache in the editor
		void valueChanged();
		
		/// set the value when the user select an entry in the menu
		//void setPreset(Muggins::VariantPtr value);
		
		/// switch the lock state (on become off etc...)
		void switchLock();
		
	protected:
		/// the menu in case there is presets
		void contextMenuEvent(QContextMenuEvent *event);
		
		/// will go though each widget children that is not
		/// m_defaultLabelWidget and set its enable state to value
		void setEnabledOnChildren(bool value);
		
	protected:

		/// default label Widget
		QLabel * m_defaultLabelWidget;
		
		/// this is the name for this attributes
		std::string m_attributeName;
		
		// parameter that contains all specific information.
		FabricCore::RTVal m_param;
		
		/// a preset table if a single level 
		/// variant map of string=value
		//Muggins::VariantMapPtr m_presets;
		
		/// a List of action to be added to each Context Menu
		/// this enable external object to add their action to the Menu
		QList<QAction *> m_extraMenuActions;
				
		/// dictate is the uiUpdate are silenced 
		/// usefull when you want to update a UI without any further action being triggered
		bool m_silentUpdate;
		
		// information about the state of the attribute 
		// those are mostly used for the display , the Model should enforce them 
		
		/// is the attributes locked
		bool m_lock;
		
		/// is the attributes connected
		bool m_connected;
		
	};
	
	
};


#endif
