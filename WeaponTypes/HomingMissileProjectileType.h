#pragma once

class CHomingMissileProjectileType: public CEntityTypeBase
{
public:

  double m_dDamage;
  DWORD  m_dwDuration;
  DWORD  m_dwFallDuration;
  DWORD  m_dwTimeBettwenAcquireTargetAttemps;
  DWORD  m_dwTimeToWaitToAcquireTargets;
  DWORD  m_dwMaximunTargetsToAcquire;
  double m_dMaxAngularSpeed;
  double m_dMaxSpeed;

  IEntity *CreateInstance(IEntity *piParent,DWORD dwCurrentTime);

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
  END_PROP_MAP();

  CHomingMissileProjectileType();
  ~CHomingMissileProjectileType();
};


class CHomingMissileProjectile: public CEntityBase,public IEntityEvents
{
  CHomingMissileProjectileType  *m_pType;
  IEntity *m_piParent;
  IEntity *m_piTarget;
  DWORD    m_dwNextTimeToAcquireTarget;
  DWORD    m_dwTargetsAcquired;

  double m_dAcquireTarget_TempDistance;
  static void AcquireTargetOperation(IEntity *piEntity,void *pParam1,void *pParam2);


  // IEntityEvents

  void OnRemoved(IEntity *piEntity);
  void OnKilled(IEntity *piEntity);
	void OnKilled();

public:

  bool OnCollision(IEntity *pOther,CVector &vCollisionPos);
  void ProcessFrame(DWORD dwCurrentTime,double dTimeFraction);

  CHomingMissileProjectile(CHomingMissileProjectileType *pType,IEntity *piParent);
};