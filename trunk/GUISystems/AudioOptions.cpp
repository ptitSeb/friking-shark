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
#include "AudioOptions.h"

CAudioOptions::CAudioOptions(void)
{
}

CAudioOptions::~CAudioOptions(void)
{
}

void CAudioOptions::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	
	if(m_piSLMaster)
	{
		m_piSLMaster->DisableSounds();
		m_piGUIManager->SetFocus(m_piSLMaster);
	}
	m_piSLMaster->EnableSounds();
	m_piSLMusic->EnableSounds();
	m_piSLSounds->EnableSounds();
	
	if(m_SoundManager.m_piSoundManager)
	{
		m_piSLMaster->SetValue(((double)m_SoundManager.m_piSoundManager->GetMasterVolume())/100.0);
		m_piSLMusic->SetValue(((double)m_SoundManager.m_piSoundManager->GetGroupVolume("Music"))/100.0);
		m_piSLSounds->SetValue(((double)m_SoundManager.m_piSoundManager->GetGroupVolume("SoundFX"))/100.0);
	}
	UpdateGUI();
}

void CAudioOptions::OnEndDialog()
{
	m_piSLMaster->DisableSounds();
	m_piSLMusic->DisableSounds();
	m_piSLSounds->DisableSounds();
	
	CGameDialogBase::OnEndDialog();
}

void CAudioOptions::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTOk)
	{
		m_piBTOk->PlayClickSound();
		EndDialog(DIALOG_OK);
	}
}

void CAudioOptions::OnSliderValueChanged(IGameGUISlider *piControl,double dValue)
{
	if(piControl==m_piSLMaster){m_SoundManager.m_piSoundManager->SetMasterVolume(dValue*100.0);UpdateGUI();}
	if(piControl==m_piSLMusic){m_SoundManager.m_piSoundManager->SetGroupVolume("Music",dValue*100.0);UpdateGUI();}
	if(piControl==m_piSLSounds){m_SoundManager.m_piSoundManager->SetGroupVolume("SoundFX",dValue*100.0);UpdateGUI();}
}

void CAudioOptions::UpdateGUI()
{
	char sTemp[100];
	sprintf(sTemp,"%d%%",(int)(m_piSLMaster->GetValue()*100.0));
	m_piSLMaster->SetText(sTemp);
	sprintf(sTemp,"%d%%",(int)(m_piSLMusic->GetValue()*100.0));
	m_piSLMusic->SetText(sTemp);
	sprintf(sTemp,"%d%%",(int)(m_piSLSounds->GetValue()*100.0));
	m_piSLSounds->SetText(sTemp);
}

bool CAudioOptions::Show(IGameWindow *piParent)
{	
	if(m_SoundManager.m_piSoundManager==NULL)
	{
		m_SoundManager.Attach("GameGUI","SoundManager");
	}
	Execute(piParent);
	
	m_SoundManager.Detach();
	
	return true;
}

void CAudioOptions::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_RETURN){return;}
	if(nKey==GK_ESCAPE){m_piBTOk->PlayClickSound();}
	CGameDialogBase::OnKeyDown(nKey,pbProcessed);
}