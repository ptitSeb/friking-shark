#pragma once
#include "GameRunTimeLib.h"

struct ISound;
struct ISoundType;

struct ISoundType:virtual public ISystemUnknown
{
    virtual ISound *CreateInstance(IEntity *piEntity,unsigned int dwCurrentTime)=0;

	virtual bool        Load(std::string sFileName)=0;
	virtual std::string GetFileName()=0;
};

struct ISound
{
    virtual void Activate(unsigned int dwCurrentTime)=0;
    virtual void Deactivate()=0;
    virtual bool IsActive()=0;

    virtual bool HasFinished()=0;

    virtual bool ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval)=0;

    virtual ~ISound(){}
};

struct ISoundManager : virtual public ISystemUnknown
{
  virtual bool Is3DSoundEnabled()=0;

  virtual unsigned int  GetMasterVolume()=0;
  virtual void			SetMasterVolume(unsigned int dVolume)=0;

  virtual ~ISoundManager(){}
};

struct ISoundManagerEvents : virtual public ISystemUnknown
{
  virtual void OnMasterVolumeChanged(unsigned int dMasterVolume)=0;

  virtual ~ISoundManagerEvents(){}
};


double DeciblesToPercentage(double dDb);
double PercentageToDecibels(double dPercentage);