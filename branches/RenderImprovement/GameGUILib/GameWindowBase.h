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


#pragma once

#define CMEF_MANDATORY	0x0000
#define CMEF_OPTIONAL		0x0001

#define BEGIN_CHILD_MAP() void InitializeChildren(){MapChildren(false,true);}bool MapChildren(bool bMapping/*false=Unmapping*/,bool bInitialize){bool bResult=true;
#define CHILD_MAP_ENTRY(_text,_interface) CHILD_MAP_ENTRY_FLAGS(_text,_interface,CMEF_MANDATORY)
#define CHILD_MAP_ENTRY_FLAGS(_text,_interface,_flags) if(bInitialize){_interface=NULL;}else if(!bMapping){REL(_interface);}else{if(bResult){IGameWindow *piControl=FindChild(_text);if(piControl==NULL){if((_flags&CMEF_OPTIONAL)==0){	bResult=false;}}else{_interface=__make_qi(piControl,_interface,__FILE__,__LINE__);REL(piControl);if(_interface==NULL){if((_flags&CMEF_OPTIONAL)==0){bResult=false;}}}if(!bResult){RTTRACE("Failed to map child window %s, interface %s",_text,#_interface);}}}
#define CHILD_MAP_ENTRY_EX(_text,_interface,_eventInterface) CHILD_MAP_ENTRY_EX_FLAGS(_text,_interface,_eventInterface,CMEF_MANDATORY)
#define CHILD_MAP_ENTRY_EX_FLAGS(_text,_interface,_eventInterface,_flags) if(bInitialize){_interface=NULL;}else if(!bMapping){if(_interface){IPublisher *piPublisher=NULL;	piPublisher=dynamic_cast<IPublisher*>(_interface);	if(piPublisher){UNSUBSCRIBE_FROM_PUBLISHER(piPublisher);}}REL(_interface);}else{if(bResult){IGameWindow *piControl=FindChild(_text);if(piControl==NULL){if((_flags&CMEF_OPTIONAL)==0){bResult=false;}}else{_interface=__make_qi(piControl,_interface,__FILE__,__LINE__);REL(piControl);if(_interface==NULL){if((_flags&CMEF_OPTIONAL)==0){bResult=false;}}else{IPublisher *piPublisher=NULL;piPublisher=dynamic_cast<IPublisher*>(_interface);if(piPublisher==NULL){REL(_interface);if((_flags&CMEF_OPTIONAL)==0){bResult=false;}}else{if(SUBSCRIBE_TO(piPublisher,_eventInterface)==false){REL(_interface);if((_flags&CMEF_OPTIONAL)==0){bResult=false;}}}}}if(!bResult){RTTRACE("Failed to map child window %s, interface %s",_text,#_interface);}}}
#define END_CHILD_MAP() return bResult;}


DECLARE_CUSTOM_WRAPPER1(CGameGUIManagerWrapper,IGameGUIManager,m_piInterface)
DECLARE_CUSTOM_WRAPPER1(CGameWindowWrapper,IGameWindow,m_piWindow)
DECLARE_CUSTOM_WRAPPER1(CGenericTextureWrapper,IGenericTexture,m_piTexture)
DECLARE_CUSTOM_WRAPPER1(CGenericFontWrapper,IGenericFont,m_piFont)
DECLARE_CUSTOM_WRAPPER1(CGenericModelWrapper,IGenericModel,m_piModel)
DECLARE_CUSTOM_WRAPPER1(CGenericCameraWrapper,IGenericCamera,m_piCamera)

DECLARE_CUSTOM_WRAPPER1(CMessageDialogWrapper,IGameGUIMessageDialog,m_piMessageDialog)
DECLARE_CUSTOM_WRAPPER1(CConfirmDialogWrapper,IGameGUIConfirmDialog,m_piConfirmDialog)
DECLARE_CUSTOM_WRAPPER1(CColorDialogWrapper,IGameGUIColorDialog,m_piColorDialog)
DECLARE_CUSTOM_WRAPPER1(CFileDialogWrapper,IGameGUIFileDialog,m_piFileDialog)
DECLARE_CUSTOM_WRAPPER1(CInputDialogWrapper,IGameGUIInputDialog,m_piInputDialog)

enum EFocusableSearchDirection
{
	eFocusableSearchRight,
	eFocusableSearchLeft,
	eFocusableSearchUp,
	eFocusableSearchDown
};

class CGameWindowBase :	virtual public CSystemObjectBase, virtual public IGameWindow
{
protected:
	
	std::vector<IGameWindow*> m_vChildren;
	std::vector<IGameWindow*> m_vChildrenZOrder;
	std::vector<CGameWindowWrapper> m_vLoadedChildrenList;

	CVector		m_vBackgroundColor;
	double		m_dBackgroundAlpha;
	
	CGenericTextureWrapper m_MouseCursorTexture;
	CGenericTextureWrapper m_BackgroundTexture;
	CGenericModelWrapper   m_BackgroundModel;
	SGameRect              m_rBackgroundTextureCoords;
	CGenericFontWrapper    m_Font;

	bool					m_bCentered;
	bool					m_bVisible;
	bool					m_bActive;
	bool					m_bPopup;
	std::string				m_sWindowName;
	
	eGameGUIReferenceSystem m_eReferenceSystem;
	eGameGUIChildrenLayout  m_eChildrenLayout;
	double 					m_dSizeInLayout;
	double 					m_dLayoutMargin; // Desde los bordes de esta ventana al los hijos
	double 					m_dLayoutSeparation; // Entre los hijos

	bool m_bNavigateChildren;

protected:

	IGameWindow		*m_piParent;

	std::string			m_sFontName;
	double				m_dFontSize;

