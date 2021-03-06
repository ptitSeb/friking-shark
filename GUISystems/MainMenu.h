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

class CMainMenu: virtual public CGameDialogBase,virtual public IMainMenu,virtual public IGameGUIButtonEvents
{
	bool m_bAllowLoad;
	bool m_bAllowContinue;
	
	IGameGUIButton *m_piBTNewGame;
	IGameGUIButton *m_piBTLoad;
	IGameGUIButton *m_piBTContinue;
	IGameGUIButton *m_piBTExit;
	IGameGUIButton *m_piBTOptions;
	IGameGUIButton *m_piBTHighScores;
	IGameGUIButton *m_piBTCredits;
	IGameWindow    *m_piLastFocusedWindow;
	
	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY_EX("NewGame",m_piBTNewGame,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Continue",m_piBTContinue,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Load",m_piBTLoad,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Options",m_piBTOptions,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("HighScores",m_piBTHighScores,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Credits",m_piBTCredits,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Exit",m_piBTExit,IGameGUIButtonEvents);
	END_CHILD_MAP()

	void OnKeyDown(int nKey,bool *pbProcessed);
	
public:
	void OnInitDialog();
	void OnEndDialog();
	void Destroy();
	
	// IMainMenu
	
	eMainMenuAction Show(IGameWindow *piParent,bool bAllowContinue, bool bAllowLoad);
	
	// IGameButtonEvents

	void OnButtonClicked(IGameGUIButton *piControl);

	 CMainMenu(void);
	~CMainMenu(void);
};
