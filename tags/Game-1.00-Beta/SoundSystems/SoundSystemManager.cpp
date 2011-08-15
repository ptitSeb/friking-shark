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
#include "SoundSystems.h"
#include "SoundSystemManager.h"

CSoundSystemManager::CSoundSystemManager()
{
  m_nMasterVolume=100;
  m_bEnable3DSound=false;
  m_pContext = NULL;
  m_pDevice = NULL;
  m_bMuted = false;
}

CSoundSystemManager::~CSoundSystemManager()
{
}

bool CSoundSystemManager::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
  bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);;
  if(bOk) {m_pDevice=alcOpenDevice(NULL);bOk=(m_pDevice!=NULL);}
  if(bOk) {m_pContext=alcCreateContext(m_pDevice,NULL);bOk=(m_pContext!=NULL);}
  if(bOk) {bOk=(alcMakeContextCurrent(m_pContext)==ALC_TRUE);}
  if(bOk) {bOk=(alutInitWithoutContext(NULL,NULL)==ALC_TRUE);}
  if(!bOk)
  {
	  RTTRACE("CSoundSystemManager::Init -> Failed to create sound context %d",alGetError());
  }
  m_vListenerOrientation=m_vListenerPosition=m_vListenerVelocity=Origin;
  return bOk;
}


bool CSoundSystemManager::Unserialize(ISystemPersistencyNode *piNode)
{
	bool bOk=CSystemObjectBase::Unserialize(piNode);
	if(bOk && m_pContext)
	{
		alListenerf(AL_GAIN ,m_bMuted?0:((float)m_nMasterVolume)/(float)100.0);
	}
	return bOk;
}

void CSoundSystemManager::Destroy()
{
  if(m_pContext){alutExit();}
  if(m_pContext){alcMakeContextCurrent(NULL);alcDestroyContext(m_pContext);m_pContext=NULL;}
  if(m_pDevice){alcCloseDevice(m_pDevice);m_pDevice=NULL;}
  CSystemObjectBase::Destroy();
}

void CSoundSystemManager::UpdateListener()
{
	if(!m_pContext){return;}
	
	alListener3f(AL_POSITION,(float)m_vListenerPosition.c[0],(float)m_vListenerPosition.c[1],(float)m_vListenerPosition.c[2]);
	alListener3f(AL_VELOCITY,(float)m_vListenerVelocity.c[0],(float)m_vListenerVelocity.c[1],(float)m_vListenerVelocity.c[2]);
	alListener3f(AL_ORIENTATION,(float)m_vListenerOrientation.c[0],(float)m_vListenerOrientation.c[1],(float)m_vListenerOrientation.c[2]);

	if(Is3DSoundEnabled())
	{
		alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
		alDopplerFactor(1.0);
	}
	else
	{
		alDistanceModel(AL_NONE);
		alDopplerFactor(0);
	}
}

unsigned int  CSoundSystemManager::GetMasterVolume(){return m_nMasterVolume;}
bool CSoundSystemManager::Is3DSoundEnabled(){return m_bEnable3DSound;}

void CSoundSystemManager::SetMasterVolume(unsigned int nVolume)
{	
	if(m_pContext)
	{
		m_nMasterVolume=nVolume;
		alListenerf(AL_GAIN ,m_bMuted?0:((float)m_nMasterVolume)/(float)100.0);
		NOTIFY_EVENT(ISoundManagerEvents,OnMasterVolumeChanged(m_nMasterVolume));
	}
}

unsigned int CSoundSystemManager::GetGroupVolume(const std::string &sName)
{
	std::map<std::string, unsigned int>::iterator i=m_mGroupVolumes.find(sName);
	if(i==m_mGroupVolumes.end()){return 100;}
	return i->second;
}

void CSoundSystemManager::SetGroupVolume(const std::string &sName,unsigned int nVolume)
{
	m_mGroupVolumes[sName]=nVolume;
	NOTIFY_EVENT(ISoundManagerEvents,OnGroupVolumeChanged(sName,m_nMasterVolume));
}

void CSoundSystemManager::SetListenerPosition(CVector vPosition){m_vListenerPosition=vPosition;UpdateListener();}
void CSoundSystemManager::SetListenerOrientation(CVector vOrientation){m_vListenerOrientation=vOrientation;UpdateListener();}
void CSoundSystemManager::SetListenerVelocity(CVector vVelocity){m_vListenerVelocity=vVelocity;UpdateListener();}

void CSoundSystemManager::SetMute(bool bOn)
{
	m_bMuted=bOn;
	if(m_pContext)
	{
		alListenerf(AL_GAIN ,m_bMuted?0:((float)m_nMasterVolume)/(float)100.0);
	}
	NOTIFY_EVENT(ISoundManagerEvents,OnMute(m_bMuted));
}
