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


class CVideoOptions: virtual public CGameDialogBase,virtual public IGameGUIButtonEvents, virtual public IVideoOptions
{
	IGameGUIButton *m_piBTFullScreen;
	IGameGUIButton *m_piBTVerticalSync;
	IGameGUIButton *m_piBTShadows;
	IGameGUIButton *m_piBTRender;
	IGameGUIButton *m_piBTOk;

	int                      m_nCurrentRenderPath;
	std::vector<std::string> m_vRenderPaths;
	CGenericRenderWrapper    m_Render;
	CGameInterfaceWrapper    m_GameInterface;
	
	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY_EX("FullScreen",m_piBTFullScreen,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("VerticalSync",m_piBTVerticalSync,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Shadows",m_piBTShadows,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Render",m_piBTRender,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Ok",m_piBTOk,IGameGUIButtonEvents);
	END_CHILD_MAP()


	void OnDraw(IGenericRender *piRender);
	void UpdateGUI();
	
public:
	
	void OnInitDialog();
	void OnEndDialog();
	void OnKeyDown(int nKey,bool *pbProcessed);
	
	bool Show(IGameWindow *piParent);
	
	// IGameButtonEvents

	void OnButtonClicked(IGameGUIButton *piControl);
	
	CVideoOptions(void);
	~CVideoOptions(void);
};
