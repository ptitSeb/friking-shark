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
#include "GameWindowBase.h"
#include "GameDialogBase.h"

CGameDialogBase::CGameDialogBase(void)
{
	m_bExecuting=false;
	m_nDialogResult=DIALOG_OK;
}

CGameDialogBase::~CGameDialogBase(void)
{
}

int	CGameDialogBase::Execute(IGameWindow *piParent)
{
	IGameGUIManager *piManager=piParent?piParent->GetGUIManager():NULL;
	IGameWindow *piOldFocus=piManager?piManager->GetFocusedWindow():NULL;
	
	int nResult=DIALOG_CANCEL;
	
	if(CGameWindowBase::InitWindow(piParent,true))
	{
		m_piGUIManager->ReleaseMouseCapture();
		m_piGUIManager->SetFocus(this);
		
		OnInitDialog();
		Show(true);
		m_piGUIManager->EnterGUILoop();
		Show(false);
		OnEndDialog();
		
		CGameWindowBase::DestroyWindow();
		nResult=m_nDialogResult;
	}
	
	if(piManager)
	{
		IGameGUIButton *piButton=QI(IGameGUIButton,piOldFocus);
		bool bSoundsEnabled=piButton?piButton->AreSoundsEnabled():false;
		if(piButton){piButton->DisableSounds();}
		piManager->SetFocus(piOldFocus);
		if(piButton && bSoundsEnabled){piButton->EnableSounds();}
		REL(piButton);
	}
	REL(piOldFocus);
	REL(piManager);
	return nResult;
}

void CGameDialogBase::EndDialog(int nResult)
{
	m_nDialogResult=nResult;
	m_piGUIManager->ExitGUILoop();
}

void CGameDialogBase::OnInitDialog()
{
}

void CGameDialogBase::OnEndDialog()
{
}

void CGameDialogBase::OnWantFocus(bool *pbWant){*pbWant=true;}

void CGameDialogBase::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(m_piGUIManager->IsNavigationControl(eGameGUINavigationControl_Cancel,nKey))
	{
		EndDialog(DIALOG_CANCEL);
		(*pbProcessed)=true;
	}
	else if(m_piGUIManager->IsNavigationControl(eGameGUINavigationControl_Accept,nKey))
	{
		EndDialog(DIALOG_OK);
		(*pbProcessed)=true;
	}
	else
	{
		CGameWindowBase::OnKeyDown(nKey,pbProcessed);
	}
}
