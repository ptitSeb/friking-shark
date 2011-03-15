#pragma once

#define DEFAULT_OBJECT_SELECTOR_BUTTON_SIZE		128.0

class IEntityEditorPropertyPanel:virtual public IGameWindow
{
public:
	virtual void SetObject(ISystemObject *piObject)=0;
};

class IEntityEditorPropertyPanelEvents
{
public:
	virtual void OnObjectChanged(IEntityEditorPropertyPanel *piPanel,ISystemObject *piObject)=0;
};

class IEntityEditorObjectLabel:virtual public IGameWindow
{
public:
	virtual void SetObject(IDesignObject *piObject)=0;
};

class IEntityEditorNamedObjectSelector:virtual public IGameWindow
{
public:
	virtual bool SelectObject(IGameWindow *piParent,std::string sTitle,std::string sSystem,std::string *psSelectedObject)=0;
};

class IEntityEditorClassSelector:virtual public IGameWindow
{
public:
	virtual bool SelectClass(IGameWindow *piParent,std::string sTitle,std::string sSystem,std::string *psClass)=0;
};


class IEntityEditorObjectSelector: virtual public ISystemUnknown
{
public:
	
	virtual bool SelectObject(std::string sTitle,IGameWindow *piParent,std::vector<IDesignObject *> *vObjects,unsigned long *pnSelectedObject,double dButtonSizeX=DEFAULT_OBJECT_SELECTOR_BUTTON_SIZE,double dButtonSizeY=DEFAULT_OBJECT_SELECTOR_BUTTON_SIZE)=0;
	
};