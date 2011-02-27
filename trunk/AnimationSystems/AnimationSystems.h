#ifndef __ANIMATION_SYSTEMS__
#define __ANIMATION_SYSTEMS__

#include "GameManagers.h"

struct IAnimation;
struct IAnimationType;
struct IAnimationObject;
struct IAnimationObjectType;

struct IAnimationType:virtual public ISystemUnknown,virtual public IDesignObject
{
    virtual IAnimation *CreateInstance(IEntity *piEntity,unsigned int dwCurrentTime)=0;
};

struct IAnimationObjectType:virtual public ISystemUnknown,virtual public IDesignObject
{
    virtual IAnimationObject *CreateInstance(IAnimation *piAnimation,unsigned int dwCurrentTime)=0;
};

struct IAnimationObject
{
    virtual string GetName()=0;

    virtual void Activate(unsigned int dwCurrentTime)=0;
    virtual void Deactivate()=0;
    virtual bool IsActive()=0;

    virtual IAnimation *GetAnimation()=0;

    virtual bool ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval)=0;
    virtual void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera)=0;

	virtual CTraceInfo GetTrace(const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 )=0;

    virtual ~IAnimationObject(){}
};

struct IAnimation
{
    virtual void Activate(unsigned int dwCurrentTime)=0;
    virtual void Deactivate()=0;
    virtual bool IsActive()=0;

    virtual bool HasFinished()=0;

    virtual IEntity *GetEntity()=0;

    virtual unsigned int GetCurrentTimeBase()=0;

    virtual void              AddObject(IAnimationObject *pObject)=0;
    virtual IAnimationObject *GetObject(string sName)=0;

	virtual CTraceInfo GetTrace(const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 )=0;

    virtual bool ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval)=0;
    virtual void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera)=0;

    virtual ~IAnimation(){}
};

#endif