#pragma once

class CLiveBonusType: public CEntityTypeBase
{
public:

  IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

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