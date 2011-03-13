#pragma once

enum EHomingMissileState
{
	eHomingMissileState_Normal=ENTITY_STATE_BASE,
	eHomingMissileState_OutOfFuel,
	eHomingMissileState_Hit
};

class CHomingMissileProjectileType: public CEntityTypeBase
{
public:

  double m_dDamage;
  unsigned int  m_dwDuration;
  unsigned int  m_dwFallDuration;
  unsigned int  m_dwTimeBettwenAcquireTargetAttemps;
  unsigned int  m_dwTimeToWaitToAcquireTargets;
  unsigned int  m_dwMaximunTargetsToAcquire;
  double m_dMaxAngularSpeed;
  double m_dMaxSpeed;

  IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

  BEGIN_PROP_MAP(CHomingMissileProjectileType)
    PROP_VALUE_FLAGS(m_dDamage,"Daño",1.0,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_VALUE_FLAGS(m_dwDuration,"Duracion",4000,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_VALUE_FLAGS(m_dwFallDuration,"DuracionCaida",5000,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_VALUE_FLAGS(m_dMaxSpeed,"Velocidad",100,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_VALUE_FLAGS(m_dMaxAngularSpeed,"VelocidadAngular",150,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_VALUE_FLAGS(m_dwTimeBettwenAcquireTargetAttemps,"TiempoEntreBusquedasDeObjetivos",300,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_VALUE_FLAGS(m_dwTimeToWaitToAcquireTargets,"TiempoInicioBusquedaObjetivos",300,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_VALUE_FLAGS(m_dwMaximunTargetsToAcquire,"NumeroMaximoObjetivos",0,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_CLASS_CHAIN(CEntityTypeBase)
  END_PROP_MAP()
  
  BEGIN_ENTITY_STATE_MAP()
	ENTITY_STATE_CHAIN(CEntityTypeBase)
	ENTITY_STATE(eHomingMissileState_OutOfFuel,"OutOfFuel")
	ENTITY_STATE(eHomingMissileState_Hit,"Hit")
  END_ENTITY_STATE_MAP()
  
  CHomingMissileProjectileType();
  ~CHomingMissileProjectileType();
};


class CHomingMissileProjectile: public CEntityBase,public IEntityEvents
{
  CHomingMissileProjectileType  *m_pType;
  IEntity *m_piParent;
  IEntity *m_piTarget;
  unsigned int    m_dwNextTimeToAcquireTarget;
  unsigned int    m_dwTargetsAcquired;

  double m_dAcquireTarget_TempDistance;
  static void AcquireTargetOperation(IEntity *piEntity,void *pParam1,void *pParam2);


  // IEntityEvents

  void OnRemoved(IEntity *piEntity);
  void OnKilled(IEntity *piEntity);
	void OnKilled();

public:

  bool OnCollision(IEntity *pOther,CVector &vCollisionPos);
  void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

  CHomingMissileProjectile(CHomingMissileProjectileType *pType,IEntity *piParent);
};