#pragma once

class CSpeedUpgradeBonusType: public CEntityTypeBase
{
public:

  double m_dSpeed;

  IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

  BEGIN_PROP_MAP(CSpeedUpgradeBonusType)
    PROP_CLASS_CHAIN(CEntityTypeBase)
    PROP(m_dSpeed,"Velocidad");
  END_PROP_MAP();

  CSpeedUpgradeBonusType();
  ~CSpeedUpgradeBonusType();
};


class CSpeedUpgradeBonus: public CEntityBase
{
  CSpeedUpgradeBonusType  *m_pType;

public:

  bool OnCollision(IEntity *pOther,CVector &vCollisionPos);

  CSpeedUpgradeBonus(CSpeedUpgradeBonusType *pType);
};