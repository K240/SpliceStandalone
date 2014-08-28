#include "AEWidgetFactory.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QPainter>
#include <QtGui/QMenu>
#include <QtGui/QScrollBar>
#include <QtGui/QDialog>
#include <QtGui/QPushButton>

#include "macros.h"

#include "AEWidget.h"
#include "BooleanAEWidget.h"
#include "Float32AEWidget.h"
#include "Float32SliderAEWidget.h"
#include "SInt32AEWidget.h"
#include "SInt32SliderAEWidget.h"
#include "StringAEWidget.h"
#include "FilePathAEWidget.h"
#include "Vec2AEWidget.h"
#include "Vec3AEWidget.h"
#include "EulerAEWidget.h"
#include "Mat44AEWidget.h"
#include "XfoAEWidget.h"
#include "ColorAEWidget.h"

using namespace FabricSplice;

AEWidgetFactory g_widgetFactory;

const AEWidgetFactory & AEWidgetFactory::widgetFactory()
{
  return g_widgetFactory;
}

AEWidgetFactory::AEWidgetFactory()
	: m_standardTypeRegistered(false)
{
}

AEWidgetFactory::~AEWidgetFactory()
{
}

bool AEWidgetFactory::registerWidgetTypes() const
{
	registerStandardType();
	return true;
	
}

bool AEWidgetFactory::registerStandardType() const
{
	if (!m_standardTypeRegistered)
	{
		registerType("Boolean", BooleanAEWidget::create );
		registerType("Boolean[]", BooleanAEWidget::create );
    registerType("Float32", Float32AEWidget::create );
    registerType("Float32[]", Float32AEWidget::create );
    registerType("Float32Slider", Float32SliderAEWidget::create );
    registerType("Float32Slider[]", Float32SliderAEWidget::create );
    registerType("SInt32", SInt32AEWidget::create );
    registerType("SInt32[]", SInt32AEWidget::create );
    registerType("SInt32Slider", SInt32SliderAEWidget::create );
    registerType("SInt32Slider[]", SInt32SliderAEWidget::create );
    registerType("Vec2", Vec2AEWidget::create );
    registerType("Vec2[]", Vec2AEWidget::create );
    registerType("Vec3", Vec3AEWidget::create );
    registerType("Vec3[]", Vec3AEWidget::create );
    registerType("Euler", EulerAEWidget::create );
    registerType("Euler[]", EulerAEWidget::create );
    registerType("Mat44", Mat44AEWidget::create );
    registerType("Mat44[]", Mat44AEWidget::create );
    registerType("Xfo", XfoAEWidget::create );
    registerType("Xfo[]", XfoAEWidget::create );
    registerType("Color", ColorAEWidget::create );
    registerType("Color[]", ColorAEWidget::create );
    registerType("String"     , StringAEWidget::create );
    registerType("String[]"   , StringAEWidget::create );
    registerType("FilePath"     , FilePathAEWidget::create );
    registerType("FilePath[]"   , FilePathAEWidget::create );

		m_standardTypeRegistered = true;
		return true;
	}
	
	return false;
}

bool AEWidgetFactory::registerType(const std::string&  typeName, AEWidgetCreateFn creatorFn ) const
{
	if(m_widgetCreatorMap.find(typeName) == m_widgetCreatorMap.end())
	{
		m_widgetCreatorMap[typeName] = creatorFn;
		return true;
	}
	else	
		return false;
}

bool AEWidgetFactory::isRegistered(const std::string& typeName )
{
	return m_widgetCreatorMap.find(typeName) != m_widgetCreatorMap.end();
}

AEWidget * AEWidgetFactory::create(FabricCore::RTVal param ,
									QWidget* parent ) const
{
  FABRIC_TRY_RETURN("AEWidgetFactory::create", NULL,

    registerWidgetTypes();

    std::string paramType = param.callMethod("String", "getType", 0, 0).getStringCString();

  	TypeMap::const_iterator it = m_widgetCreatorMap.find(paramType);
  	if(it == m_widgetCreatorMap.end())
  	{
  		std::cout << "No registration exists for type " << paramType << std::endl;
  		return NULL;
  	}
  	else
  		return (it->second)(param , parent );

  );
}														

