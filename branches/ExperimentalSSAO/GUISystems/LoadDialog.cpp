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
#include "LoadDialog.h"

CLoadDialog::CLoadDialog(void)
{
	m_nSelectedGame=0;
	m_pvSavedGames=NULL;
}

CLoadDialog::~CLoadDialog(void)
{
}

void CLoadDialog::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTCancel)
	{
		EndDialog(DIALOG_CANCEL);
	}
}

void CLoadDialog::OnSavedGameSelected(ISavedGameRow *piControl)
{
	if(m_pvSavedGames)
	{
		for(unsigned int x=0;x<MAX_SAVEDGAMES && x<m_pvSavedGames->size();x++)
		{
			if(m_piRows[x]==piControl)
			{
				m_nSelectedGame=x;
				EndDialog(DIALOG_OK);
				return;
			}
		}
	}
}

void CLoadDialog::UpdateGUI()
{
	for(unsigned int x=0;x<MAX_SAVEDGAMES;x++)
	{
		bool bExistsSaveGame=(m_pvSavedGames && x<m_pvSavedGames->size());
		m_piRows[x]->Show(bExistsSaveGame);
		m_piRows[x]->SetSavedGame(bExistsSaveGame?&(*m_pvSavedGames)[x]:NULL);
	}
}

void CLoadDialog::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	UpdateGUI();
}

void CLoadDialog::OnEndDialog()
{
	for(unsigned int x=0;x<MAX_SAVEDGAMES;x++)
	{
		m_piRows[x]->SetSavedGame(NULL);
	}
	CGameDialogBase::OnEndDialog();
}

bool CLoadDialog::LoadGame(IGameWindow *piParent,std::vector<SGameState> *pvSavedGames,SGameState *pSelected)
{
	m_pvSavedGames=pvSavedGames;
	m_nSelectedGame=0;
	int nRes=Execute(piParent);
	if(nRes==DIALOG_OK)
	{
		*pSelected=(*m_pvSavedGames)[m_nSelectedGame];
	}
	m_pvSavedGames=NULL;
	m_nSelectedGame=0;
	return nRes==DIALOG_OK;
}
