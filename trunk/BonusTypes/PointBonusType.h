#pragma once

class CPointBonusType: public CEntityTypeBase
{
public:

  IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

 
  CPointBonusType();
  ~CPointBonusType();
};


class CPointBonus: public CEntityBase
{
  CPointBonusType  *m_pType;

public:

	void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);
	
	CPointBonus(CPointBonusType *pType);
};