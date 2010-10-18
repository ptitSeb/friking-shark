#pragma once

#include "GameRuntimeLib.h"
#include "GameGraphics.h"

class IGameWindow;

enum eGameGUIReferenceSystem
{
	eGameGUIReferenceSystem_None,
	eGameGUIReferenceSystem_Absolute,
	eGameGUIReferenceSystem_Relative
};

struct SGameRect;

struct SGamePos
{
	double x;
	double y;

	void ClipToUnit();
	void ClipToRect(SGameRect *pRect);
	void CenterOnRect(SGameRect *pRect);

	SGamePos(double _x,double _y);
	SGamePos();
};

struct SGameSize
{
	double w;
	double h;

	SGameSize(double _w,double _h);
	SGameSize();
};

struct SGameRect
{
	double x;
	double y;
	double w;
	double h;

	void ClipToUnit();
	void ClipToRect(SGameRect *pRect);
	void CenterOnRect(SGameRect *pRect);

	bool Contains(SGamePos point);

	SGameRect(double _x,double _y,double _w,double _h);
	SGameRect();
};

BEGIN_STRUCT_PROPS(SGameRect)
	PROP_VALUE(x,"x",0)
	PROP_VALUE(y,"y",0)
	PROP_VALUE(w,"w",0)
	PROP_VALUE(h,"h",0)
END_STRUCT_PROPS()

BEGIN_STRUCT_PROPS(SGameSize)
	PROP_VALUE(w,"w",0)
	PROP_VALUE(h,"h",0)
END_STRUCT_PROPS()

BEGIN_STRUCT_PROPS(SGamePos)
	PROP_VALUE(x,"x",0)
	PROP_VALUE(y,"y",0)
END_STRUCT_PROPS()

DECLARE_SERIALIZABLE_ENUMERATION(eGameGUIReferenceSystem);

struct SGameScreenProperties
{
	bool										bFullScreen;
	bool										bWindowCentered;
	eGameGUIReferenceSystem eWindowReferenceSystem;
	SGameRect								rWindowRect;
	SGameSize 								sFullScreenResolution;
	double    								dFullScreenRefreshRate;
	double									dFullScreenRefreshBitsPerPixel;
	bool									bVerticalSync;

	SGameScreenProperties(){bFullScreen=false;bWindowCentered=false;eWindowReferenceSystem=eGameGUIReferenceSystem_None;dFullScreenRefreshRate=0;bVerticalSync=false;dFullScreenRefreshBitsPerPixel=0;}
};

