#pragma once

enum EPointBonusState
{
	ePointBonusState_Normal=ENTITY_STATE_BASE,
	ePointBonusState_Taken
};

class CPointBonusType: public CEntityTypeBase
{
public:

  unsigned int m_dwPoints;

  IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

  BEGIN_PROP_MAP(CPointBonusType)
    PROP_CLASS_CHAIN(CEntityTypeBase)
    PROP(m_dwPoints,"Puntos");
  END_PROP_MAP();
  
  BEGIN_ENTITY_STATE_MAP()
	ENTITY_STATE_CHAIN(CEntityTypeBase)
	ENTITY_STATE(ePointBonusState_Taken,"Taken")
  END_ENTITY_STATE_MAP()
  
  CPointBonusType();
  ~CPointBonusType();
};


class CPointBonus: public CEntityBase
{
  CPointBonusType  *m_pType;

public:

  bool OnCollision(IEntity *pOther,CVector &vCollisionPos);

  CPointBonus(CPointBonusType *pType);
};