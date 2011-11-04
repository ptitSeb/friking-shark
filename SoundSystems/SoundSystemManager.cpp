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

#ifdef ANDROID
	#include "../GameEngine/android_native_app_glue.h"
	extern "C"{jint JNI_OnLoad(JavaVM* vm, void* reserved);};
	extern android_app *g_pAndroidApp;
#endif

	   
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
  m_vListenerOrientation=m_vListenerPosition=m_vListenerVelocity=Origin;
  return bOk;
}


bool CSoundSystemManager::Unserialize(ISystemPersistencyNode *piNode)
{
	bool bOk=CSystemObjectBase::Unserialize(piNode);
#ifdef ANDROID
	// As openAL is being linked as an static library it does not receive
	// JNI_OnLoad calls and cannot initialize its internal state form the Java Virtual Machine.
	// Here we simulate the initialization call.
	JNI_OnLoad(g_pAndroidApp->activity->vm,NULL);
#endif
	
	RTTRACE("CSoundSystemManager::Unserialize -> Initializing sound system");
	if(bOk) {m_pDevice=alcOpenDevice(NULL);bOk=(m_pDevice!=NULL);}
	RTTRACE("CSoundSystemManager::Unserialize -> Creating context");
	if(bOk) {m_pContext=alcCreateContext(m_pDevice,NULL);bOk=(m_pContext!=NULL);}
	RTTRACE("CSoundSystemManager::Unserialize -> Setting current context");
	if(bOk) {bOk=(alcMakeContextCurrent(m_pContext)==ALC_TRUE);}
	if(!bOk)
	{
		RTTRACE("CSoundSystemManager::Init -> Failed to create sound context %d",alGetError());
	}
	else
	{
		RTTRACE("CSoundSystemManager::Init -> Sound system initialized using device '%s'",m_sDeviceName==""?"default":m_sDeviceName.c_str());
	}
	
	if(bOk && m_pContext)
	{
		RTTRACE("CSoundSystemManager::Unserialize -> Configuring listener");
		alListenerf(AL_GAIN ,m_bMuted?0:((float)m_nMasterVolume)/(float)100.0);
	
		RTTRACE("CSoundSystemManager::Unserialize -> Creating sources");
		for(unsigned int x=0;x<m_nMaxSources;x++)
		{
			alGetError();
			ALuint nSource=AL_NONE;
			alGenSources(1,&nSource);
			ALenum error=alGetError();
			if(error==AL_NO_ERROR)
			{
				m_vFreeSources.push_back(nSource);
			}
			else
			{
				RTTRACE("CSoundSystemManager::Unserialize -> Warning: Only %d of %d sources can be created. Error %x:%s",x,m_nMaxSources);
				break;
			}
		}
	}
	return bOk;
}

void CSoundSystemManager::Destroy()
{	
	std::list<ALuint>::iterator f;
	for(f=m_vFreeSources.begin();f!=m_vFreeSources.end();f++)
	{
		ALuint nSource=*f;
		alDeleteSources(1,&nSource);
	}
	m_vFreeSources.clear();
	
	std::map<ALuint,ISoundType *>::iterator i;
	for(i=m_mBusySources.begin();i!=m_mBusySources.end();i++)
	{
		ALuint nSource=i->first;
		ISoundType *piType=i->second;
		piType->ReclaimSource(nSource);
		alDeleteSources(1,&nSource);
		REL(piType);
	}
	m_mBusySources.clear();

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

unsigned int CSoundSystemManager::AcquireSource(ISoundType *piSoundType)
{
	std::map<ALuint,ISoundType *>::iterator i;
	std::set<ALuint> sSourcesToDelete;
	for(i=m_mBusySources.begin();i!=m_mBusySources.end();i++)
	{
		ALuint nSource=i->first;
		ISoundType *piType=i->second;

		ALint nState=AL_STOPPED;
		alGetSourcei(nSource,AL_SOURCE_STATE,&nState);
		if(nState==AL_STOPPED)
		{
			piType->ReclaimSource(nSource);
			REL(piType);
			sSourcesToDelete.insert(nSource);
			m_vFreeSources.push_back(nSource);
		}
	}
	std::set<ALuint>::iterator d;
	for(d=sSourcesToDelete.begin();d!=sSourcesToDelete.end();d++)
	{
		m_mBusySources.erase(*d);
	}

	if(m_vFreeSources.size())
	{
		ALuint nSource=m_vFreeSources.front();
		m_vFreeSources.pop_front();
		m_mBusySources[nSource]=ADD(piSoundType);
		return nSource;
	}
	return AL_NONE;
}

void CSoundSystemManager::ReleaseSource(unsigned int nSource)
{
	if(nSource==AL_NONE){return;}
	
	std::map<ALuint,ISoundType *>::iterator i;
	i=m_mBusySources.find(nSource);
	if(i!=m_mBusySources.end())
	{
		ISoundType *piType=i->second;
		m_vFreeSources.push_back(nSource);
		m_mBusySources.erase(i);
		REL(piType);
	}
}

