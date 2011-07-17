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

class CLevelOptions: virtual public CGameDialogBase,virtual public IGameGUIButtonEvents, virtual public ILevelOptions
{
	EGameMode m_eMode;
	EGameDifficulty m_eDifficulty;
	
	unsigned int m_nSelectedLevel;
	
	IGameGUIButton *m_piBTLevel1;
	IGameGUIButton *m_piBTLevel2;
	IGameGUIButton *m_piBTLevel3;
	IGameGUIButton *m_piBTLevel4;
	IGameGUIButton *m_piBTLevel5;
	IGameGUIButton *m_piBTMode;
	IGameGUIButton *m_piBTDifficulty;
	
	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY_EX("Level1",m_piBTLevel1,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Level2",m_piBTLevel2,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Level3",m_piBTLevel3,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Level4",m_piBTLevel4,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Level5",m_piBTLevel5,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Difficulty",m_piBTDifficulty,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Mode",m_piBTMode,IGameGUIButtonEvents);
	END_CHILD_MAP()

	void UpdateGUI();
	
public:
	void OnInitDialog();

	void SelectOptions(IGameWindow *piParent,EGameMode *pMode,EGameDifficulty *pDifficulty,unsigned int *pnSelectedLevel);
	
	// IGameButtonEvents

	void OnButtonClicked(IGameGUIButton *piControl);

	CLevelOptions(void);
	~CLevelOptions(void);
};
