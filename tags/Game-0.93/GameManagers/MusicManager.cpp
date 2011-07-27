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
#include "GameGUI.h"
#include "MusicManager.h"

CMusicManager::CMusicManager(void)
{
	m_bStarted=false;
	
	m_piMusicSound=NULL;
	m_piIntroMusicSound=NULL;
	m_piIntermissionMusicSound=NULL;
	m_nIntermissionFadeStart=0;
	m_nIntroFadeStart=0;
	m_nMusicFadeStart=0;
	
	PersistencyInitialize();
}

CMusicManager::~CMusicManager(void)
{
}

bool CMusicManager::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
    bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);
    if(bOk){bOk=m_GameControllerWrapper.Attach("GameSystem","GameController");}
    if(bOk){bOk=m_PlayerManagerWrapper.Attach("GameSystem","PlayerManager");}
    if(bOk){m_GameControllerWrapper.m_piGameController->RegisterManager(1000,this);}
	return bOk;
}

void CMusicManager::Destroy()
{
	if(m_GameControllerWrapper.m_piGameController){m_GameControllerWrapper.m_piGameController->UnregisterManager(this);}
	m_IntermissionMusic.Destroy();
	m_GameControllerWrapper.Detach();
	m_PlayerManagerWrapper.Detach();
	CSystemObjectBase::Destroy();
}

void CMusicManager::CreateScenario()
{
	CloseScenario();
	PersistencyDefaultValue("ScenarioProps");
}

void CMusicManager::LoadScenario(ISystemPersistencyNode *piNode)
{
	PersistencyLoad(piNode->GetNode("Music"),"ScenarioProps");
}

void CMusicManager::SaveScenario(ISystemPersistencyNode *piNode)
{
	PersistencySave(piNode->AddNode("Music"),"ScenarioProps");
}

void CMusicManager::CloseScenario()
{
	m_IntroMusic.Destroy();
	m_Music.Destroy();
}

void CMusicManager::Start()
{
	m_bStarted=true;
	m_nIntermissionFadeStart=0;
	m_nIntroFadeStart=0;
	m_nMusicFadeStart=0;
	
	EPlayerManagerGameStage eStage=m_PlayerManagerWrapper.m_piPlayerManager->GetStage();
	if(eStage==ePlayerManagerGameStage_TakeOff)
	{
		if(m_IntroMusic.m_piSoundType)
		{
			m_piIntroMusicSound=m_IntroMusic.m_piSoundType->CreateInstance();
			if(m_piIntroMusicSound)
			{
				m_piIntroMusicSound->SetLoop(false);
				m_piIntroMusicSound->Play();
			}
		}
	}
	if(m_Music.m_piSoundType)
	{
		m_piMusicSound=m_Music.m_piSoundType->CreateInstance();
		if(m_piMusicSound)
		{
			m_piMusicSound->SetLoop(true);
		}
	}
	
	if(m_IntermissionMusic.m_piSoundType)
	{
		m_piIntermissionMusicSound=m_IntermissionMusic.m_piSoundType->CreateInstance();
		if(m_piIntermissionMusicSound)
		{
			m_piIntermissionMusicSound->SetLoop(false);
		}
	}
}

void CMusicManager::Stop()
{
	m_bStarted=false;

	if(m_piMusicSound)
	{
		if(m_piMusicSound->IsPlaying()){m_piMusicSound->Stop();}
		delete m_piMusicSound;
		m_piMusicSound=NULL;
	}
	if(m_piIntroMusicSound)
	{
		if(m_piIntroMusicSound->IsPlaying()){m_piIntroMusicSound->Stop();}
		delete m_piIntroMusicSound;
		m_piIntroMusicSound=NULL;
	}
	if(m_piIntermissionMusicSound)
	{
		if(m_piIntermissionMusicSound->IsPlaying()){m_piIntermissionMusicSound->Stop();}
		delete m_piIntermissionMusicSound;
		m_piIntermissionMusicSound=NULL;
	}
}

void CMusicManager::ProcessFadeOut(ISound *piSound, unsigned int nCurrentTime,unsigned int *pFadeStart)
{
	if(piSound && piSound->IsPlaying())
	{
		if(*pFadeStart==0){*pFadeStart=nCurrentTime;}
		
		// fade desde el 100% de volumen en 100 ms.
		double dVolume=piSound->GetVolume();
		dVolume-=((double)(nCurrentTime-*pFadeStart));
		if(dVolume>0)
		{
			piSound->SetVolume(dVolume);
			*pFadeStart=nCurrentTime;
		}
		else
		{
			piSound->Stop();
			*pFadeStart=0;
		}
	}
}

