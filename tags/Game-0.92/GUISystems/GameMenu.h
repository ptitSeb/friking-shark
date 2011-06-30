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

class CGameMenu: virtual public CGameDialogBase,public IGameGUIButtonEvents
{
	IGameGUIButton *m_piBTContinue;
	IGameGUIButton *m_piBTEndGame;

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY_EX("Continue",m_piBTContinue,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EndGame",m_piBTEndGame,IGameGUIButtonEvents);
	END_CHILD_MAP()

public:
	void OnInitDialog();

	// IGameButtonEvents

	void OnButtonClicked(IGameGUIButton *piControl);

	CGameMenu(void);
	~CGameMenu(void);
};
