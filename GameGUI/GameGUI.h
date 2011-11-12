//  Friking Shark, a Flying Shark arcade remake.
//  Copyright (C) 2011  Javier Martin Garcia (javiermartingarcia@gmail.com)
//	
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


#ifndef __GAME_GUI__
#define __GAME_GUI__

#include <GameRunTimeLib.h>
#include <GameGraphics.h>

class IGameWindow;

#define DIALOG_OK 		1
#define DIALOG_CANCEL 	0

#define DIALOG_YES 		1
#define DIALOG_NO 		0

enum eGameGUIReferenceSystem
{
	eGameGUIReferenceSystem_None,
	eGameGUIReferenceSystem_Absolute,
	eGameGUIReferenceSystem_Relative
};

enum eGameGUIChildrenLayout
{
	eGameGUIChildrenLayout_None,
	eGameGUIChildrenLayout_Vertical,
	eGameGUIChildrenLayout_Horizontal
};

enum EMessageDialogType
{
	eMessageDialogType_Info,
	eMessageDialogType_Warning,
	eMessageDialogType_Error,
	eMessageDialogType_Question
};

DECLARE_SERIALIZABLE_ENUMERATION(eGameGUIReferenceSystem)
DECLARE_SERIALIZABLE_ENUMERATION(eGameGUIChildrenLayout)

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
	PROP_VALUE_FLAGS(bFullScreen,"FullScreen",false,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(bWindowCentered,"CenterWindow",true,MRPF_NORMAL|MRPF_OPTIONAL)
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
	virtual bool				GetKeyName(int nKey,std::string *psKey)=0;
	
	virtual void				SetFocus(IGameWindow *piWindow)=0;
	virtual bool				HasFocus(IGameWindow *piWindow)=0;
	virtual IGameWindow 		*GetFocusedWindow()=0;
	
	virtual void				SetMouseCapture(IGameWindow *piWindow)=0;
	virtual void				ReleaseMouseCapture()=0;
	virtual bool				HasMouseCapture(IGameWindow *piWindow)=0;

	virtual void				ShowMouseCursor(bool bShow)=0;
	virtual bool				IsMouseCursorVisible()=0;

	virtual	void				RedrawAll()=0;

	virtual IGameWindow *GetMainWindow()=0;

	// Popup management.

	virtual void		 AddPopup(IGameWindow *)=0;
	virtual void		 RemovePopup(IGameWindow *)=0;
	virtual void		 EnumeratePopups(IGameWindowEnumerationCallback *piCallback)=0;
	virtual void		 GetPopups(std::vector<IGameWindow *> *pvPopups)=0;

	virtual bool 		 DetectDrag(double dx,double dy)=0;
	
	virtual void GetScreenProperties(SGameScreenProperties *pProperties)=0;
	virtual void SetScreenProperties(SGameScreenProperties *pProperties)=0;
	
	virtual void		RestoreViewport()=0;
	
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
	virtual	double 		GetSizeInLayout()=0;
	virtual void    	SetSizeInLayout(double dSize)=0;
	
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

	virtual void OnCharacter(int wCharacter,bool *pbProcessed)=0;
	virtual void OnKeyDown(int nKey,bool *pbProcessed)=0;
	virtual void OnKeyUp(int nKey,bool *pbProcessed)=0;

	virtual void OnMouseDown(int nButton,double x,double y)=0;
	virtual void OnMouseDoubleClick(int nButton,double x,double y)=0;
	virtual void OnMouseUp(int nButton,double x,double y)=0;
	virtual void OnMouseMove(double x,double y)=0;
	virtual void OnMouseWheelUp(double x,double y)=0;
	virtual void OnMouseWheelDown(double x,double y)=0;

	virtual void OnDrawMouseCursor(SGamePos position,IGenericRender *piRender,bool *pbDrawed)=0;
	virtual void OnDrawBackground(IGenericRender *piRender)=0;
	virtual void OnDraw(IGenericRender *piRender)=0;

	virtual void OnSetFocus()=0;
	virtual void OnKillFocus(IGameWindow *piFocusedWindow)=0;
	virtual void OnWantFocus(bool *pbWant)=0;

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

class IGameGUIEdit: virtual public IGameGUILabel
{
public:
	virtual void	GetBorderColor(CVector *pvColor,double *pdAlpha)=0;
	virtual void	SetBorderColor(CVector vColor,double dAlpha)=0;
	virtual void	SetCursor(int nIndex)=0;

	virtual void	GetSelection(int *pnFirst,int *pnLast)=0;
	virtual void	SetSelection(int nFirst,int nLast)=0;
};

class IGameGUIEditEvents
{
public:
	 virtual void OnTextChanged(IGameGUIEdit *piControl,std::string sNewText)=0;
  
  	 virtual ~IGameGUIEditEvents(){}
};

class IGameGUIButton : virtual public IGameGUILabel
{
public:
	virtual void EnableSounds()=0;
	virtual void DisableSounds()=0;
	virtual bool AreSoundsEnabled()=0;
	
	virtual void PlayClickSound()=0;
	virtual void PlayHoverSound()=0;
};

class IGameGUIButtonEvents
{
public:
	 virtual void OnButtonClicked(IGameGUIButton *piControl)=0;

	 virtual ~IGameGUIButtonEvents(){}
};

class IGameGUISlider: virtual public IGameGUIButton
{
public:
	virtual void   SetValue(double dValue)=0;
	virtual double GetValue()=0;
};

class IGameGUISliderEvents
{
public:
	virtual void OnSliderValueChanged(IGameGUISlider *piControl,double dValue)=0;
	
	virtual ~IGameGUISliderEvents(){}
};

class IGameGUIList: virtual public IGameWindow
{
public:

	virtual unsigned long AddElement(std::string sText)=0;
	virtual void		  SetElement(unsigned long nIndex,std::string sText)=0;
	virtual void		  RemoveElement(unsigned long nIndex)=0;
	virtual void		  Clear()=0;
	
	virtual std::string  GetElement(unsigned int nElement)=0;
	virtual unsigned int GetElementCount()=0;
	
	virtual void 		 SetSelectedElement(int nElement)=0;
	virtual int 		 GetSelectedElement()=0;
};

class IGameGUIListEvents
{
public:
	 virtual void OnSelectionChanged(IGameGUIList *piControl,int nElement,std::string sElement)=0;
	 virtual void OnSelectionDoubleCliked(IGameGUIList *piControl,int nElement,std::string sElement)=0;

	 virtual ~IGameGUIListEvents(){}
};

class IGameGUIMessageDialog: virtual public ISystemUnknown
{
public:

	virtual void ShowMessage(IGameWindow *piParent,std::string sText,std::string sTitle,EMessageDialogType eType)=0;
};

class IGameGUIInputDialog: virtual public ISystemUnknown
{
public:

	virtual bool ShowInput(IGameWindow *piParent,std::string *psText,std::string sTitle)=0;
};

class IGameGUIConfirmDialog: virtual public ISystemUnknown
{
public:

	virtual bool Confirm(IGameWindow *piParent,std::string sText,std::string sTitle,EMessageDialogType eType)=0;
};

class IGameGUIColorDialogCallback
{
	public:
	virtual void OnColorChanged(CVector vColor)=0;
};

class IGameGUIColorDialog: virtual public ISystemUnknown
{
public:

	virtual bool SelectColor(IGameWindow *piParent,std::string sTitle,CVector *pvColor)=0;
	virtual bool SelectColor(IGameWindow *piParent,IGameGUIColorDialogCallback *piCallback,std::string sTitle,CVector *pvColor)=0;
};


class IGameGUIFileDialog: virtual public ISystemUnknown
{
public:

	virtual bool OpenFile(IGameWindow *piParent,std::string sTitle,const char *psFilter,std::string *psFile)=0;
	virtual bool SaveFile(IGameWindow *piParent,std::string sTitle,const char *psFilter,std::string *psFile,bool bOverWriteWarn)=0;
};

#endif