	SGameRect			m_rRect;
	SGameRect			m_rRealRect;
	SGameSize           m_sMargin;

	IGameGUIManager *m_piGUIManager;

	virtual bool MapChildren(bool bMapping/*false=Unmapping*/,bool bInitialize);

	bool DetectDrag(double dx,double dy);

	void UpdateChildrenRealRects();

	CLine GetMouseRay(double x,double y,double dLength,IGenericCamera *piCamera);

	void 		 GetFocusableDescendants(IGameWindow *piParent,std::vector<IGameWindow *> *pvFocusableWindows);
	IGameWindow *GetFocusableAncestor();
	IGameWindow *GetFocusedDescendant();
	
	IGameWindow *FindNextFocusableWindow(IGameWindow *pReference);
	IGameWindow *FindPreviousFocusableWindow(IGameWindow *pReference);
	IGameWindow *FindClosestFocusableWindow(IGameWindow *pReference,EFocusableSearchDirection eDirection);
	
	void ComputeClipRect(SGameRect *pRect);
	
public:

	BEGIN_PROP_MAP(CGameWindowBase)
		PROP_VALUE_FLAGS(m_bNavigateChildren,"NavigateChildren",false,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_rRect,"Position",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_vLoadedChildrenList,"Children",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_Font,"Font",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_sMargin,"Margin",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dFontSize,"FontSize",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bCentered,"Centered",false,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_sWindowName,"Name","",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_eChildrenLayout,"ChildrenLayout",eGameGUIChildrenLayout_None,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dSizeInLayout,"SizeInLayout",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dLayoutMargin,"LayoutMargin",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dLayoutSeparation,"LayoutSeparation",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_eReferenceSystem,"ReferenceSystem",eGameGUIReferenceSystem_Absolute,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dBackgroundAlpha,"BkAlpha",1.0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vBackgroundColor,"BkColor",CVector(0.5,0.5,0.5),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_rBackgroundTextureCoords,"TextureCoords",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_BackgroundTexture,"Texture",MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_FLAGS(m_BackgroundModel,"Model",MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_FLAGS(m_MouseCursorTexture,"MouseCursorTexture",MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP()

	 bool		InitWindow(IGameWindow *piParent,bool bPopup);
	 void		DestroyWindow();
	 void   	Destroy();

	 void		 SetWindowName(std::string sName);
	 std::string GetWindowName();

	 IGameGUIManager *GetGUIManager();

	 bool		IsPopup();

	 void		Show(bool bShow);
	 void		Activate(bool bActivate);
	 bool		IsVisible();
	 bool		IsActive();

	 void		GetRect(SGameRect *pRect);
	 void		SetRect(SGameRect *pRect);

	 void		SetFont(IGenericFont *piFont,double dSize);
	 void		GetFont(IGenericFont **ppiFont,double *pdSize);

	 void		UpdateRealRect();
	 void		GetRealRect(SGameRect *pRect);
	 void  		SetSizeInLayout(double dSize);
	 double 	GetSizeInLayout();

	 void		SetReferenceSystem(eGameGUIReferenceSystem eRefSystem);
	 eGameGUIReferenceSystem GetReferenceSystem();

	 IGameWindow *GetParent();
	 void		 GetChildren(std::vector<IGameWindow *> *pvChildren);

	 void					 AddChild(IGameWindow *);
	 void					 RemoveChild(IGameWindow *);
	 void					 EnumerateChildren(IGameWindowEnumerationCallback *piCallback);
	 IGameWindow  *FindChild(std::string sName);

	 void		 BringToFront();
	 void		 BringChildToFront(IGameWindow *);

	 IGenericTexture *GetBackgroundTexture();
	 void			SetBackgroundTexture(IGenericTexture *piTexture);

	 IGenericModel *GetBackgroundModel();
	 void			SetBackgroundModel(IGenericModel *piModel);

	 void	GetBackgroundColor(CVector *pvColor,double *pdAlpha);
	 void	SetBackgroundColor(CVector vColor,double dAlpha);

	// Events received from the manager

	 void OnCharacter(int nCharacter,bool *pbProcessed);
	 void OnKeyDown(int nKey,bool *pbProcessed);
	 void OnKeyUp(int nKey,bool *pbProcessed);

	 void OnMouseDown(int nButton,double x,double y);
	 void OnMouseDoubleClick(int nButton,double x,double y);
	 void OnMouseUp(int nButton,double x,double y);
	 void OnMouseMove(double x,double y);
	 void OnMouseWheelUp(double x,double y);
	 void OnMouseWheelDown(double x,double y);

	 void OnDrawMouseCursor(SGamePos position,IGenericRender *piRender,bool *pbDrawed);
	 void OnDrawBackground(IGenericRender *piRender);
	 void OnDraw(IGenericRender *piRender);

	 void OnSetFocus();
	 void OnKillFocus(IGameWindow *piFocusedWindow);
	 void OnWantFocus(bool *pbWant);
	 void OnReleaseMouseCapture();

	 bool OpenFileDialog(std::string sTitle,const char *psFilter,std::string *psFile);
	 bool SaveFileDialog(std::string sTitle,const char *psFilter,std::string *psFile,bool bOverWriteWarn);
	 bool SelectColorDialog(std::string sTitle,CVector *pvColor);
	 bool SelectColorDialog(std::string sTitle,IGameGUIColorDialogCallback *piCallBack,CVector *pvColor);
	 bool ConfirmDialog(std::string sText,std::string sTitle,EMessageDialogType eType);
	 void MessageDialog(std::string sText,std::string sTitle,EMessageDialogType eType);
	 bool InputDialog(std::string *psText,std::string sTitle);

	CGameWindowBase(void);
	~CGameWindowBase(void);
};
