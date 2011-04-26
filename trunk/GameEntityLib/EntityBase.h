#pragma once

#include "GameRunTimeLib.h"
#include "AnimationSystems.h"
#include "EntityTypeBase.h"
#include "SteeringBehaviours.h"
#include "GameManagers.h"

struct SChildEntity
{
	CVector vPosition;
	CVector vAngles;
	IEntity *piEntity;
	
	SChildEntity(){piEntity=NULL;}
};

class CEntityBase: public IEntity, public CPublisherBase, public CSubscriberBase
{
protected:
	CSteeringBehaviours m_Behaviours;

    IEntityManager *GetEntityManager();
    
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
    vector<IAnimation*>  m_vActiveAnimations;
    unsigned int         m_nCurrentState;
	unsigned int         m_nCurrentStateAnimation;
	
    // Miembros para gestion de Armamento

    vector<IWeapon*>     m_vWeapons;

	// Miembros para gestion de jerarquia
	
	IEntity 			*m_piParent;
	vector<SChildEntity> m_vChildren;
	
    // Alineacion (Bando) de la entidad

    unsigned int m_dwAlignment;

    // Miembors de Rutas
    IRoute  *m_piRoute;

    IEntity *m_piTarget;

    void FireWeapon(unsigned int dwWeaponSlot,unsigned int dwCurrentTime);
    void OnKilledInternal(bool bRemove);
	
public:
	
	void 		  SetState(unsigned int nState,unsigned int nAnimation=ANIMATION_RANDOM);
	unsigned int  GetState();
	
	void 		  SetEntityTypeBase(CEntityTypeBase *pTypeBase);
	
    string      *GetEntityClass();
    string      *GetEntityName();
    SPhysicInfo *GetPhysicInfo();

    unsigned int	GetAlignment();
    void        	SetAlignment(unsigned int dwAlignment);
	
	IEntity      *GetParent();
	void          SetParent(IEntity *piEntity);
	void          AddChild(IEntity *piEntity,CVector vPos,CVector vAngles);
	void          RemoveChild(IEntity *piEntity);
	unsigned int  GetChildren();
	IEntity      *GetChild(unsigned int nIndex);
	void          SetChildLocation(IEntity *piEntity,CVector vPosition,CVector vAngles);
	void          GetChildLocation(IEntity *piEntity,CVector &vPosition,CVector &vAngles);
	
    bool          IsRemoved();
    void          Remove();
	
	void          Kill();
	
    // Render
    void Render(IGenericRender *piRender,IGenericCamera *piCamera);

    // Procesamiento
    unsigned int GetNextProcessFrame();
    void         ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

    // Colisiones y Daños
    double 	GetHealth();
	void 	SetHealth(double dHealth);

    double 	GetMaxHealth();
	void 	SetMaxHealth(double dMaxHealth);

	unsigned int  GetDamageType();
	void          SetDamageType(unsigned int dwDamageType);
	
	CTraceInfo GetTrace(const CVector &p1,const CVector &p2);

    bool OnCollision(IEntity *pOther,CVector &vCollisionPos); 
    void OnDamage(double dDamage,IEntity *pAggresor);
    virtual void OnKilled();

	// Steering Behaviours

    // Animaciones
    void ProcessAnimations(unsigned int dwCurrentTime,double dTimeFraction,bool *pbAnimationsFinished);
    void OnAnimationEvent(string sEvent,string sParams);

    // Armas
	unsigned int  AddWeapon(IWeapon *piWeapon);
	unsigned int  GetWeapons();
	IWeapon		 *GetWeapon(unsigned int nIndex);
	
    // Rutas
    void SetRoute(IRoute *piRoute);
	IRoute *GetRoute();
	bool HasFinishedRoute();
	
    IEntity *GetTarget();
	void	 SetTarget(IEntity *piTarget);

           CEntityBase();
    virtual ~CEntityBase();
};


