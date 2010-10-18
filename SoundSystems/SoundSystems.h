#pragma once
#include "GameRuntimeLib.h"

struct ISound;
struct ISoundType;

struct ISoundType:virtual public ISystemUnknown
{
    virtual ISound *CreateInstance(IEntity *piEntity,DWORD dwCurrentTime)=0;
};

struct ISound
{
    virtual void Activate(DWORD dwCurrentTime)=0;
    virtual void Deactivate()=0;
    virtual bool IsActive()=0;

    virtual bool HasFinished()=0;

    virtual bool ProcessFrame(IPhysicManager *pPhysicManager,DWORD dwCurrentTime,double dInterval)=0;

    virtual ~ISound(){}
};

double DeciblesToPercentage(double dDb);
double PercentageToDecibels(double dPercentage);