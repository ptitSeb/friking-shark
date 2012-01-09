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
#include "ControlsDialog.h"

CControlsDialog::CControlsDialog(void)
{
	m_bShowKeyboardControls=true;
	m_dJoystickDeadZone=0;
}

CControlsDialog::~CControlsDialog(void)
{
}

void CControlsDialog::UpdateGUIControls()
{
	if(m_piGUIManager==NULL){return;}
	
	SGameGUIAdditionalNavigationControls controls;
	controls.nKeyboardLeft=(m_LeftKeyMapping.vValidCombinations.size())?m_LeftKeyMapping.vValidCombinations[0].nKey:0;
	controls.nKeyboardRight=(m_RightKeyMapping.vValidCombinations.size())?m_RightKeyMapping.vValidCombinations[0].nKey:0;
	controls.nKeyboardUp=(m_UpKeyMapping.vValidCombinations.size())?m_UpKeyMapping.vValidCombinations[0].nKey:0;
	controls.nKeyboardDown=(m_DownKeyMapping.vValidCombinations.size())?m_DownKeyMapping.vValidCombinations[0].nKey:0;
	controls.nKeyboardAccept=(m_FireKeyMapping.vValidCombinations.size())?m_FireKeyMapping.vValidCombinations[0].nKey:0;
	controls.nKeyboardCancel=0;
	controls.nJoystickAccept=m_FireJoyMapping.nButton;
	controls.nJoystickCancel=m_BackJoyMapping.nButton;
	
	m_piGUIManager->SetAdditionalNavigationControls(&controls);
}

void CControlsDialog::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTOk)
	{
		m_piBTOk->PlayClickSound();
		EndDialog(DIALOG_OK);
	}
	else if(piControl==m_piBTKeyUp){ProcessKeyChange(&m_UpKeyMapping);UpdateGUIControls();UpdateGUI();}
	else if(piControl==m_piBTKeyDown){ProcessKeyChange(&m_DownKeyMapping);UpdateGUIControls();UpdateGUI();}
	else if(piControl==m_piBTKeyLeft){ProcessKeyChange(&m_LeftKeyMapping);UpdateGUIControls();UpdateGUI();}
	else if(piControl==m_piBTKeyRight){ProcessKeyChange(&m_RightKeyMapping);UpdateGUIControls();UpdateGUI();}
	else if(piControl==m_piBTKeyFire){ProcessKeyChange(&m_FireKeyMapping);UpdateGUIControls();UpdateGUI();}
	else if(piControl==m_piBTKeyBomb){ProcessKeyChange(&m_BombKeyMapping);UpdateGUIControls();UpdateGUI();}
	else if(piControl==m_piBTJoyFire){ProcessJoystickButtonChange(&m_FireJoyMapping);UpdateGUIControls();UpdateGUI();}
	else if(piControl==m_piBTJoyBomb){ProcessJoystickButtonChange(&m_BombJoyMapping);UpdateGUIControls();UpdateGUI();}
	else if(piControl==m_piBTJoyBack){ProcessJoystickButtonChange(&m_BackJoyMapping);UpdateGUIControls();UpdateGUI();}
	else if(piControl==m_piBTControls)
	{
		m_bShowKeyboardControls=!m_bShowKeyboardControls;
		UpdateGUI();
	}
	else if(piControl==m_piBTJoyDevice)
	{
		CycleJoystick(1);
	}
}

void CControlsDialog::CycleJoystick(int nIncrement)
{
	std::vector<std::string> vJoysticks;
	m_piGUIManager->GetJoysticks(&vJoysticks);
	if(vJoysticks.size())
	{
		int nStartId=0;
		for(unsigned int x=0;x<vJoysticks.size();x++)
		{
			if(vJoysticks[x]==m_piGUIManager->GetCurrentJoystick())
			{
				nStartId=x;
				break;
			}
		}
		// Cycle all joysticks starting from the current next one until one can be configured.
		for(unsigned int x=0;x<vJoysticks.size();x++)
		{
			unsigned int id=(x+nStartId+nIncrement+vJoysticks.size())%vJoysticks.size();
			if(m_piGUIManager->SetCurrentJoystick(vJoysticks[id]))
			{
				m_sJoystickDevice=vJoysticks[id];
				break;
			}
		}
	}
	UpdateGUI();
}

void CControlsDialog::OnSliderValueChanged(IGameGUISlider *piControl,double dValue)
{
	if(piControl==m_piSLJoyDeadZone)
	{
		m_dJoystickDeadZone=m_piSLJoyDeadZone->GetValue()*0.8+0.1;
		m_piGUIManager->SetJoystickDeadZone(m_dJoystickDeadZone);
		UpdateGUI();
	}
}

