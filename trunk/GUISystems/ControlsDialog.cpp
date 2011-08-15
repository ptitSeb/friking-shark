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
}

CControlsDialog::~CControlsDialog(void)
{
}

void CControlsDialog::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTOk)
	{
		EndDialog(DIALOG_OK);
	}
	else if(piControl==m_piBTUpA){ProcessKeyChange(&m_UpKeyMapping,0);UpdateGUI();}
	else if(piControl==m_piBTDownA){ProcessKeyChange(&m_DownKeyMapping,0);UpdateGUI();}
	else if(piControl==m_piBTLeftA){ProcessKeyChange(&m_LeftKeyMapping,0);UpdateGUI();}
	else if(piControl==m_piBTRightA){ProcessKeyChange(&m_RightKeyMapping,0);UpdateGUI();}
	else if(piControl==m_piBTFireA){ProcessKeyChange(&m_FireKeyMapping,0);UpdateGUI();}
	else if(piControl==m_piBTBombA){ProcessKeyChange(&m_BombKeyMapping,0);UpdateGUI();}
}

void CControlsDialog::ProcessKeyChange(SKeyMapping *pKeyMapping, unsigned int nIndex)
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
		
		
		if(nIndex<pKeyMapping->vValidCombinations.size())
		{
			pKeyMapping->vValidCombinations[nIndex].nKey=nKey;
		}
		else
		{
			pKeyMapping->vValidCombinations.push_back(SKeyCombination(nKey));
		}
	}
}

void CControlsDialog::UpdateRow(IGameGUIButton *piKey1Button,SKeyMapping *pKeyMapping)
{
	std::string sKey1="<None>";
	if(pKeyMapping->vValidCombinations.size()>0){m_piGUIManager->GetKeyName(pKeyMapping->vValidCombinations[0].nKey,&sKey1);}
	piKey1Button->SetText(sKey1);	
}

void CControlsDialog::UpdateGUI()
{
	UpdateRow(m_piBTUpA,&m_UpKeyMapping);
	UpdateRow(m_piBTDownA,&m_DownKeyMapping);
	UpdateRow(m_piBTLeftA,&m_LeftKeyMapping);
	UpdateRow(m_piBTRightA,&m_RightKeyMapping);
	UpdateRow(m_piBTFireA,&m_FireKeyMapping);
	UpdateRow(m_piBTBombA,&m_BombKeyMapping);
}

void CControlsDialog::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	UpdateGUI();
	if(m_piBTUpA)
	{
		m_piBTUpA->DisableSounds();
		m_piGUIManager->SetFocus(m_piBTUpA);
		m_piBTUpA->EnableSounds();		
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
	}

	Execute(piParent);
	
	piProfile->SetKeyMapping("MoveForward",&m_UpKeyMapping);
	piProfile->SetKeyMapping("MoveBackward",&m_DownKeyMapping);
	piProfile->SetKeyMapping("MoveLeft",&m_LeftKeyMapping);
	piProfile->SetKeyMapping("MoveRight",&m_RightKeyMapping);
	piProfile->SetKeyMapping("FireBullets",&m_FireKeyMapping);
	piProfile->SetKeyMapping("FireBomb",&m_BombKeyMapping);
	
	return true;
}

void CControlsDialog::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_RETURN){return;}
	CGameDialogBase::OnKeyDown(nKey,pbProcessed);
}