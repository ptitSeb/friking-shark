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

DECLARE_CUSTOM_WRAPPER1(CWindowWapper,IGameWindow,m_piWindow)
DECLARE_CUSTOM_WRAPPER1(CRenderWrapper,IGenericRender,m_piRender)
DECLARE_CUSTOM_WRAPPER1(CViewportWrapper,IGenericViewport,m_piViewport)

class CGameGUIManager :	virtual public CSystemObjectBase,virtual public IGameGUIManager,virtual public IGenericViewportCallBack
{
	bool m_bSetup;
	
	IGameWindow *m_piFocusedWindow;
	IGameWindow *m_piMouseCaptureWindow;

	bool		m_bShowMouseCursor;
	SGameSize   m_sWindowedResolution;
	SGameScreenProperties m_sScreenProperties;
	std::string m_sUserSettingsFile;
	std::string m_sUserSettingsNode;

	CRenderWrapper m_Render;
	CViewportWrapper m_Viewport;
	CWindowWapper m_MainWindow;
	IGameWindow  *m_piRootWindow;

	std::vector<IGameWindow*> m_vPopups;

	void RenderWindow(IGenericRender *piRender,IGameWindow *piWindow);
	void ProcessMouseActivation(IGameWindow *piWindow);

	IGameWindow *GetWindowFromPos(SGamePos *pPosition,bool bOnlyActive);
	IGameWindow *GetWindowFromPos(IGameWindow *piWindow,SGamePos *pPosition,bool bOnlyActive);

	// IGenericViewportCallBack
	
	bool Setup();
	
	void OnRender();

	void OnLButtonDown(unsigned x,unsigned y);
	void OnLButtonDoubleClick(unsigned x,unsigned y);
	void OnLButtonUp(unsigned x,unsigned y);
	void OnRButtonDown(unsigned x,unsigned y);
	void OnRButtonUp(unsigned x,unsigned y);
	void OnRButtonDoubleClick(unsigned x,unsigned y);
	void OnMouseMove(unsigned x,unsigned y);
	void OnMouseWheelUp(unsigned x,unsigned y);
	void OnMouseWheelDown(unsigned x,unsigned y);
	
	void OnCharacter(unsigned short wCharacter);
	void OnKeyDown(unsigned short wKey);
	void OnKeyUp(unsigned short wKey);
	
	void OnSize(unsigned cx,unsigned cy);
	void OnMove(unsigned x,unsigned y);
	
	void UpdateScreenPlacement();

public:

	BEGIN_PROP_MAP(CGameGUIManager)
		PROP_FLAGS(m_sScreenProperties,"Screen",MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP()

	// IGameGUIManager overloaded.

	bool Init(std::string sClass,std::string sName,ISystem *piSystem);
	void Destroy();

	void EnterGUILoop(); 
	void ExitGUILoop(); 

	void		GetWindowSize(SGameSize *pSize);
	void		GetMousePosition(IGameWindow *piWindow,SGamePos *pPos);
	void		SetMousePosition(IGameWindow *piWindow,SGamePos &pos);

	bool		IsKeyDown(int nKey);
	bool		IsMouseDown(int nMouseButton);
	bool		GetKeyName(int nKey,std::string *psKey);

	void		SetFocus(IGameWindow *piWindow);
	bool		HasFocus(IGameWindow *piWindow);
	IGameWindow *GetFocusedWindow();
	
	void		SetMouseCapture(IGameWindow *piWindow);
	void		ReleaseMouseCapture();
	bool		HasMouseCapture(IGameWindow *piWindow);

	void 		ShowMouseCursor(bool bShow);
	bool 		IsMouseCursorVisible();

	void		RedrawAll();

	IGameWindow *GetMainWindow();

	void		GetScreenProperties(SGameScreenProperties *pProperties);
	void    	SetScreenProperties(SGameScreenProperties *pProperties);

	// Popup management.

	void		 AddPopup(IGameWindow *);
	void		 RemovePopup(IGameWindow *);
	void		 EnumeratePopups(IGameWindowEnumerationCallback *piCallback);
	void		 GetPopups(std::vector<IGameWindow *> *pvPopups);

	bool 		 DetectDrag(double dx,double dy);
	
	void		 RestoreViewport();
	
	CGameGUIManager(void);
	~CGameGUIManager(void);
};