void CControlsDialog::ProcessJoystickButtonChange(SJoystickButtonMapping *pJoyButtonMapping)
{
	if(m_KeyCaptureDialog.m_piKeyCaptureDialog==NULL){return;}

	unsigned int nButton=0;
	if(m_KeyCaptureDialog.m_piKeyCaptureDialog->CaptureJoystickButton(this,&nButton))
	{
		// Search for duplicates.

		bool bSameSelected=false;

		SJoystickButtonMapping *ppAllJoyButtonMappings[]={&m_FireJoyMapping,&m_BombJoyMapping,&m_BackJoyMapping,NULL};
		SJoystickButtonMapping  **ppCurrentJoyButtonMapping=ppAllJoyButtonMappings;
		SJoystickButtonMapping  *pInUse=NULL;
		while(!pInUse && !bSameSelected && *ppCurrentJoyButtonMapping!=NULL)
		{
			if((*ppCurrentJoyButtonMapping)->nButton==(int)nButton)
			{
				bSameSelected=((*ppCurrentJoyButtonMapping)->sFriendlyName==pJoyButtonMapping->sFriendlyName);
				pInUse=(*ppCurrentJoyButtonMapping);
			}
			ppCurrentJoyButtonMapping++;
		}

		if(bSameSelected){return;}
		if(pInUse)
		{
			std::string sText="Button assigned to ";
			sText+=pInUse->sFriendlyName;
			sText+=". Reassign?";

			if(!ConfirmDialog(sText,"Friking shark",eMessageDialogType_Warning))
			{
				return;
			}
			else
			{
				pInUse->nButton=0;
			}
		}
		pJoyButtonMapping->nButton=nButton;
	}
}

void CControlsDialog::ProcessKeyChange(SKeyMapping *pKeyMapping)
{
	if(m_KeyCaptureDialog.m_piKeyCaptureDialog==NULL){return;}
	
	unsigned int nKey=0;
	if(m_KeyCaptureDialog.m_piKeyCaptureDialog->CaptureKey(this,&nKey))
	{
		// Search for duplicates.
		
		bool bSameSelected=false;
		
		SKeyMapping *ppAllKeyMappings[]={&m_UpKeyMapping,&m_DownKeyMapping,&m_LeftKeyMapping,&m_RightKeyMapping,&m_FireKeyMapping,&m_BombKeyMapping,NULL};
		SKeyMapping **ppCurrentKeyMapping=ppAllKeyMappings;
		SKeyMapping *pInUse=NULL;
		while(!pInUse && !bSameSelected && *ppCurrentKeyMapping!=NULL)
		{
			for(unsigned int x=0;x<(*ppCurrentKeyMapping)->vValidCombinations.size();x++)
			{
				if((*ppCurrentKeyMapping)->vValidCombinations[x].nKey==(int)nKey)
				{
					bSameSelected=((*ppCurrentKeyMapping)->sFriendlyName==pKeyMapping->sFriendlyName);
					pInUse=(*ppCurrentKeyMapping);
					break;
				}
			}
			ppCurrentKeyMapping++;
		}
		
		if(bSameSelected){return;}
		if(pInUse)
		{
			std::string sText="Key assigned to ";
			sText+=pInUse->sFriendlyName;
			sText+=". Reassign?";
			
			if(!ConfirmDialog(sText,"Friking shark",eMessageDialogType_Warning))
			{
				return;
			}
			else
			{
				pInUse->vValidCombinations.clear();
			}
		}
		
		
		if(pKeyMapping->vValidCombinations.size())
		{
			pKeyMapping->vValidCombinations[0].nKey=nKey;
		}
		else
		{
			pKeyMapping->vValidCombinations.push_back(SKeyCombination(nKey));
		}
	}
}

void CControlsDialog::UpdateRow(IGameGUIButton *piKeyButton,SKeyMapping *pKeyMapping)
{
	std::string sKey1="<None>";
	if(pKeyMapping->vValidCombinations.size()>0){m_piGUIManager->GetKeyName(pKeyMapping->vValidCombinations[0].nKey,&sKey1);}
	piKeyButton->SetText(sKey1);	
}

void CControlsDialog::UpdateRow(IGameGUIButton *piJoyButton,SJoystickButtonMapping *pJoyMapping)
{
	std::string sButton="<None>";
	if(pJoyMapping->nButton){m_piGUIManager->GetKeyName(pJoyMapping->nButton,&sButton);}
	piJoyButton->SetText(sButton);	
}

