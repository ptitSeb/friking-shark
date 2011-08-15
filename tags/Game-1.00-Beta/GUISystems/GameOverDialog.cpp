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
#include "GameOverDialog.h"

CGameOverDialog::CGameOverDialog(void)
{
	m_nStartTime=0;
	m_bAlreadyFinished=false;
	m_piGameOverMusicSound=NULL;
}

CGameOverDialog::~CGameOverDialog(void)
{
}
bool CGameOverDialog::Unserialize(ISystemPersistencyNode *piNode)
{
	bool bOk=CGameDialogBase::Unserialize(piNode);
	if(bOk)
	{
		if(m_GameOverMusic.m_piSoundType)
		{
			m_piGameOverMusicSound=m_GameOverMusic.m_piSoundType->CreateInstance();
			if(m_piGameOverMusicSound){m_piGameOverMusicSound->SetLoop(false);}
		}
	}
	return bOk;
}

void CGameOverDialog::Destroy()
{
	if(m_piGameOverMusicSound)
	{
		if(m_piGameOverMusicSound->IsPlaying()){m_piGameOverMusicSound->Stop();}
		delete m_piGameOverMusicSound;
		m_piGameOverMusicSound=NULL;
	}
	
	CGameDialogBase::Destroy();
}


int	CGameOverDialog::Execute(IGameWindow *piParent)
{
	m_nStartTime=GetTimeStamp();
	m_bAlreadyFinished=false;
	if(m_piGameOverMusicSound){m_piGameOverMusicSound->Play();}
	int nRes=CGameDialogBase::Execute(piParent);
	if(m_piGameOverMusicSound){m_piGameOverMusicSound->Stop();}
	return nRes;
}

void CGameOverDialog::OnDraw(IGenericRender *piRender)
{
	CGameDialogBase::OnDraw(piRender);
	
	if(m_piGameOverMusicSound==NULL)
	{
		if(!m_bAlreadyFinished && (m_nStartTime+3*1000)<GetTimeStamp())
		{
			m_bAlreadyFinished=true;
			EndDialog(0);
		}
	}
	else
	{
		if(!m_bAlreadyFinished && !m_piGameOverMusicSound->IsPlaying())
		{
			m_bAlreadyFinished=true;
			EndDialog(0);
		}
	}
}

