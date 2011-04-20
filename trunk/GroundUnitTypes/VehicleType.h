#pragma once

enum EVehicleState
{
	eVehicleState_Normal=ENTITY_STATE_BASE,
	eVehicleState_Destroyed
};

class CVehicleType: public CEntityTypeBase
{
public:
    IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

    double  m_dMaxAngularSpeed;
	
    BEGIN_PROP_MAP(CVehicleType)
        PROP_CLASS_CHAIN(CEntityTypeBase)
        PROP_VALUE_FLAGS(m_dMaxAngularSpeed,"MaxAngularVelocity",60,MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP();
	
	BEGIN_ENTITY_STATE_MAP()
		ENTITY_STATE_CHAIN(CEntityTypeBase)
		ENTITY_STATE(eVehicleState_Destroyed,"Destroyed")
	END_ENTITY_STATE_MAP()

    CVehicleType();
    ~CVehicleType();
};


class CVehicle: public CEntityBase
{
	IEntity *m_piTarget;
    CVehicleType  *m_pType;
	int m_nRoutePoint;
	bool m_bRouteFinished;
	unsigned int m_dwNextShotTime;
	unsigned int m_nConfiguredDamageType;
	
	void AcquireTarget();
	
	void Render(IGenericRender *piRender,IGenericCamera *piCamera);
	
public:

	bool HasFinishedRoute();
	void SetRoute(IRoute *piRoute);
	void OnKilled();
	void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

    CVehicle(CVehicleType *pType);
};