#pragma once

enum ETruckState
{
	eTruckState_Normal=ENTITY_STATE_BASE,
	eTruckState_Destroyed
};

class CTruckType: public CEntityTypeBase
{
public:
    IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

    double  m_dMaxAngularSpeed;
	
    BEGIN_PROP_MAP(CTruckType)
        PROP_CLASS_CHAIN(CEntityTypeBase)
        PROP_VALUE_FLAGS(m_dMaxAngularSpeed,"MaxAngularVelocity",60,MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP();
	
	BEGIN_ENTITY_STATE_MAP()
		ENTITY_STATE_CHAIN(CEntityTypeBase)
		ENTITY_STATE(eTruckState_Destroyed,"Destroyed")
	END_ENTITY_STATE_MAP()

    CTruckType();
    ~CTruckType();
};


class CTruck: public CEntityBase
{
	IEntity *m_piTarget;
    CTruckType  *m_pType;
	int m_nRoutePoint;
	bool m_bRouteFinished;
	unsigned int m_dwNextShotTime;
	
	void AcquireTarget();
public:

	bool HasFinishedRoute();
	void SetRoute(IRoute *piRoute);
	void OnKilled();
	void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

    CTruck(CTruckType *pType);
};