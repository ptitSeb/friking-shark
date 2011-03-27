#pragma once

enum EStaticStructureState
{
	eStaticStructureState_Normal=ENTITY_STATE_BASE,
	eStaticStructureState_Destroyed
};

class CStaticStructureType: public CEntityTypeBase
{
public:
	IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);
	void InitializeEntity(CEntityBase *piEntity,unsigned int dwCurrentTime);

	BEGIN_ENTITY_STATE_MAP()
		ENTITY_STATE_CHAIN(CEntityTypeBase)
		ENTITY_STATE(eStaticStructureState_Destroyed,"Destroyed")
	END_ENTITY_STATE_MAP()
	
	CStaticStructureType();
	~CStaticStructureType();
};


class CStaticStructure: public CEntityBase
{
	CStaticStructureType  *m_pType;
	
	IEntity *GetTarget();
	void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);
	
public:

	void OnKilled();

	CStaticStructure(CStaticStructureType *pType);
};