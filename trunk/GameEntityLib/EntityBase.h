#pragma once

#include "GameRunTimeLib.h"
#include "AnimationSystems.h"
#include "EntityTypeBase.h"
#include "SteeringBehaviours.h"

class CEntityBase: public IEntity, public CPublisherBase, public CSubscriberBase
{
protected:
	CSteeringBehaviours m_Behaviours;

    IEntityManager *GetEntityManager();
    IPhysicManager *GetPhysicManager();
    IFrameManager  *GetFrameManager();

    CEntityTypeBase *m_pTypeBase;

    string	m_sName;
    string	m_sClassName;

    // Miembros de Fisica

    SPhysicInfo m_PhysicInfo;

    // Miembros para el control de tiempos

    unsigned int	m_dwCreationTime;
    unsigned int	m_dwNextProcessFrame;

    // Miembros para la aplicacion de daños

    unsigned int   m_dwDamageType;
    double	m_dDamage;
    double	m_dMaxHealth;
    double	m_dHealth;

		double  m_dLastFrameDamage;

    bool	m_bRemoved;

    // Miembros de Animaciones
    vector<IAnimation*>  m_dAnimations;
    int                  m_nCurrentAnimation;

    // Miembros para gestion de Armamento

    vector<IWeapon*>     m_dWeapons;

    // Alineacion (Bando) de la entidad

    unsigned int m_dwAlignment;

    // Miembors de Rutas
    IRoute  *m_piRoute;

    IEntity *m_piTarget;

    void FireWeapon(unsigned int dwWeaponSlot,unsigned int dwCurrentTime);
    void OnKilledInternal(bool bRemove);

public:

    string      *GetEntityClass();
    string      *GetEntityName();
    SPhysicInfo *GetPhysicInfo();

    unsigned int       GetAlignment();
    void        SetAlignment(unsigned int dwAlignment);

    bool        IsRemoved();
    void        Remove();

    // Render
    void Render(CMaterial *pMaterial){}
    void Render(IGenericRender *piRender,IGenericCamera *piCamera);

    // Procesamiento
    unsigned int GetNextProcessFrame();
    void  ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

    // Colisiones y Daños
    double GetHealth();
    double GetMaxHealth();
	unsigned int  GetDamageType();

	CTraceInfo GetTrace(const CVector &p1,const CVector &p2);

    bool OnCollision(IEntity *pOther,CVector &vCollisionPos); 
    void OnDamage(double dDamage,IEntity *pAggresor);
    virtual void OnKilled();

	// Steering Behaviours

    // Animaciones
    void AddAnimation(unsigned index,IAnimation *piAnimation);
    void SetCurrentAnimation(int index);
    int  GetCurrentAnimation();
    void ProcessAnimations(unsigned int dwCurrentTime,double dTimeFraction,bool *pbAnimationsFinished);

    void OnAnimationEvent(string sEvent,string sParams);

    // Armas
    void AddWeapon(IWeapon *piWeapon);

    // Rutas
    void SetRoute(IRoute *piRoute);

    IEntity *GetTarget();

           CEntityBase();
    virtual ~CEntityBase();
};


