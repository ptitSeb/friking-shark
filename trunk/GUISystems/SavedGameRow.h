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

#define MAX_SAVEDGAMES 5

class CSavedGameRow: virtual public CGameWindowBase, virtual public ISavedGameRow, virtual public IGameGUIButtonEvents
{
	SGameState     *m_pSavedGame;
	IGameGUIButton *m_piBTButton;
	IGameGUIButton *m_piBTEmpty;
	IGameGUILabel  *m_piSTLevel;
	IGameGUILabel  *m_piSTLives;
	IGameGUILabel  *m_piSTWeapon;
	IGameGUILabel  *m_piSTBombs;
	IGameGUILabel  *m_piSTPoints;
	IGameGUILabel  *m_piSTMode;
	IGameGUILabel  *m_piSTDifficulty;
	
	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY_EX("Button",m_piBTButton,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Empty",m_piBTEmpty,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY("Level",m_piSTLevel);
		CHILD_MAP_ENTRY("Lives",m_piSTLives);
		CHILD_MAP_ENTRY("Weapon",m_piSTWeapon);
		CHILD_MAP_ENTRY("Bombs",m_piSTBombs);
		CHILD_MAP_ENTRY("Points",m_piSTPoints);
		CHILD_MAP_ENTRY("Mode",m_piSTMode);
		CHILD_MAP_ENTRY("Difficulty",m_piSTDifficulty);
	END_CHILD_MAP()

	void UpdateGUI();
	
public:
	void OnButtonClicked(IGameGUIButton *piControl);
	
	void SetSavedGame(SGameState *pGame);
	
	CSavedGameRow(void);
	~CSavedGameRow(void);
};
