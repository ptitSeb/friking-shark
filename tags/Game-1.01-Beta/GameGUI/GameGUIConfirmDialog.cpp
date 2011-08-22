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
#include "GameGUILib.h"
#include "GameGUIConfirmDialog.h"

CGameGUIConfirmDialog::CGameGUIConfirmDialog(void)
{
	m_eType=eMessageDialogType_Info;
	InitializeChildren();
}

CGameGUIConfirmDialog::~CGameGUIConfirmDialog(void)
{
}

void CGameGUIConfirmDialog::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	if(m_piSTTitle)
	{
	  CVector vColor;
	  double dAlpha=0;
	  m_piSTTitle->GetBackgroundColor(&vColor,&dAlpha);
	  switch(m_eType)
	  {
		case eMessageDialogType_Warning:m_piSTTitle->SetBackgroundColor(m_vWarningBkColor,dAlpha);break;
		case eMessageDialogType_Error:m_piSTTitle->SetBackgroundColor(m_vErrorBkColor,dAlpha);break;
		case eMessageDialogType_Question:m_piSTTitle->SetBackgroundColor(m_vQuestionBKColor,dAlpha);break;
		case eMessageDialogType_Info:m_piSTTitle->SetBackgroundColor(m_vInfoBkColor,dAlpha);break;
		default:m_piSTTitle->SetBackgroundColor(m_vInfoBkColor,dAlpha);break;
	  };
	  m_piSTTitle->SetText(m_sTitle);
	}
	if(m_piSTText){m_piSTText->SetText(m_sText);}	
	if(m_piBTYes && m_bNavigateChildren)
	{
		bool bWantFocus=false;
		m_piBTYes->OnWantFocus(&bWantFocus);
		if(bWantFocus)
		{
			m_piBTYes->DisableSounds();
			m_piGUIManager->SetFocus(m_piBTYes);
			m_piBTYes->EnableSounds();
		}
	}
}

void CGameGUIConfirmDialog::OnEndDialog()
{
	CGameDialogBase::OnEndDialog();
}

void CGameGUIConfirmDialog::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTYes){EndDialog(DIALOG_OK);}
	if(piControl==m_piBTNo){EndDialog(DIALOG_CANCEL);}
}

void CGameGUIConfirmDialog::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_ESCAPE){EndDialog(DIALOG_CANCEL);if(m_piBTNo){m_piBTNo->PlayClickSound();}*pbProcessed=true;return;}
	if(nKey==GK_RETURN && !m_bNavigateChildren){if(m_piBTYes){m_piBTYes->PlayClickSound();}EndDialog(DIALOG_OK);*pbProcessed=true;return;}
	CGameDialogBase::OnKeyDown(nKey,pbProcessed);
}

bool CGameGUIConfirmDialog::Confirm(IGameWindow *piParent,std::string sText,std::string sTitle,EMessageDialogType eType)
{	
	m_eType=eType;
	m_sText=sText;
	m_sTitle=sTitle;
	return Execute(piParent)==DIALOG_OK;
}




