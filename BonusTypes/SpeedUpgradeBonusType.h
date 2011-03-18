#pragma once

enum ESpeedUpgradeBonusState
{
	eSpeedUpgradeBonusState_Normal=ENTITY_STATE_BASE,
	eSpeedUpgradeBonusState_Taken
};

class CSpeedUpgradeBonusType: public CEntityTypeBase
{
public:

  double m_dSpeed;

  IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

  BEGIN_PROP_MAP(CSpeedUpgradeBonusType)
    PROP_CLASS_CHAIN(CEntityTypeBase)
    PROP(m_dSpeed,"Speed");
  END_PROP_MAP();

  BEGIN_ENTITY_STATE_MAP()
	ENTITY_STATE_CHAIN(CEntityTypeBase)
	ENTITY_STATE(eSpeedUpgradeBonusState_Taken,"Taken")
  END_ENTITY_STATE_MAP()
  
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