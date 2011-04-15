#pragma once

#define DEFAULT_OBJECT_SELECTOR_BUTTON_SIZE		128.0

class IBSPDebuggerPropertyPanel:virtual public IGameWindow
{
public:
	virtual bool SetObject(ISystemObject *piObject)=0;
};

class IBSPDebuggerPropertyPanelEvents
{
public:
	virtual void OnObjectChanged(IBSPDebuggerPropertyPanel *piPanel,ISystemObject *piObject)=0;
	virtual void OnObjectRemoved(IBSPDebuggerPropertyPanel *piPanel,ISystemObject *piObject)=0;
};

class IBSPDebuggerObjectLabel:virtual public IGameWindow
{
public:
	virtual void SetObject(IDesignObject *piObject)=0;
};

class IBSPDebuggerNamedObjectSelector:virtual public IGameWindow
{
public:
	virtual bool SelectObject(IGameWindow *piParent,std::string sTitle,std::string sSystem,std::string *psSelectedObject)=0;
};

class IBSPDebuggerClassSelector:virtual public IGameWindow
{
public:
	virtual bool SelectClass(IGameWindow *piParent,std::string sTitle,std::string sSystem,std::string *psClass)=0;
};


class IBSPDebuggerObjectSelector: virtual public ISystemUnknown
{
public:
	
	virtual bool SelectObject(std::string sTitle,IGameWindow *piParent,std::vector<IDesignObject *> *vObjects,unsigned long *pnSelectedObject,double dButtonSizeX=DEFAULT_OBJECT_SELECTOR_BUTTON_SIZE,double dButtonSizeY=DEFAULT_OBJECT_SELECTOR_BUTTON_SIZE)=0;
	
};