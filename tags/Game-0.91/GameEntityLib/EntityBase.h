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
	unsigned int nId;
	
	SChildEntity(){piEntity=NULL;nId=0;}
};

class CEntityBase: public IEntity,public CPublisherBase, public CSubscriberBase, virtual public IEntityEvents
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
	unsigned int		 m_nNextChildId;
	
    // Alineacion (Bando) de la entidad

    unsigned int m_dwAlignment;

    // Miembors de Rutas
    IRoute  *m_piRoute;
	unsigned int m_nRouteDelay;
	

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
	void SetRouteDelay(unsigned int nDelay);
	
	IRoute *GetRoute();
	bool HasFinishedRoute();
	
    IEntity *GetTarget();
	void	 SetTarget(IEntity *piTarget);
	
	void GivePoints(unsigned int nPoints);
	
	void OnKilled(IEntity *piEntity);
	void OnRemoved(IEntity *piEntity);
	
           CEntityBase();
    virtual ~CEntityBase();
};


