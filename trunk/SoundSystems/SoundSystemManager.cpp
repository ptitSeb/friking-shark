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
}

CSoundSystemManager::~CSoundSystemManager()
{
}

bool CSoundSystemManager::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
  bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);;
  if(bOk) {m_pDevice=alcOpenDevice(NULL);bOk=(m_pDevice!=NULL);}
  if(bOk) {m_pContext=alcCreateContext(m_pDevice,NULL);bOk=(m_pContext!=NULL);}
  if(bOk) {bOk=alcMakeContextCurrent(m_pContext);}
  if(bOk) {bOk=alutInitWithoutContext(NULL,NULL);}
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
		alListenerf(AL_GAIN ,((float)m_nMasterVolume)/(float)100.0);
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
	if(!Is3DSoundEnabled())
	{
		alListener3f(AL_POSITION,(float)m_vListenerPosition.c[0],(float)m_vListenerPosition.c[1],(float)m_vListenerPosition.c[2]);
		alListener3f(AL_VELOCITY,(float)m_vListenerVelocity.c[0],(float)m_vListenerVelocity.c[1],(float)m_vListenerVelocity.c[2]);
		alListener3f(AL_ORIENTATION,(float)m_vListenerOrientation.c[0],(float)m_vListenerOrientation.c[1],(float)m_vListenerOrientation.c[2]);
	}
}

unsigned int  CSoundSystemManager::GetMasterVolume(){return m_nMasterVolume;}
bool CSoundSystemManager::Is3DSoundEnabled(){return m_bEnable3DSound;}

void CSoundSystemManager::SetMasterVolume(unsigned int nVolume)
{	
	if(m_pContext)
	{
		alListenerf(AL_GAIN ,((float)m_nMasterVolume)/(float)100.0);
		m_nMasterVolume=nVolume;
		NOTIFY_EVENT(ISoundManagerEvents,OnMasterVolumeChanged(m_nMasterVolume));
	}
}

void CSoundSystemManager::SetListenerPosition(CVector vPosition){m_vListenerPosition=vPosition;UpdateListener();}
void CSoundSystemManager::SetListenerOrientation(CVector vOrientation){m_vListenerOrientation=vOrientation;UpdateListener();}
void CSoundSystemManager::SetListenerVelocity(CVector vVelocity){m_vListenerVelocity=vVelocity;UpdateListener();}

