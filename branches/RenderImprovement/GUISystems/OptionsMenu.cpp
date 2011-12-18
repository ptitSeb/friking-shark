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
#include "OptionsMenu.h"

COptionsMenu::COptionsMenu(void)
{
	m_piLastFocusedWindow=NULL;
}

COptionsMenu::~COptionsMenu(void)
{
}

void COptionsMenu::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_ESCAPE)
	{
		m_piBTBack->PlayClickSound();
		EndDialog(eOptionsMenuAction_Back);
		*pbProcessed=true;
		return;
	}
	if(nKey==GK_RETURN){return;}
	CGameDialogBase::OnKeyDown(nKey,pbProcessed);
}

void COptionsMenu::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTControls)
	{
		EndDialog(eOptionsMenuAction_Controls);
	}
	if(piControl==m_piBTVideo)
	{
		EndDialog(eOptionsMenuAction_Video);
	}
	if(piControl==m_piBTAudio)
	{
		EndDialog(eOptionsMenuAction_Audio);
	}
	if(piControl==m_piBTBack)
	{
		EndDialog(eOptionsMenuAction_Back);
	}
}
void COptionsMenu::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	
	if(m_piLastFocusedWindow && m_piLastFocusedWindow->IsActive())
	{
		IGameGUIButton *piButton=QI(IGameGUIButton,m_piLastFocusedWindow);
		if(piButton){piButton->DisableSounds();}
		m_piGUIManager->SetFocus(m_piLastFocusedWindow);
		if(piButton){piButton->EnableSounds();}
		REL(piButton);
	}
	else if(m_piBTControls)
	{
		m_piBTControls->DisableSounds();
		m_piGUIManager->SetFocus(m_piBTControls);
		m_piBTControls->EnableSounds();
	}
}

void COptionsMenu::OnEndDialog()
{
	REL(m_piLastFocusedWindow);
	m_piLastFocusedWindow=GetFocusedDescendant();
	if(m_piLastFocusedWindow==m_piBTBack){REL(m_piLastFocusedWindow);}
	CGameDialogBase::OnEndDialog();
}

void COptionsMenu::Destroy()
{
	REL(m_piLastFocusedWindow);
	CGameDialogBase::Destroy();
}

eOptionsMenuAction COptionsMenu::Show(IGameWindow *piParent)
{
	int nRes=Execute(piParent);
	return (eOptionsMenuAction)nRes;
}
