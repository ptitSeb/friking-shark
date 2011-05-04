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
#include "GameGUIInputDialog.h"

CGameGUIInputDialog::CGameGUIInputDialog(void)
{
	InitializeChildren();
}

CGameGUIInputDialog::~CGameGUIInputDialog(void)
{
}

void CGameGUIInputDialog::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	if(m_piSTTitle){m_piSTTitle->SetText(m_sTitle);}
	if(m_piEDText){m_piEDText->SetText(m_sText);}
	if(m_piEDText){m_piGUIManager->SetFocus(m_piEDText);}
}

void CGameGUIInputDialog::OnEndDialog()
{
	CGameDialogBase::OnEndDialog();
}

void CGameGUIInputDialog::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTAccept)
	{
		if(m_piEDText){m_sText=m_piEDText->GetText();}
		EndDialog(DIALOG_OK);
	}
	if(piControl==m_piBTCancel)
	{
		EndDialog(DIALOG_CANCEL);
	}
}

void CGameGUIInputDialog::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_ESCAPE){EndDialog(DIALOG_CANCEL);*pbProcessed=true;}
	if(nKey==GK_RETURN)
	{
		if(m_piEDText){m_sText=m_piEDText->GetText();}
		EndDialog(DIALOG_OK);
		*pbProcessed=true;
	}
}

bool CGameGUIInputDialog::ShowInput(IGameWindow *piParent,std::string *psText,std::string sTitle)
{	
	m_sText=*psText;
	m_sTitle=sTitle;
	bool bOk=(Execute(piParent)==DIALOG_OK);
	if(bOk){*psText=m_sText;}
	return bOk;
}




