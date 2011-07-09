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
#include "GUISystems.h"
#include "LevelOptions.h"

CLevelOptions::CLevelOptions(void)
{
	m_eMode=eGameMode_Normal;
	m_nSelectedLevel=0;
}

CLevelOptions::~CLevelOptions(void)
{
}

void CLevelOptions::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTLevel1)
	{
		m_nSelectedLevel=0;
		EndDialog(0);
	}
	if(piControl==m_piBTLevel2)
	{
		m_nSelectedLevel=1;
		EndDialog(0);
	}
	if(piControl==m_piBTLevel3)
	{
		m_nSelectedLevel=2;
		EndDialog(0);
	}	
	if(piControl==m_piBTLevel4)
	{
		m_nSelectedLevel=3;
		EndDialog(0);
	}
	if(piControl==m_piBTLevel5)
	{
		m_nSelectedLevel=4;
		EndDialog(0);
	}
	if(piControl==m_piBTMode)
	{
		unsigned int nMode=m_eMode;
		nMode++;
		if(nMode>=eGameMode_Count){nMode=eGameMode_Normal;}
		m_eMode=(EGameMode)nMode;
		UpdateGUI();
	}
}

void CLevelOptions::UpdateGUI()
{
	if(m_eMode==eGameMode_Normal){m_piBTMode->SetText("Mode: Normal");}
	if(m_eMode==eGameMode_God){m_piBTMode->SetText("Mode: God");}
	if(m_eMode==eGameMode_InfiniteLives){m_piBTMode->SetText("Mode: Infinite Lives");}
}

void CLevelOptions::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	UpdateGUI();
}

void CLevelOptions::SelectOptions(IGameWindow *piParent,EGameMode *pMode,unsigned int *pnSelectedLevel)
{
	m_eMode=*pMode;
	m_nSelectedLevel=*pnSelectedLevel;
	Execute(piParent);
	*pMode=m_eMode;
	*pnSelectedLevel=m_nSelectedLevel;
}
