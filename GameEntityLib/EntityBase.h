#pragma once

#include "GameRuntimeLib.h"
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

    DWORD	m_dwCreationTime;
    DWORD	m_dwNextProcessFrame;

    // Miembros para la aplicacion de daños

    DWORD   m_dwDamageType;
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

    DWORD m_dwAlignment;

    // Miembors de Rutas
    IRoute  *m_piRoute;

    IEntity *m_piTarget;

    void FireWeapon(DWORD dwWeaponSlot,DWORD dwCurrentTime);
    void OnKilledInternal(bool bRemove);

public:

    string      *GetEntityClass();
    string      *GetEntityName();
    SPhysicInfo *GetPhysicInfo();

    DWORD       GetAlignment();
    void        SetAlignment(DWORD dwAlignment);

    bool        IsRemoved();
    void        Remove();

    // Render
    void Render(CMaterial *pMaterial){}
    void Render(IGenericRender *piRender,IGenericCamera *piCamera);

    // Procesamiento
    DWORD GetNextProcessFrame();
    void  ProcessFrame(DWORD dwCurrentTime,double dTimeFraction);

    // Colisiones y Daños
    double GetHealth();
    double GetMaxHealth();
	DWORD  GetDamageType();

	CTraceInfo GetTrace(const CVector &p1,const CVector &p2);

    bool OnCollision(IEntity *pOther,CVector &vCollisionPos); 
    void OnDamage(double dDamage,IEntity *pAggresor);
    virtual void OnKilled();

	// Steering Behaviours

    // Animaciones
    void AddAnimation(unsigned index,IAnimation *piAnimation);
    void SetCurrentAnimation(int index);
    int  GetCurrentAnimation();
    void ProcessAnimations(DWORD dwCurrentTime,double dTimeFraction,bool *pbAnimationsFinished);

    void OnAnimationEvent(string sEvent,string sParams);

    // Armas
    void AddWeapon(IWeapon *piWeapon);

    // Rutas
    void SetRoute(IRoute *piRoute);

    IEntity *GetTarget();

           CEntityBase();
    virtual ~CEntityBase();
};


