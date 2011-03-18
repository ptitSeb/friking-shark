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