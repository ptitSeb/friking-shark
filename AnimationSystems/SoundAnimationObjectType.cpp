#include "./stdafx.h"
#include "AnimationSystems.h"
#include "AnimationObjectBase.h"
#include "SoundSystems.h"
#include "SoundAnimationObjectType.h"

CSoundAnimationObjectType::CSoundAnimationObjectType()
{
  m_nStartTime=0;
  m_nEndTime=0;
  m_bLoop=false;
  m_dVolume=100;
}

CSoundAnimationObjectType::~CSoundAnimationObjectType()
{
}

IAnimationObject *CSoundAnimationObjectType::CreateInstance(IAnimation *piAnimation,unsigned int dwCurrentTime)
{
    CSoundAnimationObject *pSound=new CSoundAnimationObject(this,piAnimation);
    return pSound;
}

void CSoundAnimationObjectType::GetConfig(SSoundAnimationObjectTypeConfig *pConfig)
{
	pConfig->nStartTime=m_nStartTime;
	pConfig->nEndTime=m_nEndTime;
	pConfig->nVolume=m_dVolume;
	pConfig->bLoop=m_bLoop;
}

void CSoundAnimationObjectType::SetConfig(SSoundAnimationObjectTypeConfig *pConfig)
{
	m_nStartTime=pConfig->nStartTime;
	m_nEndTime=pConfig->nEndTime;
	m_dVolume=pConfig->nVolume;
	m_bLoop=pConfig->bLoop;
}

void CSoundAnimationObjectType::SetSound(ISoundType *piSound){m_SoundType.Attach(piSound);}
void CSoundAnimationObjectType::GetSound(ISoundType **ppiSound){(*ppiSound)=ADD(m_SoundType.m_piSoundType);}

CSoundAnimationObject::CSoundAnimationObject(CSoundAnimationObjectType *pType,IAnimation *piAnimation)
:CAnimationObjectBase(pType,piAnimation)
{
    m_piSound=NULL;
    m_pType=pType;
}
CSoundAnimationObject::~CSoundAnimationObject()
{
    if(m_piSound){delete m_piSound;m_piSound=NULL;}
}

void CSoundAnimationObject::Activate(unsigned int dwCurrentTime)
{
    if(m_piSound){delete m_piSound;m_piSound=NULL;}
    CAnimationObjectBase::Activate(dwCurrentTime);
	CheckActivation(dwCurrentTime);
}

void CSoundAnimationObject::CheckActivation(unsigned int dwCurrentTime)
{
	unsigned int dwRelativeTime=dwCurrentTime-m_piAnimation->GetCurrentTimeBase();
	if(m_piSound==NULL)
	{
		if(m_pType->m_SoundType.m_piSoundType && dwRelativeTime>=m_pType->m_nStartTime)
		{
			m_piSound=m_pType->m_SoundType.m_piSoundType->CreateInstance();
			if(m_piSound)
			{
				m_piSound->SetVolume(m_pType->m_dVolume);
				m_piSound->SetLoop(m_pType->m_bLoop);
				m_piSound->Play();
			}
		}
	}
	else
	{
		if(m_pType->m_SoundType.m_piSoundType && m_pType->m_nEndTime && dwRelativeTime>=m_pType->m_nEndTime)
		{
			Deactivate();
		}
	}
}

void CSoundAnimationObject::Deactivate()
{
    if(m_piSound){m_piSound->Stop();}
    CAnimationObjectBase::Deactivate();
}

void CSoundAnimationObject::CustomRender(IGenericRender *piRender,IGenericCamera *piCamera)
{
}

bool CSoundAnimationObject::ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval)
{
	CheckActivation(dwCurrentTime);
	if(m_piSound)
    {
      if(!m_piSound->IsPlaying())
      {
        return true;
      }
      else
      {
        m_piSound->Stop();
      }
    }
    return false;
}

