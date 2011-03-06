#include "./stdafx.h"
#include "SoundSystems.h"
#include "SoundType.h"

CSoundType::CSoundType(void)
{
  m_bLoop=false;
  m_nChannels=5;
  m_piSoundManager=NULL;
  m_iSoundBuffer=AL_NONE;
  m_dVolume=100;
}

CSoundType::~CSoundType(void)
{
  REL(m_piSoundManager);
}

bool CSoundType::Unserialize(ISystemPersistencyNode *piNode)
{
	bool bOk=CSystemObjectBase::Unserialize(piNode);
	if(!bOk){return false;}

	m_iSoundBuffer=alutCreateBufferFromFile(m_sFileName.c_str());
	if(m_iSoundBuffer!=AL_NONE)
	{
	  RTTRACE("CSoundType::CreateInstance -> Loaded Sound %s",m_sFileName.c_str());
	  
	  ALuint *pSources=new ALuint [m_nChannels];
	  alGetError();
	  alGenSources(m_nChannels,pSources);
	  if(alGetError()==AL_NO_ERROR)
	  {
		for(unsigned int x=0;x<m_nChannels;x++)
		{
		  alSourcei(pSources[x],AL_BUFFER,m_iSoundBuffer);
		  alSourcef(pSources[x],AL_GAIN,m_dVolume/100.0);
	  
		  m_dAvailableSources.push_back(pSources[x]);
		}
	  }
	  else
	  {
		RTTRACE("CSoundType::CreateInstance -> Failed to create sources. Error %d",alGetError());
	  }
	  delete [] pSources;
	  pSources=NULL;
	}
	bOk=(m_dAvailableSources.size()!=0);
	if(!bOk)
	{
	  RTTRACE("CSoundType::CreateInstance -> Failed to load sound %s. Error %d:%s",m_sFileName.c_str(),alutGetError(),alutGetErrorString(alutGetError()));
	}
	return bOk;
}

bool CSoundType::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
  bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);;
  if(bOk)
  {
    ISystemManager  *piSystemManager=GetSystemManager();
    ISystem         *piTempSystem=NULL;
    ISystemObject   *piObject=NULL;
    if(piSystemManager){piTempSystem=piSystemManager->GetSystem("GameSystem");}
    if(piTempSystem){piTempSystem->GetObject("SoundManager",&piObject);}
    if(piObject){m_piSoundManager=QI(ISoundManager,piObject);}
    REL(piSystemManager);
    REL(piTempSystem);
    REL(piObject);
    bOk=m_piSoundManager!=NULL;
  }
  return bOk;
}

void CSoundType::Destroy()
{
  for(size_t x=0;x<m_dAvailableSources.size();x++)
  {
	ALuint nSource=m_dAvailableSources[x];
	if(nSource){alDeleteSources(1,&nSource);}
  }
  m_dAvailableSources.clear();
  
  if(m_iSoundBuffer){alDeleteBuffers(1,&m_iSoundBuffer);m_iSoundBuffer=AL_NONE;}

  CSystemObjectBase::Destroy();
}

ISound *CSoundType::CreateInstance(IEntity *piEntity,unsigned int dwCurrentTime)
{
  if(m_piSoundManager==NULL)
  {
    return NULL;
  }
  CSound *pSound=new CSound(this,piEntity,dwCurrentTime);
  return pSound;
}

ALuint CSoundType::AcquireSoundSource()
{
  if(m_dAvailableSources.size())
  {
    ALuint nSource=m_dAvailableSources.front();
    m_dAvailableSources.pop_front();
    return nSource;
  }
  return AL_NONE;
}

void CSoundType::ReleaseSoundSource(ALuint nSource)
{
  if(nSource!=AL_NONE)
  {
	m_dAvailableSources.push_back(nSource);
  }
}


CSound::CSound(CSoundType *pType,IEntity *piEntity,unsigned int dwCurrentTimeBase)
{
  m_bActive=false;
  m_nSource=AL_NONE;
  m_pType=pType;
  m_piEntity=piEntity;
  SUBSCRIBE_TO_CAST(pType->m_piSoundManager,ISoundManagerEvents);
}

CSound::~CSound()
{
    if(m_nSource!=AL_NONE)
    {
	  alSourceStop(m_nSource);
      m_pType->ReleaseSoundSource(m_nSource);
	  m_nSource=AL_NONE;
    }
    UNSUBSCRIBE_FROM_CAST(m_pType->m_piSoundManager,ISoundManagerEvents);
}

void CSound::Activate(unsigned int dwCurrentTime)
{
  m_bActive=true;
  if(m_nSource==AL_NONE)
  {
    m_nSource=m_pType->AcquireSoundSource();
    if(m_nSource!=AL_NONE)
	{
      alSourcei(m_nSource,AL_LOOPING,m_pType->m_bLoop?AL_TRUE:AL_FALSE);
	  alSourcePlay(m_nSource);
    }
  }
}

bool CSound::IsActive()
{
  return m_bActive;
}

void CSound::Deactivate()
{
  if(m_pType->m_bLoop)
  {
    if(m_nSource!=AL_NONE)
    {
	  alSourceStop(m_nSource);
      m_pType->ReleaseSoundSource(m_nSource);
	  m_nSource=AL_NONE;
    }
  }
  m_bActive=false;
}

bool CSound::HasFinished()
{
  if(m_nSource==AL_NONE){return true;}
  ALint nState=AL_STOPPED;
  alGetSourcei(m_nSource,AL_SOURCE_STATE,&nState);
  if(nState==AL_PLAYING || nState==AL_INITIAL){return false;}
  
  m_pType->ReleaseSoundSource(m_nSource);
  m_nSource=AL_NONE;
  return true;
}

bool CSound::ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval)
{
  if(m_nSource==AL_NONE){return false;}
  if(m_pType->m_piSoundManager->Is3DSoundEnabled())
  {
    if(m_piEntity)
    {
      SPhysicInfo *pPhysicInfo=m_piEntity->GetPhysicInfo();
	  alSourcei(m_nSource,AL_CONE_INNER_ANGLE,360);
	  alSourcei(m_nSource,AL_CONE_OUTER_ANGLE,360);
	  alSourcei(m_nSource,AL_SOURCE_RELATIVE,AL_FALSE);
	  alSource3f(m_nSource,AL_POSITION,(float)pPhysicInfo->vPosition.c[0],(float)pPhysicInfo->vPosition.c[1],(float)pPhysicInfo->vPosition.c[2]);
	  alSource3f(m_nSource,AL_VELOCITY,(float)pPhysicInfo->vVelocity.c[0],(float)pPhysicInfo->vVelocity.c[1],(float)pPhysicInfo->vVelocity.c[2]);
	  alSource3f(m_nSource,AL_DIRECTION,(float)pPhysicInfo->vForward.c[0],(float)pPhysicInfo->vForward.c[1],(float)pPhysicInfo->vForward.c[2]);
    }
  }
  return false;
}
