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
#include "SavedGameRow.h"

CSavedGameRow::CSavedGameRow(void)
{
	m_pSavedGame=NULL;
}

CSavedGameRow::~CSavedGameRow(void)
{
}

void CSavedGameRow::UpdateGUI()
{
	char sLevel[128]={0};
	char sLives[128]={0};
	char sWeapon[128]={0};
	char sBombs[128]={0};
	char sPoints[128]={0};
	const char *pMode="Unknown";
	const char *pDifficulty="Unknown";
	
	if(m_pSavedGame)
	{
		// nCheckpoint+2 -> level start must be shown as "1", first checkpoint as "2"
		// nWeapon+1 -> first level must be shown as "1"
		sprintf(sLevel,"Level %d-%d",m_pSavedGame->nLevel+1,m_pSavedGame->nCheckpoint+2);
		sprintf(sLives,"x%d",m_pSavedGame->nLivesLeft);
		sprintf(sWeapon,"x%d",m_pSavedGame->nWeapon+1);
		sprintf(sBombs,"x%d",m_pSavedGame->nBombs);
		sprintf(sPoints,"%d",m_pSavedGame->nPoints);
		switch(m_pSavedGame->eMode)
		{
			case eGameMode_God:pMode="God";break;
			case eGameMode_Normal:pMode="Normal";break;
			case eGameMode_InfiniteLives:pMode="Infinite Lives";break;
			default:pMode="Unknown";break;
		}
		switch(m_pSavedGame->eDifficulty)
		{
			case eGameDifficulty_Easy:pDifficulty="Easy";break;
			case eGameDifficulty_Normal:pDifficulty="Normal";break;
			case eGameDifficulty_Hard:pDifficulty="Hard";break;
			case eGameDifficulty_VeryHard:pDifficulty="Very Hard";break;
			default:pDifficulty="Unknown";break;
		}
	}
	m_piBTButton->Activate(m_bActive);
	m_piBTButton->Show(m_pSavedGame!=NULL);
	m_piBTEmpty->Activate(m_bActive);
	m_piBTEmpty->Show(m_pSavedGame==NULL);
	m_piSTLevel->SetText(sLevel);
	m_piSTLives->SetText(sLives);
	m_piSTWeapon->SetText(sWeapon);
	m_piSTBombs->SetText(sBombs);
	m_piSTPoints->SetText(sPoints);
	m_piSTMode->SetText(pMode);
	m_piSTDifficulty->SetText(pDifficulty);
	
}

void CSavedGameRow::SetSavedGame(SGameState *pGame)
{
	m_pSavedGame=pGame;
	UpdateGUI();
}


void CSavedGameRow::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTButton || piControl==m_piBTEmpty)
	{
		NOTIFY_EVENT(ISavedGameRowEvents,OnSavedGameSelected(this));
	}
}
