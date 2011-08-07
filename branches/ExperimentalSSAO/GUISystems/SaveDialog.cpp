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
#include "SaveDialog.h"

CSaveDialog::CSaveDialog(void)
{
	m_nSelectedGame=-1;
	m_pvSavedGames=NULL;
}

CSaveDialog::~CSaveDialog(void)
{
}

void CSaveDialog::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTCancel)
	{
		EndDialog(DIALOG_CANCEL);
	}
}

void CSaveDialog::OnSavedGameSelected(ISavedGameRow *piControl)
{
	if(m_pvSavedGames)
	{
		for(unsigned int x=0;x<MAX_SAVEDGAMES;x++)
		{
			if(m_piRows[x]==piControl)
			{
				if(x<m_pvSavedGames->size())
				{
					if(ConfirmDialog("Overwrite game?","Friking shark",eMessageDialogType_Warning))
					{
						m_nSelectedGame=x;
						EndDialog(DIALOG_OK);
						return;
					}
				}
				else
				{
					m_nSelectedGame=x;
					EndDialog(DIALOG_OK);
					return;
				}
			}
		}
	}
	UpdateGUI();
}

void CSaveDialog::UpdateGUI()
{
	for(unsigned int x=0;x<MAX_SAVEDGAMES;x++)
	{
		bool bExistsSaveGame=(m_pvSavedGames && x<m_pvSavedGames->size());
		m_piRows[x]->SetSavedGame(bExistsSaveGame?&(*m_pvSavedGames)[x]:NULL);
		// if there are less savegames than rows, let visible the last one to add a new one
		m_piRows[x]->Show(x<=m_pvSavedGames->size());
	}
}

void CSaveDialog::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	UpdateGUI();
}

void CSaveDialog::OnEndDialog()
{
	for(unsigned int x=0;x<MAX_SAVEDGAMES;x++)
	{
		m_piRows[x]->SetSavedGame(NULL);
	}
	CGameDialogBase::OnEndDialog();
}

bool CSaveDialog::SaveGame(IGameWindow *piParent,SGameState *pCurrent,std::vector<SGameState> *pvSavedGames)
{
	m_pvSavedGames=pvSavedGames;
	m_nSelectedGame=-1;
	int nRes=Execute(piParent);
	if(nRes==DIALOG_OK)
	{
		if(m_nSelectedGame<(int)m_pvSavedGames->size())
		{
			(*m_pvSavedGames)[m_nSelectedGame]=*pCurrent;
		}
		else
		{
			m_pvSavedGames->push_back(*pCurrent);
		}
	}
	m_pvSavedGames=NULL;
	m_nSelectedGame=0;
	return nRes==DIALOG_OK;
}