void CMusicManager::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	if(!m_bStarted){return;}
	
	
	EPlayerManagerGameStage eStage=m_PlayerManagerWrapper.m_piPlayerManager->GetStage();
	
	if(eStage==ePlayerManagerGameStage_Killed)
	{
		if(m_piMusicSound){m_piMusicSound->Stop();}
		if(m_piIntroMusicSound){m_piIntroMusicSound->Stop();}
		if(m_piIntermissionMusicSound){m_piIntermissionMusicSound->Stop();}
	}
	else if(eStage==ePlayerManagerGameStage_Finished)
	{
		ProcessFadeOut(m_piMusicSound,dwCurrentTime,&m_nMusicFadeStart);
		ProcessFadeOut(m_piIntroMusicSound,dwCurrentTime,&m_nIntroFadeStart);
		//ProcessFadeOut(m_piIntermissionMusicSound,dwCurrentTime,&m_nIntermissionFadeStart);
	}
	else if(eStage==ePlayerManagerGameStage_Landing)
	{
		ProcessFadeOut(m_piMusicSound,dwCurrentTime,&m_nMusicFadeStart);
		ProcessFadeOut(m_piIntroMusicSound,dwCurrentTime,&m_nIntroFadeStart);
		
		if(m_piIntermissionMusicSound && !m_piIntermissionMusicSound->IsPlaying())
		{
			m_piIntermissionMusicSound->Play();
		}
	}
	else
	{
		bool bPlayMainTheme=(m_piIntroMusicSound==NULL || !m_piIntroMusicSound->IsPlaying());
		if(bPlayMainTheme && m_piMusicSound && !m_piMusicSound->IsPlaying())
		{
			m_piMusicSound->Play();
		}
	}
}

bool CMusicManager::SetIntroMusic(std::string sMusicFile)
{
	if(m_piIntroMusicSound)
	{
		if(m_piIntroMusicSound->IsPlaying()){m_piIntroMusicSound->Stop();}
		delete m_piIntroMusicSound;
		m_piIntroMusicSound=NULL;
	}
	
	m_IntroMusic.Destroy();
	bool bOk=m_IntroMusic.Create(m_piSystem,"SoundType","");
	if(bOk){m_IntroMusic.m_piSoundType->Load(sMusicFile);}
	if(bOk && m_bStarted && m_IntroMusic.m_piSoundType)
	{
		m_piIntroMusicSound=m_IntroMusic.m_piSoundType->CreateInstance();
		if(m_piIntroMusicSound)
		{
			m_piIntroMusicSound->SetLoop(false);
			if(m_piMusicSound && m_piMusicSound->IsPlaying()){m_piMusicSound->Stop();}
			m_piIntroMusicSound->Play();				
		}
	}
	return bOk;
}

void CMusicManager::GetIntroMusic(std::string *psMusicFile,ISoundType **ppiSoundType)
{
	if(psMusicFile)
	{
		*psMusicFile="";
		if(m_IntroMusic.m_piSoundType){*psMusicFile=m_IntroMusic.m_piSoundType->GetFileName();}
	}
	if(ppiSoundType)
	{
		*ppiSoundType=NULL;
		if(m_IntroMusic.m_piSoundType){*ppiSoundType=m_IntroMusic.m_piSoundType;}
	}
}

bool CMusicManager::SetMusic(std::string sMusicFile)
{
	if(m_piMusicSound)
	{
		if(m_piMusicSound->IsPlaying()){m_piMusicSound->Stop();}
		delete m_piMusicSound;
		m_piMusicSound=NULL;
	}
	
	m_Music.Destroy();
	bool bOk=m_Music.Create(m_piSystem,"SoundType","");
	if(bOk){bOk=m_Music.m_piSoundType->Load(sMusicFile);}
	if(bOk && m_bStarted && m_Music.m_piSoundType)
	{
		m_piMusicSound=m_Music.m_piSoundType->CreateInstance();
		if(m_piMusicSound)
		{
			m_piMusicSound->SetLoop(true);
			m_piMusicSound->Play();
		}
	}
	return bOk;
}

void CMusicManager::GetMusic(std::string *psMusicFile,ISoundType **ppiSoundType)
{
	if(psMusicFile)
	{
		*psMusicFile="";
		if(m_Music.m_piSoundType){*psMusicFile=m_Music.m_piSoundType->GetFileName();}
	}
	if(ppiSoundType)
	{
		*ppiSoundType=NULL;
		if(m_Music.m_piSoundType){*ppiSoundType=m_Music.m_piSoundType;}
	}
}
