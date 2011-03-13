#pragma once

enum EStaticEntityState
{
	eStaticEntityState_Normal=ENTITY_STATE_BASE,
	eStaticEntityState_Destroyed
};

class CStaticEntityType: public CEntityTypeBase
{
public:
	IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);
	void InitializeEntity( CEntityBase *piEntity,unsigned int dwCurrentTime );

	double  m_dMaxHealth;

	BEGIN_PROP_MAP(CStaticEntityType)
		PROP_CLASS_CHAIN(CEntityTypeBase)
		PROP_VALUE_FLAGS(m_dMaxHealth,"Vida",1,MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP();
	
	BEGIN_ENTITY_STATE_MAP()
		ENTITY_STATE_CHAIN(CEntityTypeBase)
		ENTITY_STATE(eStaticEntityState_Destroyed,"Destroyed")
	END_ENTITY_STATE_MAP()

	CStaticEntityType();
	~CStaticEntityType();
};


class CStaticEntity: public CEntityBase
{
	CStaticEntityType  *m_pType;
public:
	void OnKilled();

	CStaticEntity(CStaticEntityType *pType);
};