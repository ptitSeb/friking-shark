#pragma once
#include "GameRuntimeLib.h"

struct IAnimation;
struct IAnimationType;
struct IAnimationObject;
struct IAnimationObjectType;

struct IAnimationType:virtual public ISystemUnknown,virtual public IDesignObject
{
    virtual IAnimation *CreateInstance(IEntity *piEntity,DWORD dwCurrentTime)=0;
};

struct IAnimationObjectType:virtual public ISystemUnknown,virtual public IDesignObject
{
    virtual IAnimationObject *CreateInstance(IAnimation *piAnimation,DWORD dwCurrentTime)=0;
};

struct IAnimationObject
{
    virtual string GetName()=0;

    virtual void Activate(DWORD dwCurrentTime)=0;
    virtual void Deactivate()=0;
    virtual bool IsActive()=0;

    virtual IAnimation *GetAnimation()=0;

    virtual bool ProcessFrame(IPhysicManager *pPhysicManager,DWORD dwCurrentTime,double dInterval)=0;
    virtual void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera)=0;

	virtual CTraceInfo GetTrace(const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 )=0;

    virtual ~IAnimationObject(){}
};

struct IAnimation
{
    virtual void Activate(DWORD dwCurrentTime)=0;
    virtual void Deactivate()=0;
    virtual bool IsActive()=0;

    virtual bool HasFinished()=0;

    virtual IEntity *GetEntity()=0;

    virtual DWORD GetCurrentTimeBase()=0;

    virtual void              AddObject(IAnimationObject *pObject)=0;
    virtual IAnimationObject *GetObject(string sName)=0;

	virtual CTraceInfo GetTrace(const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 )=0;

    virtual bool ProcessFrame(IPhysicManager *pPhysicManager,DWORD dwCurrentTime,double dInterval)=0;
    virtual void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera)=0;

    virtual ~IAnimation(){}
};