BEGIN_STRUCT_PROPS(SGameScreenProperties)
// Hay un problema a corregir. Aqui no pilla valores por defecto de SGameRect ni SGameSize para utilizar PROP_VALUE_xxx
// Se ha pasado el codigo de default al Unserialize del GUIManager.
	PROP_FLAGS(rWindowRect,"WindowRect",MRPF_NORMAL|MRPF_OPTIONAL) 
	PROP_FLAGS(sFullScreenResolution,"Resolution",MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(bFullScreen,"FullScreen",FALSE,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(bWindowCentered,"CenterWindow",TRUE,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(eWindowReferenceSystem,"WindowReferenceSystem",eGameGUIReferenceSystem_Relative,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(dFullScreenRefreshRate,"RefreshRate",0,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(dFullScreenRefreshBitsPerPixel,"BitsPerPixel",0,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(bVerticalSync,"VerticalSync",false,MRPF_NORMAL|MRPF_OPTIONAL)
END_STRUCT_PROPS()

class IGameWindowEnumerationCallback
{
public:
	virtual void ProcessEnumeratedWindow(IGameWindow *piWindow,bool *pbStopEnumerating)=0;

	virtual ~IGameWindowEnumerationCallback(){}
};

class IGameGUIManager:virtual public ISystemUnknown
{
public:

	virtual void EnterGUILoop()=0; 
	virtual void ExitGUILoop()=0; 

	virtual void				GetWindowSize(SGameSize	*pSize)=0; // Rectangulo de la ventana de windows

	virtual void				GetMousePosition(IGameWindow *piWindow,SGamePos *pPos)=0;
	virtual void				SetMousePosition(IGameWindow *piWindow,SGamePos &pPos)=0;

	virtual bool				IsKeyDown(int nKey)=0;
	virtual bool				IsMouseDown(int nMouseButton)=0;

	virtual void				SetFocus(IGameWindow *piWindow)=0;
	virtual IGameWindow*GetFocus()=0;

	virtual void				SetMouseCapture(IGameWindow *piWindow)=0;
	virtual void				ReleaseMouseCapture()=0;
	virtual IGameWindow *GetMouseCapture()=0;

	virtual void				ShowMouseCursor(bool bShow)=0;
	virtual bool				IsMouseCursorVisible()=0;

	virtual	void				RedrawAll()=0;

	virtual IGameWindow *GetMainWindow()=0;

	// Popup management.

	virtual void		 AddPopup(IGameWindow *)=0;
	virtual void		 RemovePopup(IGameWindow *)=0;
	virtual void		 EnumeratePopups(IGameWindowEnumerationCallback *piCallback)=0;
	virtual void		 GetPopups(std::vector<IGameWindow *> *pvPopups)=0;

	void GetScreenProperties(SGameScreenProperties *pProperties);
	void SetScreenProperties(SGameScreenProperties *pProperties);
};

class IGameWindow: virtual public ISystemObject
{	
public:
	virtual bool		InitWindow(IGameWindow *piParent,bool bPopup)=0;
  virtual void		DestroyWindow()=0;

	virtual void		SetWindowName(std::string sName)=0;
	virtual std::string GetWindowName()=0;

	virtual IGameGUIManager *GetGUIManager()=0;

	virtual bool		IsPopup()=0;

	virtual void		Show(bool bShow)=0;
	virtual void		Activate(bool bActivate)=0;
	virtual bool		IsVisible()=0;
	virtual bool		IsActive()=0;

	virtual void		GetRect(SGameRect *pRect)=0;
	virtual void		SetRect(SGameRect *pRect)=0;

	virtual void		SetFont(IGenericFont *piFont,double dSize)=0;
	virtual void		GetFont(IGenericFont **ppiFont,double *pdSize)=0;

	virtual void		UpdateRealRect()=0;
	virtual void		GetRealRect(SGameRect *pRect)=0;

	virtual void		SetReferenceSystem(eGameGUIReferenceSystem eRefSystem)=0;
	virtual eGameGUIReferenceSystem GetReferenceSystem()=0;

	virtual IGameWindow *GetParent()=0;
	virtual void		 GetChildren(std::vector<IGameWindow *> *pvChildren)=0;

	virtual void		 AddChild(IGameWindow *)=0;
	virtual void		 RemoveChild(IGameWindow *)=0;
	virtual void		 EnumerateChildren(IGameWindowEnumerationCallback *piCallback)=0;
	virtual IGameWindow *FindChild(std::string sName)=0;

	virtual void		BringToFront()=0;
	virtual void		BringChildToFront(IGameWindow *piChild)=0;

	virtual IGenericTexture *GetBackgroundTexture()=0;
	virtual void			 SetBackgroundTexture(IGenericTexture *piTexture)=0;
	virtual IGenericModel	*GetBackgroundModel()=0;
	virtual void			 SetBackgroundModel(IGenericModel *piModel)=0;

	virtual void	GetBackgroundColor(CVector *pvColor,double *pdAlpha)=0;
	virtual void	SetBackgroundColor(CVector vColor,double dAlpha)=0;

	// Events received from the manager

	virtual void OnKeyDown(int nKey,bool *pbProcessed)=0;
	virtual void OnKeyUp(int nKey,bool *pbProcessed)=0;

	virtual void OnMouseDown(int nButton,double x,double y)=0;
	virtual void OnMouseUp(int nButton,double x,double y)=0;
	virtual void OnMouseMove(double x,double y)=0;

	virtual void OnDrawMouseCursor(SGamePos position,IGenericRender *piRender,bool *pbDrawed)=0;
	virtual void OnDrawBackground(IGenericRender *piRender)=0;
	virtual void OnDraw(IGenericRender *piRender)=0;

	virtual void OnSetFocus()=0;
	virtual void OnKillFocus(IGameWindow *piFocusedWindow)=0;

	virtual void OnReleaseMouseCapture()=0;
};

class IGameDialog: virtual public IGameWindow
{	
public:

	virtual int	Execute(IGameWindow *piParent)=0;
};

class IGameWindowEvents
{
public:

	virtual ~IGameWindowEvents(){}
};

class IGameGUILabel : virtual public IGameWindow
{
public:
	virtual void				SetText(std::string sText)=0;
	virtual std::string GetText()=0;

	virtual void	GetTextColor(CVector *pvColor,double *pdAlpha)=0;
	virtual void	SetTextColor(CVector vColor,double dAlpha)=0;

	virtual void				SetHorizontalAlignment(eTextAlignment eAlignment)=0;
	virtual void				SetVerticalAlignment(eTextAlignment eAlignment)=0;
	virtual eTextAlignment		GetHorizontalAlignment()=0;
	virtual eTextAlignment		GetVerticalAlignment()=0;

};

class IGameGUIButton : virtual public IGameGUILabel
{
public:
};

class IGameGUIButtonEvents
{
public:
	 virtual void OnButtonClicked(IGameGUIButton *piControl)=0;

	 virtual ~IGameGUIButtonEvents(){}
};
