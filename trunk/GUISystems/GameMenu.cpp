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


#include "./stdafx.h"
#include "GameRunTimeLib.h"
#include "GameGUILib.h"
#include "GameMenu.h"

CGameMenu::CGameMenu(void)
{
	m_piBTContinue=NULL;
	m_piBTEndGame=NULL;
}

CGameMenu::~CGameMenu(void)
{
}

void CGameMenu::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTContinue)
	{
		EndDialog(eGameMenuAction_Continue);
	}
	if(piControl==m_piBTEndGame)
	{
		EndDialog(eGameMenuAction_EndGame);
	}
	if(piControl==m_piBTControls)
	{
		EndDialog(eGameMenuAction_Controls);
	}
}
void CGameMenu::OnInitDialog()
{
}
