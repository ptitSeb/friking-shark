#pragma once
#include "GameRunTimeLib.h"

struct ISound;
struct ISoundType;

struct ISoundType:virtual public ISystemUnknown
{
    virtual ISound *CreateInstance()=0;

	virtual bool        Load(std::string sFileName)=0;
	virtual std::string GetFileName()=0;
};

struct ISound
{
    virtual void Play()=0;
    virtual void Stop()=0;
    virtual bool IsPlaying()=0;
	
	virtual void SetLoop(bool bLoop)=0;
	virtual void SetVolume(double nVolume)=0;
	
	virtual void SetPosition(CVector vPosition)=0;
	virtual void SetOrientation(CVector vOrientation)=0;
	virtual void SetVelocity(CVector vVelocity)=0;
	
    virtual ~ISound(){}
};

struct ISoundManager : virtual public ISystemUnknown
{
  virtual bool Is3DSoundEnabled()=0;

  virtual unsigned int  GetMasterVolume()=0;
  virtual void			SetMasterVolume(unsigned int dVolume)=0;
  
  virtual void SetListenerPosition(CVector vPosition)=0;
  virtual void SetListenerOrientation(CVector vOrientation)=0;
  virtual void SetListenerVelocity(CVector vVelocity)=0;
  
  virtual ~ISoundManager(){}
};

struct ISoundManagerEvents : virtual public ISystemUnknown
{
  virtual void OnMasterVolumeChanged(unsigned int dMasterVolume)=0;

  virtual ~ISoundManagerEvents(){}
};


double DeciblesToPercentage(double dDb);
double PercentageToDecibels(double dPercentage);