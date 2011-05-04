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


#ifndef __PARTICLE_SYSTEMS__
#define __PARTICLE_SYSTEMS__

#include "GameRunTimeLib.h"
#include "GameManagers.h"

struct IParticle;
struct IParticleEmitter;
struct IParticleEmitterModifier;
struct IParticleModifier;
struct IParticleSystem;

struct IParticleType:virtual public ISystemUnknown
{
    virtual IParticle *CreateInstance(IParticleEmitter *piEmiter,unsigned int dwCurrentTime)=0;
};

struct IParticleSystemType:virtual public ISystemUnknown
{
    virtual IParticleSystem *CreateInstance(unsigned int dwCurrentTime)=0;
};

struct IParticleEmitterType:virtual public ISystemUnknown
{
    virtual IParticleEmitter *CreateInstance(unsigned int dwCurrentTime)=0;
};

struct IParticleEmitterModifierType:virtual public ISystemUnknown
{
	virtual IParticleEmitterModifier *CreateInstance(unsigned int dwCurrentTime)=0;
	virtual unsigned           GetEmitterNameCount()=0;
	virtual string             GetEmitterName(unsigned index)=0;
};

struct IParticleModifierType:virtual public ISystemUnknown
{
    virtual IParticleModifier *CreateInstance(unsigned int dwCurrentTime)=0;
    virtual unsigned           GetEmitterNameCount()=0;
    virtual string             GetEmitterName(unsigned index)=0;
};

struct IParticle
{
    SPhysicInfo m_PhysicInfo;
	CVector     m_vWhirlPoolVelocity;

    unsigned int       m_dwStartTime;
    unsigned int       m_dwEndTime;
    double      m_dLifeSpent; 

    bool        m_bActive;  // a 1 hasta que la particula se elimina
    bool        m_bNew;     // a 1 solo en el primer frame de la particula

    CVector     m_vPositionOnParent; // Posicion inicial en el sistema de referencia del padre
    bool        m_bFixedPositionOnParent; // Mantiene la posicion inicial en el sistema de referencia del padre, pensado para fuentes de luz y efectos estaticos respecto a la entidad

    IParticleEmitter *m_piEmiter; // emisor que genero la particula.

    // Propiedades para el render

    CVector     m_vColor;
    double      m_dAlpha;
    double      m_dSize;

    virtual void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera)=0;

    IParticle();
    virtual ~IParticle(){}
};


struct IParticleSystem
{
    virtual CVector GetPosition()=0;
    virtual void    SetPosition(CVector)=0;
    virtual CVector GetAngles()=0;
    virtual void    SetAngles(CVector)=0;

    virtual void    GetVectors(CVector &vForward,CVector &vRight,CVector &vUp)=0;

    virtual IParticleEmitter *GetEmitter(string sName)=0;

    virtual void AddEmitter(IParticleEmitter    *pParticleEmitter)=0;
    virtual void AddModifier(IParticleModifier  *pParticleModifier)=0;
	virtual void AddEmitterModifier(IParticleEmitterModifier    *pParticleEmitterModifier)=0;
	virtual void AddParticle(IParticle          *pParticle)=0;
    virtual void DeactivateAllEmitters()=0;

    virtual bool ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval)=0;
    virtual void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera)=0;

    virtual ~IParticleSystem(){}
};

struct IParticleEmitter
{
    virtual string  GetName()=0;
    virtual bool    IsActive()=0;
    virtual void    Deactivate()=0;
    virtual void    ProcessFrame(IParticleSystem *pSystem,unsigned int dwCurrentTime,double dInterval)=0;
	
	virtual CVector GetPosition()=0;
	virtual void    SetPosition(CVector vPosition)=0;
	
    virtual ~IParticleEmitter(){}
};

struct IParticleModifier
{
    virtual void AddEmitter(IParticleEmitter *piEmitter)=0;
    virtual void ProcessParticle(IParticle *pParticle,IParticleSystem *pSystem,unsigned int dwCurrentTime,double dInterval)=0;
 
    virtual ~IParticleModifier(){}
};

struct IParticleEmitterModifier
{
	virtual void AddEmitter(IParticleEmitter *piEmitter)=0;
	virtual void ProcessEmitter(IParticleEmitter *pParticleEmitter,IParticleSystem *pSystem,unsigned int dwCurrentTime,double dInterval)=0;
	
	virtual ~IParticleEmitterModifier(){}
};

#endif
