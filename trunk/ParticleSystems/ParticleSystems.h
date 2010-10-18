#include "GameRuntimeLib.h"

struct IParticle;
struct IParticleEmitter;
struct IParticleModifier;
struct IParticleSystem;

struct IParticleType:virtual public ISystemUnknown
{
    virtual IParticle *CreateInstance(IParticleEmitter *piEmiter,DWORD dwCurrentTime)=0;
};

struct IParticleSystemType:virtual public ISystemUnknown
{
    virtual IParticleSystem *CreateInstance(DWORD dwCurrentTime)=0;
};

struct IParticleEmitterType:virtual public ISystemUnknown
{
    virtual IParticleEmitter *CreateInstance(DWORD dwCurrentTime)=0;
};

struct IParticleModifierType:virtual public ISystemUnknown
{
    virtual IParticleModifier *CreateInstance(DWORD dwCurrentTime)=0;
    virtual unsigned           GetEmitterNameCount()=0;
    virtual string             GetEmitterName(unsigned index)=0;
};

struct IParticle
{
    SPhysicInfo m_PhysicInfo;

    DWORD       m_dwStartTime;
    DWORD       m_dwEndTime;
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
    virtual void AddParticle(IParticle          *pParticle)=0;
    virtual void DeactivateAllEmitters()=0;

    virtual bool ProcessFrame(IPhysicManager *pPhysicManager,DWORD dwCurrentTime,double dInterval)=0;
    virtual void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera)=0;

    virtual ~IParticleSystem(){}
};

struct IParticleEmitter
{
    virtual string  GetName()=0;
    virtual bool    IsActive()=0;
    virtual void    Deactivate()=0;
    virtual void    ProcessFrame(IParticleSystem *pSystem,DWORD dwCurrentTime,double dInterval)=0;

    virtual ~IParticleEmitter(){}
};

struct IParticleModifier
{
    virtual void AddEmitter(IParticleEmitter *piEmitter)=0;
    virtual void ProcessParticle(IParticle *pParticle,IParticleSystem *pSystem,DWORD dwCurrentTime,double dInterval)=0;
 
    virtual ~IParticleModifier(){}
};