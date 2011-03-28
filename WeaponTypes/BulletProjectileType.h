#pragma once

enum EBulletState
{
	eBulletState_Normal=ENTITY_STATE_BASE,
	eBulletState_Hit
};

class CBulletProjectileType: public CEntityTypeBase
{
public:

  double m_dDamage;
  unsigned int  m_dwDuration;
  
  IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

  BEGIN_PROP_MAP(CBulletProjectileType)
    PROP_VALUE_FLAGS(m_dDamage,"Damage",1.0,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_VALUE_FLAGS(m_dwDuration,"Duration",1000,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_CLASS_CHAIN(CEntityTypeBase)
  END_PROP_MAP();
  
  BEGIN_ENTITY_STATE_MAP()
	ENTITY_STATE_CHAIN(CEntityTypeBase)
	ENTITY_STATE(eBulletState_Hit,"Hit")
  END_ENTITY_STATE_MAP()
  
  CBulletProjectileType();
  ~CBulletProjectileType();
};


class CBulletProjectile: public CEntityBase
{
  CBulletProjectileType  *m_pType;
  IEntity *m_piParent;
  unsigned int  m_dwNextTryAcquireTarget;

  bool m_bTargetAcquired;
  CVector m_vTargetPosition;

public:

  bool OnCollision(IEntity *pOther,CVector &vCollisionPos);
  void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

  static void AcquireTargetOperation(IEntity *piEntity,void *pParam1,void *pParam2);

  CBulletProjectile(CBulletProjectileType *pType,IEntity *piParent);
};