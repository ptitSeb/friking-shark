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
#include "GameGUIMessageDialog.h"

CGameGUIMessageDialog::CGameGUIMessageDialog(void)
{
	m_eType=eMessageDialogType_Info;
	InitializeChildren();
}

CGameGUIMessageDialog::~CGameGUIMessageDialog(void)
{
}

void CGameGUIMessageDialog::OnInitDialog()
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
}

void CGameGUIMessageDialog::OnEndDialog()
{
	CGameDialogBase::OnEndDialog();
}

void CGameGUIMessageDialog::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTAccept){EndDialog(DIALOG_OK);}
}

void CGameGUIMessageDialog::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_ESCAPE){EndDialog(DIALOG_OK);*pbProcessed=true;}
	if(nKey==GK_RETURN){EndDialog(DIALOG_OK);*pbProcessed=true;}
}

void CGameGUIMessageDialog::ShowMessage(IGameWindow *piParent,std::string sText,std::string sTitle,EMessageDialogType eType)
{	
	m_eType=eType;
	m_sText=sText;
	m_sTitle=sTitle;
	Execute(piParent);
}




