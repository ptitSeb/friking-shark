#pragma once

enum ELiveBonusState
{
	eLiveBonusState_Normal=ENTITY_STATE_BASE,
	eLiveBonusState_Taken
};

class CLiveBonusType: public CEntityTypeBase
{
public:

  IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

  BEGIN_ENTITY_STATE_MAP()
	ENTITY_STATE_CHAIN(CEntityTypeBase)
	ENTITY_STATE(eLiveBonusState_Taken,"Taken")
  END_ENTITY_STATE_MAP()
  
  CLiveBonusType();
  ~CLiveBonusType();
};


class CLiveBonus: public CEntityBase
{
  CLiveBonusType  *m_pType;

public:

  bool OnCollision(IEntity *pOther,CVector &vCollisionPos);

  CLiveBonus(CLiveBonusType *pType);
};