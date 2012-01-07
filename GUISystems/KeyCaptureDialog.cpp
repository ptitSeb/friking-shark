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
#include "KeyCaptureDialog.h"

CKeyCaptureDialog::CKeyCaptureDialog(void)
{
	m_bCapturingJoystickButton=false;
	m_nKey=0;
}

CKeyCaptureDialog::~CKeyCaptureDialog(void)
{
}

void CKeyCaptureDialog::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	if(m_piSTText){m_piSTText->SetText(m_bCapturingJoystickButton?"Press the joystick button now...":"Press the key now...");}
	if(m_piSTCancelText)
	{
		std::string sKeyName;
		std::string sText="(";
		SGameGUIAdditionalNavigationControls sControls;
		m_piGUIManager->GetAdditionalNavigationControls(&sControls);
		m_piGUIManager->GetKeyName(GK_ESCAPE,&sKeyName);
		sText+=sKeyName;
		
		if(sControls.nKeyboardCancel)
		{
			m_piGUIManager->GetKeyName(sControls.nKeyboardCancel,&sKeyName);
			sText+=" or ";
			sText+=sKeyName;
		}
		if(sControls.nJoystickCancel)
		{
			m_piGUIManager->GetKeyName(sControls.nJoystickCancel,&sKeyName);
			sText+=" or ";
			sText+=sKeyName;
		}
		
		sText+=" to cancel)";
		m_piSTCancelText->SetText(sText);
	}
}
	
void CKeyCaptureDialog::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(m_piGUIManager->IsNavigationControl(eGameGUINavigationControl_Cancel,nKey))
	{
		CGameDialogBase::OnKeyDown(nKey,pbProcessed);
		return;	
	}
	bool bJoyButton=(nKey>=GK_JOY_BUTTON_FIRST && nKey<=GK_JOY_BUTTON_LAST);
	bool bJoyKey=(nKey>=GK_JOY_FIRST && nKey<=GK_JOY_LAST);
	if((m_bCapturingJoystickButton && bJoyButton) ||
	   (!m_bCapturingJoystickButton && !bJoyKey))
	{
		*pbProcessed=true;
		m_nKey=nKey;
		EndDialog(DIALOG_OK);
	}
}

bool CKeyCaptureDialog::CaptureKey(IGameWindow *piParent,unsigned int *pKey)
{
	m_nKey=0;
	Execute(piParent);
	*pKey=m_nKey;
	return m_nKey!=0;
}

bool CKeyCaptureDialog::CaptureJoystickButton(IGameWindow *piParent,unsigned int *pButton)
{
	m_bCapturingJoystickButton=true;
	m_nKey=0;
	Execute(piParent);
	*pButton=m_nKey;
	m_bCapturingJoystickButton=false;
	return m_nKey!=0;
}