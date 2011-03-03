#include "./stdafx.h"
#include ".\SoundSystems.h"
#include ".\SoundType.h"

CSoundType::CSoundType(void)
{
  m_dMinDistance=30;
  m_dMaxDistance=100;
  m_bCyclic=false;
  m_dwMaxSimultaneousSounds=5;
  m_piSoundManager=NULL;
  m_bSoundsAcquired=false;
  m_pSound=NULL;
  m_dPan=0;
  m_dFrequency=DSBFREQUENCY_ORIGINAL;
  m_dVolume=100;
}

CSoundType::~CSoundType(void)
{
  REL(m_piSoundManager);
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
  for(size_t x=0;x<m_dAvailableBuffers.size();x++)
  {
    IDirectSoundBuffer8 *piSound=m_dAvailableBuffers[x];
    piSound->Release();
    piSound=NULL;
  }
  m_dAvailableBuffers.clear();

  if(m_pSound){delete m_pSound;m_pSound=NULL;}

  CSystemObjectBase::Destroy();
}

ISound *CSoundType::CreateInstance(IEntity *piEntity,unsigned int dwCurrentTime)
{
  if(m_piSoundManager==NULL)
  {
    return NULL;
  }
  if(!m_bSoundsAcquired)
  {
    m_bSoundsAcquired=true;

    CHAR wcName[MAX_PATH]={0};
    sprintf(wcName,"%s",m_sFileName.c_str());
    unsigned int dwFlags=DSBCAPS_CTRLPAN|DSBCAPS_CTRLFREQUENCY|DSBCAPS_CTRLVOLUME;
    if(m_piSoundManager->Is3DSoundEnabled()){dwFlags|=DSBCAPS_CTRL3D;dwFlags&=~DSBCAPS_CTRLPAN;}
    HRESULT hr=m_piSoundManager->GetSoundManager()->Create(&m_pSound,wcName,dwFlags,GUID_NULL,m_dwMaxSimultaneousSounds);
    if(SUCCEEDED(hr))
    {
      unsigned int x;
      for(x=0;x<m_dwMaxSimultaneousSounds;x++)
       {
          IDirectSoundBuffer *piSoundBuffer=m_pSound->GetBuffer(x);
          // Las interfaces devueltas por getbuffer vienen sin referenciar
          piSoundBuffer->AddRef();
          if(piSoundBuffer)
          {
            IDirectSoundBuffer8 *piSoundBuffer8=NULL;
            hr=piSoundBuffer->QueryInterface(IID_IDirectSoundBuffer8,(void**)&piSoundBuffer8);
            if(SUCCEEDED(hr))
            {
              m_dAvailableBuffers.push_back(piSoundBuffer8);
            }
            LONG dwVolume=1;
            piSoundBuffer->Release();
            piSoundBuffer=NULL;
          }
       }
    }
  }

  CSoundObject *pSound=new CSoundObject(this,piEntity,dwCurrentTime);
  return pSound;
}

IDirectSoundBuffer8 *CSoundType::AcquireSoundBuffer()
{
  if(m_dAvailableBuffers.size())
  {
    IDirectSoundBuffer8 *piSound=m_dAvailableBuffers.front();
    m_dAvailableBuffers.pop_front();
    return piSound;
  }
  return NULL;
}

void CSoundType::ReleaseSoundBuffer(IDirectSoundBuffer8 *piSound)
{
  piSound->AddRef();
  m_dAvailableBuffers.push_back(piSound);
}


CSoundObject::CSoundObject(CSoundType *pType,IEntity *piEntity,unsigned int dwCurrentTimeBase)
{
  m_bActive=false;
  m_piSound=NULL;
  m_piSound3D=NULL;
  m_pType=pType;
  m_piEntity=piEntity;
  SUBSCRIBE_TO_CAST(pType->m_piSoundManager,ISoundManagerEvents);
}

CSoundObject::~CSoundObject()
{
    if(m_piSound)
    {
      m_piSound->Stop();
      m_pType->ReleaseSoundBuffer(m_piSound);
      if(m_piSound){m_piSound->Release();m_piSound=NULL;}
      if(m_piSound3D){m_piSound3D->Release();m_piSound3D=NULL;}
    }
    UNSUBSCRIBE_FROM_CAST(m_pType->m_piSoundManager,ISoundManagerEvents);
}

