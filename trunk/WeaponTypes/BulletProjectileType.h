#pragma once

class CBulletProjectileType: public CEntityTypeBase
{
public:

  double m_dDamage;
  DWORD  m_dwDuration;

  IEntity *CreateInstance(IEntity *piParent,DWORD dwCurrentTime);

  BEGIN_PROP_MAP(CBulletProjectileType)
    PROP_VALUE_FLAGS(m_dDamage,"Daño",1.0,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_VALUE_FLAGS(m_dwDuration,"Duracion",1000,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_CLASS_CHAIN(CEntityTypeBase)
  END_PROP_MAP();

  CBulletProjectileType();
  ~CBulletProjectileType();
};


class CBulletProjectile: public CEntityBase
{
  CBulletProjectileType  *m_pType;
  IEntity *m_piParent;
public:

  bool OnCollision(IEntity *pOther,CVector &vCollisionPos);
  void ProcessFrame(DWORD dwCurrentTime,double dTimeFraction);

  CBulletProjectile(CBulletProjectileType *pType,IEntity *piParent);
};