#pragma once

enum EBombState
{
	eBombState_Normal=ENTITY_STATE_BASE,
	eBombState_Hit
};

class CBombProjectileType: public CEntityTypeBase
{
public:

  double m_dDamage;

  IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

  BEGIN_PROP_MAP(CBombProjectileType)
    PROP_VALUE_FLAGS(m_dDamage,"Damage",1.0,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_CLASS_CHAIN(CEntityTypeBase)
  END_PROP_MAP();
  
  BEGIN_ENTITY_STATE_MAP()
	ENTITY_STATE_CHAIN(CEntityTypeBase)
	ENTITY_STATE(eBombState_Hit,"Hit")
  END_ENTITY_STATE_MAP()
  
  CBombProjectileType();
  ~CBombProjectileType();
};


class CBombProjectile: public CEntityBase
{
  CBombProjectileType  *m_pType;
  IEntity *m_piParent;
public:

  bool OnCollision(IEntity *pOther,CVector &vCollisionPos);

  CBombProjectile(CBombProjectileType *pType,IEntity *piParent);
};