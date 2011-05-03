#pragma once

enum EGroundBossState
{
	eGroundBossState_Normal=ENTITY_STATE_BASE,
	eGroundBossState_Destroyed
};

class CGroundBossType: public CEntityTypeBase
{
public:
    IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

    double  m_dMaxAngularSpeed;
	
    BEGIN_PROP_MAP(CGroundBossType)
        PROP_CLASS_CHAIN(CEntityTypeBase)
        PROP_VALUE_FLAGS(m_dMaxAngularSpeed,"MaxAngularVelocity",60,MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP();
	
	BEGIN_ENTITY_STATE_MAP()
		ENTITY_STATE_CHAIN(CEntityTypeBase)
		ENTITY_STATE(eGroundBossState_Destroyed,"Destroyed")
	END_ENTITY_STATE_MAP()

    CGroundBossType();
    ~CGroundBossType();
};


class CGroundBoss: public CEntityBase
{
	IEntity *m_piTarget;
    CGroundBossType  *m_pType;
	int m_nRoutePoint;
	bool m_bRouteFinished;
	unsigned int m_dwNextShotTime;
	unsigned int m_nConfiguredDamageType;
	unsigned int m_nPauseEnd;
	
	void AcquireTarget();
	
	void Render(IGenericRender *piRender,IGenericCamera *piCamera);
	
public:

	bool HasFinishedRoute();
	void SetRoute(IRoute *piRoute);
	void OnKilled();
	void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

    CGroundBoss(CGroundBossType *pType);
};