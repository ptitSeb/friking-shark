#pragma once

enum ETurretState
{
	eTurretState_Normal=ENTITY_STATE_BASE,
	eTurretState_Destroyed
};

class CTurretType: public CEntityTypeBase
{
public:
	IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);
	void InitializeEntity(CEntityBase *piEntity,unsigned int dwCurrentTime);

	BEGIN_ENTITY_STATE_MAP()
		ENTITY_STATE_CHAIN(CEntityTypeBase)
		ENTITY_STATE(eTurretState_Destroyed,"Destroyed")
	END_ENTITY_STATE_MAP()
	
	CTurretType();
	~CTurretType();
};


class CTurret: public CEntityBase
{
	CTurretType  *m_pType;
	
	void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

public:

	void OnKilled();

	CTurret(CTurretType *pType);
};