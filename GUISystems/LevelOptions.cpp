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
#include "LevelOptions.h"

CLevelOptions::CLevelOptions(void)
{
	m_eMode=eGameMode_Normal;
	m_eDifficulty=eGameDifficulty_Normal;
	m_nSelectedLevel=0;
}

CLevelOptions::~CLevelOptions(void)
{
}

void CLevelOptions::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTOk)
	{
		EndDialog(DIALOG_OK);
	}
	if(piControl==m_piBTCancel)
	{
		EndDialog(DIALOG_CANCEL);
	}
	if(piControl==m_piBTLevel1)
	{
		m_nSelectedLevel=0;
		UpdateGUI();
	}
	if(piControl==m_piBTLevel2)
	{
		m_nSelectedLevel=1;
		UpdateGUI();
	}
	if(piControl==m_piBTLevel3)
	{
		m_nSelectedLevel=2;
		UpdateGUI();
	}	
	if(piControl==m_piBTLevel4)
	{
		m_nSelectedLevel=3;
		UpdateGUI();
	}
	if(piControl==m_piBTLevel5)
	{
		m_nSelectedLevel=4;
		UpdateGUI();
	}
	if(piControl==m_piBTMode)
	{
		unsigned int nMode=m_eMode;
		nMode++;
		if(nMode>=eGameMode_Count){nMode=eGameMode_Normal;}
		m_eMode=(EGameMode)nMode;
		UpdateGUI();
	}
	
	if(piControl==m_piBTDifficulty)
	{
		unsigned int nDifficulty=m_eDifficulty;
		nDifficulty++;
		if(nDifficulty>=eGameDifficulty_Count){nDifficulty=eGameDifficulty_Easy;}
		m_eDifficulty=(EGameDifficulty)nDifficulty;
		UpdateGUI();
	}
}

void CLevelOptions::UpdateGUI()
{
	if(m_eMode==eGameMode_Normal){m_piBTMode->SetText("Classic (3 Lives)");}
	if(m_eMode==eGameMode_God){m_piBTMode->SetText("God");}
	if(m_eMode==eGameMode_InfiniteLives){m_piBTMode->SetText("Infinite Lives");}

	if(m_eDifficulty==eGameDifficulty_Easy){m_piBTDifficulty->SetText("Easy");}
	if(m_eDifficulty==eGameDifficulty_Normal){m_piBTDifficulty->SetText("Normal");}
	if(m_eDifficulty==eGameDifficulty_Hard){m_piBTDifficulty->SetText("Hard");}
	if(m_eDifficulty==eGameDifficulty_VeryHard){m_piBTDifficulty->SetText("Very Hard");}
	
	double dFontSize=0;
	if(m_piBTLevel1 && m_piBTLevel2 && m_piBTLevel3 && m_piBTLevel4 && m_piBTLevel5)
	{
		m_piBTLevel1->GetFont(NULL,&dFontSize);
		m_piBTLevel1->SetFont(m_nSelectedLevel==0?m_LevelSelectedFont.m_piFont:m_LevelUnselectedFont.m_piFont,dFontSize);
		m_piBTLevel2->SetFont(m_nSelectedLevel==1?m_LevelSelectedFont.m_piFont:m_LevelUnselectedFont.m_piFont,dFontSize);
		m_piBTLevel3->SetFont(m_nSelectedLevel==2?m_LevelSelectedFont.m_piFont:m_LevelUnselectedFont.m_piFont,dFontSize);
		m_piBTLevel4->SetFont(m_nSelectedLevel==3?m_LevelSelectedFont.m_piFont:m_LevelUnselectedFont.m_piFont,dFontSize);
		m_piBTLevel5->SetFont(m_nSelectedLevel==4?m_LevelSelectedFont.m_piFont:m_LevelUnselectedFont.m_piFont,dFontSize);
		m_piBTLevel1->SetTextColor(m_nSelectedLevel==0?CVector(1,1,1):CVector(0.5,0.5,0.5),m_nSelectedLevel==0?1.0:0.5);
		m_piBTLevel2->SetTextColor(m_nSelectedLevel==1?CVector(1,1,1):CVector(0.5,0.5,0.5),m_nSelectedLevel==1?1.0:0.5);
		m_piBTLevel3->SetTextColor(m_nSelectedLevel==2?CVector(1,1,1):CVector(0.5,0.5,0.5),m_nSelectedLevel==2?1.0:0.5);
		m_piBTLevel4->SetTextColor(m_nSelectedLevel==3?CVector(1,1,1):CVector(0.5,0.5,0.5),m_nSelectedLevel==3?1.0:0.5);
		m_piBTLevel5->SetTextColor(m_nSelectedLevel==4?CVector(1,1,1):CVector(0.5,0.5,0.5),m_nSelectedLevel==4?1.0:0.5);
		m_piBTLevel1->SetBackgroundColor(m_nSelectedLevel==0?CVector(1,1,1):CVector(0.5,0.5,0.5),m_nSelectedLevel==0?1.0:0.5);
		m_piBTLevel2->SetBackgroundColor(m_nSelectedLevel==1?CVector(1,1,1):CVector(0.5,0.5,0.5),m_nSelectedLevel==1?1.0:0.5);
		m_piBTLevel3->SetBackgroundColor(m_nSelectedLevel==2?CVector(1,1,1):CVector(0.5,0.5,0.5),m_nSelectedLevel==2?1.0:0.5);
		m_piBTLevel4->SetBackgroundColor(m_nSelectedLevel==3?CVector(1,1,1):CVector(0.5,0.5,0.5),m_nSelectedLevel==3?1.0:0.5);
		m_piBTLevel5->SetBackgroundColor(m_nSelectedLevel==4?CVector(1,1,1):CVector(0.5,0.5,0.5),m_nSelectedLevel==4?1.0:0.5);
	}
}

