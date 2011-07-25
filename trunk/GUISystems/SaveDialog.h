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

class CSaveDialog: virtual public CGameDialogBase,virtual public IGameGUIButtonEvents,virtual public ISavedGameRowEvents, virtual public ISaveDialog
{
	int m_nSelectedGame;
	
	ISavedGameRow *m_piRows[MAX_SAVEDGAMES];
	IGameGUIButton *m_piBTCancel;
	
	std::vector<SGameState> *m_pvSavedGames;
	
	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY_EX("Row0",m_piRows[0],ISavedGameRowEvents);
		CHILD_MAP_ENTRY_EX("Row1",m_piRows[1],ISavedGameRowEvents);
		CHILD_MAP_ENTRY_EX("Row2",m_piRows[2],ISavedGameRowEvents);
		CHILD_MAP_ENTRY_EX("Row3",m_piRows[3],ISavedGameRowEvents);
		CHILD_MAP_ENTRY_EX("Row4",m_piRows[4],ISavedGameRowEvents);
		CHILD_MAP_ENTRY_EX("Cancel",m_piBTCancel,IGameGUIButtonEvents);
	END_CHILD_MAP()

	void UpdateGUI();
	
public:
	void OnInitDialog();
	void OnEndDialog();
	
	bool SaveGame(IGameWindow *piParent,SGameState *pCurrent,std::vector<SGameState> *pvSavedGames);
	
	// IGameButtonEvents
	void OnButtonClicked(IGameGUIButton *piControl);
	
	// ISavedGameRowEvents
	void OnSavedGameSelected(ISavedGameRow *piControl);
	

	CSaveDialog(void);
	~CSaveDialog(void);
};