void CControlsDialog::UpdateGUI()
{
	UpdateRow(m_piBTKeyUp,&m_UpKeyMapping);
	UpdateRow(m_piBTKeyDown,&m_DownKeyMapping);
	UpdateRow(m_piBTKeyLeft,&m_LeftKeyMapping);
	UpdateRow(m_piBTKeyRight,&m_RightKeyMapping);
	UpdateRow(m_piBTKeyFire,&m_FireKeyMapping);
	UpdateRow(m_piBTKeyBomb,&m_BombKeyMapping);

	std::string sJoystick=m_piGUIManager->GetCurrentJoystick();
	if(sJoystick.length())
	{
		UpdateRow(m_piBTJoyFire,&m_FireJoyMapping);
		UpdateRow(m_piBTJoyBomb,&m_BombJoyMapping);
		UpdateRow(m_piBTJoyBack,&m_BackJoyMapping);
		if(m_piBTJoyDevice){m_piBTJoyDevice->SetText(sJoystick.c_str());}
	}
	else
	{
		if(m_piBTJoyFire){m_piBTJoyFire->SetText("Not Detected");}
		if(m_piBTJoyBomb){m_piBTJoyBomb->SetText("Not Detected");}
		if(m_piBTJoyBack){m_piBTJoyBack->SetText("Not Detected");}
		if(m_piBTJoyDevice){m_piBTJoyDevice->SetText("Not Detected");}
	}
	if(m_piSLJoyDeadZone)
	{
		char sTemp[100];
		sprintf(sTemp,"%d%%",(int)(m_piSLJoyDeadZone->GetValue()*100.0));
		m_piSLJoyDeadZone->SetText(sTemp);
	}
	if(m_piBTJoyFire){m_piBTJoyFire->Activate(sJoystick.length()!=0);}
	if(m_piBTJoyBomb){m_piBTJoyBomb->Activate(sJoystick.length()!=0);}
	if(m_piBTJoyBack){m_piBTJoyBack->Activate(sJoystick.length()!=0);}
	if(m_piBTControls){m_piBTControls->SetText(m_bShowKeyboardControls?"Keyboard":"Joystick / Gamepad");}
	if(m_piSTKeyboardControls){m_piSTKeyboardControls->Show(m_bShowKeyboardControls);}
	if(m_piSTJoystickControls){m_piSTJoystickControls->Show(!m_bShowKeyboardControls);}
}

void CControlsDialog::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	UpdateGUI();
	if(m_piSLJoyDeadZone)
	{
		m_piSLJoyDeadZone->SetValue((m_dJoystickDeadZone-0.1)/0.8);
	}
	if(m_piBTControls)
	{
		m_piBTControls->DisableSounds();
		m_piGUIManager->SetFocus(m_piBTControls);
		m_piBTControls->EnableSounds();		
	}
}

bool CControlsDialog::SelectControls(IGameWindow *piParent,IPlayerProfile *piProfile)
{
	if(piProfile)
	{
		piProfile->GetKeyMapping("MoveForward",&m_UpKeyMapping);
		piProfile->GetKeyMapping("MoveBackward",&m_DownKeyMapping);
		piProfile->GetKeyMapping("MoveLeft",&m_LeftKeyMapping);
		piProfile->GetKeyMapping("MoveRight",&m_RightKeyMapping);
		piProfile->GetKeyMapping("FireBullets",&m_FireKeyMapping);
		piProfile->GetKeyMapping("FireBomb",&m_BombKeyMapping);
		piProfile->GetJoystickButtonMapping("FireBullets",&m_FireJoyMapping);
		piProfile->GetJoystickButtonMapping("FireBomb",&m_BombJoyMapping);
		piProfile->GetJoystickButtonMapping("Back",&m_BackJoyMapping);
		m_dJoystickDeadZone=piProfile->GetJoystickDeadZone();
		m_sJoystickDevice=piProfile->GetJoystickDevice();
	}

	Execute(piParent);
	
	piProfile->SetKeyMapping("MoveForward",&m_UpKeyMapping);
	piProfile->SetKeyMapping("MoveBackward",&m_DownKeyMapping);
	piProfile->SetKeyMapping("MoveLeft",&m_LeftKeyMapping);
	piProfile->SetKeyMapping("MoveRight",&m_RightKeyMapping);
	piProfile->SetKeyMapping("FireBullets",&m_FireKeyMapping);
	piProfile->SetKeyMapping("FireBomb",&m_BombKeyMapping);
	piProfile->SetJoystickButtonMapping("FireBullets",&m_FireJoyMapping);
	piProfile->SetJoystickButtonMapping("FireBomb",&m_BombJoyMapping);
	piProfile->SetJoystickButtonMapping("Back",&m_BackJoyMapping);
	piProfile->SetJoystickDeadZone(m_dJoystickDeadZone);
	piProfile->SetJoystickDevice(m_sJoystickDevice);
	return true;
}

void CControlsDialog::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(m_piGUIManager->IsNavigationControl(eGameGUINavigationControl_Accept,nKey)){return;}
	if(m_piGUIManager->IsNavigationControl(eGameGUINavigationControl_Left,nKey) || m_piGUIManager->IsNavigationControl(eGameGUINavigationControl_Right,nKey))
	{
		if(m_piGUIManager->HasFocus(m_piBTJoyDevice))
		{
			CycleJoystick(m_piGUIManager->IsNavigationControl(eGameGUINavigationControl_Left,nKey)?-1:1);
		}
		else
		{
			m_bShowKeyboardControls=!m_bShowKeyboardControls;
			if(m_piBTControls)
			{
				m_piBTControls->DisableSounds();
				m_piGUIManager->SetFocus(m_piBTControls);
				m_piBTControls->EnableSounds();		
			}

			UpdateGUI();
		}
		
		*pbProcessed=true;
		return;
	}
	if(m_piGUIManager->IsNavigationControl(eGameGUINavigationControl_Cancel,nKey)){m_piBTOk->PlayClickSound();}
	
	CGameDialogBase::OnKeyDown(nKey,pbProcessed);
}