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

	double  m_dMaxHealth;

	BEGIN_PROP_MAP(CStaticStructureType)
		PROP_CLASS_CHAIN(CEntityTypeBase)
		PROP_VALUE_FLAGS(m_dMaxHealth,"Vida",1,MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP();
	
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
public:

	void OnKilled();

	CStaticStructure(CStaticStructureType *pType);
};