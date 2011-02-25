#pragma once
#include "GameRunTimeLib.h"

struct ISound;
struct ISoundType;

struct ISoundType:virtual public ISystemUnknown
{
    virtual ISound *CreateInstance(IEntity *piEntity,unsigned int dwCurrentTime)=0;
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

double DeciblesToPercentage(double dDb);
double PercentageToDecibels(double dPercentage);