void CSoundObject::Activate(unsigned int dwCurrentTime)
{
  m_bActive=true;
  if(!m_piSound)
  {
    m_piSound=m_pType->AcquireSoundBuffer();
    if(m_piSound){m_piSound->QueryInterface(IID_IDirectSound3DBuffer8,(void**)&m_piSound3D);}
    if(m_piSound)
    {
      UpdateSound();
      m_piSound->Play(0,0,m_pType->m_bCyclic?DSBPLAY_LOOPING:0);
    }
  }
}

bool CSoundObject::IsActive()
{
  return m_bActive;
}

void CSoundObject::Deactivate()
{
  if(m_pType->m_bCyclic)
  {
    if(m_piSound)
    {
      m_piSound->Stop();
      m_pType->ReleaseSoundBuffer(m_piSound);
    if(m_piSound){m_piSound->Release();m_piSound=NULL;}
    if(m_piSound3D){m_piSound3D->Release();m_piSound3D=NULL;}
    }
  }
  m_bActive=false;
}

bool CSoundObject::HasFinished()
{
  if(!m_piSound){return true;}
  unsigned int dwStatus=0;
  m_piSound->GetStatus( (DWORD*)&dwStatus );
  if(dwStatus&DSBSTATUS_PLAYING)
  {
     return false;
  }

  m_pType->ReleaseSoundBuffer(m_piSound);
  if(m_piSound){m_piSound->Release();m_piSound=NULL;}
  if(m_piSound3D){m_piSound3D->Release();m_piSound3D=NULL;}
  return true;
}

void CSoundObject::UpdateSound()
{
  if(m_piSound)
  {
    double dFinalVolume=m_pType->m_dVolume*m_pType->m_piSoundManager->GetMasterVolume()/100;
    double dAttenuation=PercentageToDecibels(100-dFinalVolume);
    HRESULT hr=m_piSound->SetVolume(0-(LONG)dAttenuation);// rango DX: 0 , -10000
    hr=m_piSound->SetPan(m_pType->m_dPan*100);// rango DX: -10000 , +10000
    hr=m_piSound->SetFrequency(m_pType->m_dFrequency);
  }
}

bool CSoundObject::ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval)
{
  HRESULT hr=S_OK;
  if(m_pType->m_piSoundManager->Is3DSoundEnabled())
  {
    if(m_piEntity && m_piSound3D)
    {
      SPhysicInfo *pPhysicInfo=m_piEntity->GetPhysicInfo();
      DS3DBUFFER dsBuffer={0};
      dsBuffer.dwSize=sizeof(dsBuffer);
      hr=m_piSound3D->GetAllParameters(&dsBuffer);
      dsBuffer.dwInsideConeAngle=360;
      dsBuffer.dwOutsideConeAngle=360;
      dsBuffer.dwMode=DS3DMODE_NORMAL;
      dsBuffer.flMaxDistance=(float)m_pType->m_dMaxDistance;
      dsBuffer.flMinDistance=(float)m_pType->m_dMinDistance;
      dsBuffer.vPosition.x=(float)pPhysicInfo->vPosition.c[0];
      //dsBuffer.vPosition.y=(float)pPhysicInfo->vPosition.c[1];
      dsBuffer.vPosition.z=(float)pPhysicInfo->vPosition.c[2];
      dsBuffer.vVelocity.x=(float)pPhysicInfo->vVelocity.c[0];
      //dsBuffer.vVelocity.y=(float)pPhysicInfo->vVelocity.c[1];
      dsBuffer.vVelocity.z=(float)pPhysicInfo->vVelocity.c[2];
      hr=m_piSound3D->SetAllParameters(&dsBuffer,DS3D_IMMEDIATE);
    }
  }
  return false;
}

void CSoundObject::OnMasterPanChanged(int dMasterPan)
{
  UpdateSound();
}

void CSoundObject::OnMasterVolumeChanged(int dMasterVolume)
{
  UpdateSound();
}
