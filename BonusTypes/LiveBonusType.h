#pragma once

class CLiveBonusType: public CEntityTypeBase
{
public:

  IEntity *CreateInstance(IEntity *piParent,DWORD dwCurrentTime);

  BEGIN_PROP_MAP(CLiveBonusType)
    PROP_CLASS_CHAIN(CEntityTypeBase)
  END_PROP_MAP();

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