void CLevelOptions::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	UpdateGUI();
	if(m_piBTOk)
	{
		m_piBTOk->DisableSounds();
		m_piGUIManager->SetFocus(m_piBTOk);
		m_piBTOk->EnableSounds();
	}
}

bool CLevelOptions::SelectOptions(IGameWindow *piParent,EGameMode *pMode,EGameDifficulty *pDifficulty,unsigned int *pnSelectedLevel)
{
	m_eMode=*pMode;
	m_nSelectedLevel=*pnSelectedLevel;
	m_eDifficulty=*pDifficulty;
	int nRes=Execute(piParent);
	*pMode=m_eMode;
	*pnSelectedLevel=m_nSelectedLevel;
	*pDifficulty=m_eDifficulty;
	return nRes==DIALOG_OK;
}

void CLevelOptions::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_RETURN){return;}
	if(nKey==GK_ESCAPE){m_piBTOk->PlayClickSound();}
	
	if(nKey==GK_LEFT && m_piBTDifficulty && m_piGUIManager->HasFocus(m_piBTDifficulty))
	{
		int nDifficulty=m_eDifficulty;
		nDifficulty--;
		if(nDifficulty<0){nDifficulty=((int)eGameDifficulty_Count)-1;}
		m_eDifficulty=(EGameDifficulty)nDifficulty;
		m_piBTDifficulty->PlayClickSound();
		UpdateGUI();
		
		*pbProcessed=true;
		return;
	}
	if(nKey==GK_RIGHT && m_piBTDifficulty && m_piGUIManager->HasFocus(m_piBTDifficulty))
	{
		unsigned int nDifficulty=m_eDifficulty;
		nDifficulty++;
		if(nDifficulty>=eGameDifficulty_Count){nDifficulty=0;}
		m_eDifficulty=(EGameDifficulty)nDifficulty;
		m_piBTDifficulty->PlayClickSound();
		UpdateGUI();
		
		*pbProcessed=true;
		return;
	}
	if(nKey==GK_LEFT && m_piBTMode && m_piGUIManager->HasFocus(m_piBTMode))
	{
		int nMode=m_eMode;
		nMode--;
		if(nMode<0){nMode=((int)eGameMode_Count)-1;}
		m_eMode=(EGameMode)nMode;
		m_piBTMode->PlayClickSound();
		UpdateGUI();
		
		*pbProcessed=true;
		return;
	}
	if(nKey==GK_RIGHT && m_piBTMode && m_piGUIManager->HasFocus(m_piBTMode))
	{
		unsigned int nMode=m_eMode;
		nMode++;
		if(nMode>=eGameMode_Count){nMode=0;}
		m_eMode=(EGameMode)nMode;
		m_piBTMode->PlayClickSound();
		UpdateGUI();
		
		*pbProcessed=true;
		return;
	}
	CGameDialogBase::OnKeyDown(nKey,pbProcessed);
}