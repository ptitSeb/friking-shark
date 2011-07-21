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
	else if(piControl==m_piBTCancel)
	{
		EndDialog(DIALOG_CANCEL);
	}
	else if(piControl==m_piBTUpClear){m_UpKeyMapping.vValidCombinations.clear();UpdateGUI();}
	else if(piControl==m_piBTDownClear){m_DownKeyMapping.vValidCombinations.clear();UpdateGUI();}
	else if(piControl==m_piBTLeftClear){m_LeftKeyMapping.vValidCombinations.clear();UpdateGUI();}
	else if(piControl==m_piBTRightClear){m_RightKeyMapping.vValidCombinations.clear();UpdateGUI();}
	else if(piControl==m_piBTFireClear){m_FireKeyMapping.vValidCombinations.clear();UpdateGUI();}
	else if(piControl==m_piBTBombClear){m_BombKeyMapping.vValidCombinations.clear();UpdateGUI();}
	else if(piControl==m_piBTUpA){ProcessKeyChange(&m_UpKeyMapping,0);UpdateGUI();}
	else if(piControl==m_piBTUpB){ProcessKeyChange(&m_UpKeyMapping,1);UpdateGUI();}
	else if(piControl==m_piBTDownA){ProcessKeyChange(&m_DownKeyMapping,0);UpdateGUI();}
	else if(piControl==m_piBTDownB){ProcessKeyChange(&m_DownKeyMapping,1);UpdateGUI();}
	else if(piControl==m_piBTLeftA){ProcessKeyChange(&m_LeftKeyMapping,0);UpdateGUI();}
	else if(piControl==m_piBTLeftB){ProcessKeyChange(&m_LeftKeyMapping,1);UpdateGUI();}
	else if(piControl==m_piBTRightA){ProcessKeyChange(&m_RightKeyMapping,0);UpdateGUI();}
	else if(piControl==m_piBTRightB){ProcessKeyChange(&m_RightKeyMapping,1);UpdateGUI();}
	else if(piControl==m_piBTFireA){ProcessKeyChange(&m_FireKeyMapping,0);UpdateGUI();}
	else if(piControl==m_piBTFireB){ProcessKeyChange(&m_FireKeyMapping,1);UpdateGUI();}
	else if(piControl==m_piBTBombA){ProcessKeyChange(&m_BombKeyMapping,0);UpdateGUI();}
	else if(piControl==m_piBTBombB){ProcessKeyChange(&m_BombKeyMapping,1);UpdateGUI();}
}

void CControlsDialog::ProcessKeyChange(SKeyMapping *pKeyMapping, unsigned int nIndex)
{
	if(m_KeyCaptureDialog.m_piKeyCaptureDialog==NULL){return;}
	
	unsigned int nKey=0;
	if(m_KeyCaptureDialog.m_piKeyCaptureDialog->CaptureKey(this,&nKey))
	{
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

void CControlsDialog::UpdateRow(IGameGUIButton *piKey1Button,IGameGUIButton *piKey2Button, SKeyMapping *pKeyMapping)
{
	std::string sKey1="<None>";
	std::string sKey2="<None>";
	if(pKeyMapping->vValidCombinations.size()>0){m_piGUIManager->GetKeyName(pKeyMapping->vValidCombinations[0].nKey,&sKey1);}
	if(pKeyMapping->vValidCombinations.size()>1){m_piGUIManager->GetKeyName(pKeyMapping->vValidCombinations[1].nKey,&sKey2);}
	piKey1Button->SetText(sKey1);
	piKey2Button->SetText(sKey2);	
	piKey2Button->Show(pKeyMapping->vValidCombinations.size()>0);
}

void CControlsDialog::UpdateGUI()
{
	UpdateRow(m_piBTUpA,m_piBTUpB,&m_UpKeyMapping);
	UpdateRow(m_piBTDownA,m_piBTDownB,&m_DownKeyMapping);
	UpdateRow(m_piBTLeftA,m_piBTLeftB,&m_LeftKeyMapping);
	UpdateRow(m_piBTRightA,m_piBTRightB,&m_RightKeyMapping);
	UpdateRow(m_piBTFireA,m_piBTFireB,&m_FireKeyMapping);
	UpdateRow(m_piBTBombA,m_piBTBombB,&m_BombKeyMapping);
}

void CControlsDialog::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	UpdateGUI();
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
	int nRes=Execute(piParent);
	if(nRes==DIALOG_OK)
	{
		piProfile->SetKeyMapping("MoveForward",&m_UpKeyMapping);
		piProfile->SetKeyMapping("MoveBackward",&m_DownKeyMapping);
		piProfile->SetKeyMapping("MoveLeft",&m_LeftKeyMapping);
		piProfile->SetKeyMapping("MoveRight",&m_RightKeyMapping);
		piProfile->SetKeyMapping("FireBullets",&m_FireKeyMapping);
		piProfile->SetKeyMapping("FireBomb",&m_BombKeyMapping);
	}
	return nRes==DIALOG_OK;
}
