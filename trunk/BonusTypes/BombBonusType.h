#pragma once

enum EBombBonusState
{
	eBombBonusState_Normal=ENTITY_STATE_BASE,
	eBombBonusState_Taken
};

class CBombBonusType: public CEntityTypeBase
{
public:
	CPlayAreaManagerWrapper m_PlayAreaManager;
	
  IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

  
  BEGIN_ENTITY_STATE_MAP()
	ENTITY_STATE_CHAIN(CEntityTypeBase)
	ENTITY_STATE(eBombBonusState_Taken,"Taken")
  END_ENTITY_STATE_MAP()
  
  CBombBonusType();
  ~CBombBonusType();
};


class CBombBonus: public CEntityBase
{
  CBombBonusType  *m_pType;
 
  CVector 		   m_vOriginalPosition;
  CPlane 		   m_PlayAreaPlane;
  IGenericCamera  *m_piCamera;
  
public:
 
  bool OnCollision(IEntity *pOther,CVector &vCollisionPos);
  
  void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);
  
  CBombBonus(CBombBonusType *pType,CVector vOriginalPosition);
  ~CBombBonus();
};