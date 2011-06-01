//  Friking Shark, a Flying Shark arcade remake.
//  Copyright (C) 2011  Javier Martin Garcia (javiermartingarcia@gmail.com)
//	
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


#ifndef __ANIMATION_SYSTEMS__
#define __ANIMATION_SYSTEMS__

#include "GameManagers.h"

struct IAnimation;
struct IAnimationType;
struct IAnimationObject;
struct IAnimationObjectType;
struct IParticleSystemType;
class  ISoundType;
struct IGenericFont;

#define OBJECT_INVALID (unsigned int)(-1)

struct SAnimationTypeConfig
{
	bool bLoop;

	SAnimationTypeConfig(){bLoop=false;}
};

struct SEventAnimationObjectTypeConfig
{
	string      	sName;
	string      	sParams;
	unsigned int    nTime;

	SEventAnimationObjectTypeConfig(){nTime=0;}
};

struct SEntityAnimationObjectTypeConfig
{
	unsigned int    nTime;
	
	SEntityAnimationObjectTypeConfig(){nTime=0;}
};

struct SModelAnimationKeyFrame
{
	unsigned int nTime;
	CVector vAngles;
	CVector vPosition;
};

struct SModelAnimationObjectTypeConfig
{
	unsigned int nStartTime;
	unsigned int nEndTime;
	float 		 dFps;
	bool 		 bLoop;
	bool 		 bCastShadow;
	bool 		 bReceiveShadows;
	std::vector<SModelAnimationKeyFrame> vKeyFrames;
	
	SModelAnimationObjectTypeConfig(){dFps=50.0;nStartTime=0;nEndTime=0;bLoop=false;bCastShadow=true;bReceiveShadows=true;}
};

struct SParticleSystemAnimationObjectTypeConfig
{
	unsigned int    nStartTime;
	unsigned int    nEndTime;
	bool            bTrackEntity;

	SParticleSystemAnimationObjectTypeConfig(){nStartTime=0;nEndTime=0;bTrackEntity=false;}
};

struct SSoundAnimationObjectTypeConfig
{
	unsigned int nStartTime;
	unsigned int nEndTime;
	bool 		 bLoop;
	unsigned int nVolume;

	SSoundAnimationObjectTypeConfig(){nStartTime=0;nEndTime=0;nVolume=100;bLoop=false;}
};

struct STextAnimationObjectTypeConfig
{
	unsigned int nStartTime;
	unsigned int nEndTime;
	std::string  sText;
	double       dFontSize;
	
	STextAnimationObjectTypeConfig(){nStartTime=0;nEndTime=0;dFontSize=0;}
};

struct IAnimationType:virtual public ISystemUnknown,virtual public IDesignObject
{
    virtual IAnimation *CreateInstance(IEntity *piEntity,unsigned int dwCurrentTime)=0;
};

struct IAnimationObjectType:virtual public ISystemUnknown,virtual public IDesignObject
{
    virtual IAnimationObject *CreateInstance(IAnimation *piAnimation,unsigned int dwCurrentTime)=0;
	
	virtual std::string GetAnimationObjectDescription()=0;
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

struct IAnimationTypeDesign:virtual public ISystemUnknown
{
	virtual void		 GetAnimationTypeConfig(SAnimationTypeConfig *pConfig)=0;
	virtual void		 SetAnimationTypeConfig(SAnimationTypeConfig *pConfig)=0;

	// Object management

	virtual unsigned int	AddObject(std::string sObjectType)=0;
	virtual bool			RemoveObject(unsigned int nObject)=0;
	virtual bool			GetObject(unsigned int nObject,IAnimationObjectType **ppiObject)=0;
	virtual unsigned int	GetObjectCount()=0;
};

struct IAnimationObjectTypePositionDesign:virtual public IAnimationObjectType
{
	virtual CVector GetPosition()=0;
	virtual void	SetPosition(CVector vPosition)=0;
};

struct IAnimationObjectTypeOrientationDesign:virtual public IAnimationObjectType
{
	virtual CVector GetAngles()=0;
	virtual void	SetAngles(CVector vAngles)=0;
};

struct IEventAnimationObjectTypeDesign:virtual public IAnimationObjectType
{
	virtual void		 GetConfig(SEventAnimationObjectTypeConfig *pConfig)=0;
	virtual void		 SetConfig(SEventAnimationObjectTypeConfig *pConfig)=0;
};


struct IModelAnimationObjectTypeDesign:virtual public IAnimationObjectType
{
	virtual void		 GetConfig(SModelAnimationObjectTypeConfig *pConfig)=0;
	virtual void		 SetConfig(SModelAnimationObjectTypeConfig *pConfig)=0;
	virtual void		 SetModel(IGenericModel *piModel)=0;
	virtual void		 GetModel(IGenericModel **ppiModel)=0;
};

struct IParticleSystemAnimationObjectTypeDesign:virtual public IAnimationObjectType
{
	virtual void		 GetConfig(SParticleSystemAnimationObjectTypeConfig *pConfig)=0;
	virtual void		 SetConfig(SParticleSystemAnimationObjectTypeConfig *pConfig)=0;
	virtual void		 SetParticleSystemType(IParticleSystemType *piParticleSystemType)=0;
	virtual void		 GetParticleSystemType(IParticleSystemType **ppiParticleSystemType)=0;
};

struct IEntityAnimationObjectTypeDesign:virtual public IAnimationObjectType
{
	virtual void		 GetConfig(SEntityAnimationObjectTypeConfig *pConfig)=0;
	virtual void		 SetConfig(SEntityAnimationObjectTypeConfig *pConfig)=0;
	virtual void		 SetEntityType(IEntityType *piEntityType)=0;
	virtual void		 GetEntityType(IEntityType **ppiEntityType)=0;
};

struct ISoundAnimationObjectTypeDesign:virtual public IAnimationObjectType
{
	virtual void		 GetConfig(SSoundAnimationObjectTypeConfig *pConfig)=0;
	virtual void		 SetConfig(SSoundAnimationObjectTypeConfig *pConfig)=0;
	virtual void		 SetSound(ISoundType *piSoundType)=0;
	virtual void		 GetSound(ISoundType **ppiSoundType)=0;
};

struct ITextAnimationObjectTypeDesign:virtual public IAnimationObjectType
{
	virtual void		 GetConfig(STextAnimationObjectTypeConfig *pConfig)=0;
	virtual void		 SetConfig(STextAnimationObjectTypeConfig *pConfig)=0;
	virtual void		 SetFont(IGenericFont *piFont)=0;
	virtual void		 GetFont(IGenericFont **ppiFont)=0;
};

#endif