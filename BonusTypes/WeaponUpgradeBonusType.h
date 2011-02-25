#pragma once

class CWeaponUpgradeBonusType: public CEntityTypeBase
{
public:

  unsigned int m_dwLevels;
  unsigned int m_dwSlot;

  IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

  BEGIN_PROP_MAP(CWeaponUpgradeBonusType)
    PROP_CLASS_CHAIN(CEntityTypeBase)
    PROP(m_dwLevels,"Niveles");
    PROP(m_dwSlot,"Banco");
  END_PROP_MAP();

  CWeaponUpgradeBonusType();
  ~CWeaponUpgradeBonusType();
};


class CWeaponUpgradeBonus: public CEntityBase
{
  CWeaponUpgradeBonusType  *m_pType;

public:

  bool OnCollision(IEntity *pOther,CVector &vCollisionPos);

  CWeaponUpgradeBonus(CWeaponUpgradeBonusType *pType);
};