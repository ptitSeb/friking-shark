#pragma once
enum EFighterState
{
	eFighterState_Normal=ENTITY_STATE_BASE,
	eFighterState_Falling,
	eFighterState_Crashed
};

class CFighterType: public CEntityTypeBase
{
public:
    IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

    double  m_dMaxRoll;
    double  m_dMaxAngularSpeed;
    double  m_dTimeFirstShotMin;
    double  m_dTimeFirstShotMax;
    double  m_dTimeBetweenShotsMin;
    double  m_dTimeBetweenShotsMax;

    // Behaviour parameterers
	bool         m_bHeadToTarget; // false: folow the route, true, head directly to the target
	bool         m_bFleeOnSameX;
	bool         m_bFleeOnSameZ;
	unsigned int m_nFleeDelay;
	double       m_dFleeProbability;
	double       m_dMinFleeAngle; // Min angle to flee, centered on 90 and 270 depending on the target position
	double       m_dMaxFleeAngle; // Max angle to flee, centered on 90 and 270 depending on the target position
	double       m_dMaxHeadingCorrection;// Max angle per sec to correct to head the target

    BEGIN_PROP_MAP(CFighterType)
        PROP_CLASS_CHAIN(CEntityTypeBase)
        PROP_VALUE_FLAGS(m_dMaxRoll,"MaxRoll",40,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dMaxAngularSpeed,"MaxAngularVelocity",60,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dTimeFirstShotMin,"TimeFirstShotMin",500,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dTimeFirstShotMax,"TimeFirstShotMax",5000,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_dTimeBetweenShotsMin,"TimeBetweenShotsMin",2000,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_dTimeBetweenShotsMax,"TimeBetweenShotsMax",2000,MRPF_NORMAL|MRPF_OPTIONAL);
		
        PROP_VALUE_FLAGS(m_bHeadToTarget,"HeadToTarget",false,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_dFleeProbability,"FleeProbability",1,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_nFleeDelay,"FleeDelay",0,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_bFleeOnSameX,"FleeOnSameX",false,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_bFleeOnSameZ,"FleeOnSameZ",false,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dMinFleeAngle,"MinFleeAngle",20,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dMaxFleeAngle,"MaxFleeAngle",55,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dMaxHeadingCorrection,"MaxHeadingCorrection",0,MRPF_NORMAL|MRPF_OPTIONAL);
    END_PROP_MAP();
	
	BEGIN_ENTITY_STATE_MAP()
		ENTITY_STATE_CHAIN(CEntityTypeBase)
		ENTITY_STATE(eFighterState_Falling,"Falling")
		ENTITY_STATE(eFighterState_Crashed,"Crashed")
	END_ENTITY_STATE_MAP()
	
    CFighterType();
    ~CFighterType();
};


class CFighter: public CEntityBase
{
    CFighterType  *m_pType;

    int    m_nRoutePoint;
	bool   m_bFleeEnabled;
	bool   m_bFleeing;
	double m_dFleeAngle;
	bool   m_bWasVisible;
	double m_dRadius;
	
    double m_dwNextShotTime;
	
	unsigned int m_nFallStartTime;
	unsigned int m_nFleeStartTime;
	unsigned int m_nCurrentTime;
public:

	void AcquireTarget();
	
    void Render(IGenericRender *piRender,IGenericCamera *piCamera);
    void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

    void OnKilled();
    bool OnCollision(IEntity *pOther,CVector &vCollisionPos);
    IEntity *GetTarget();

	bool HasFinishedRoute();
	
	CFighter(CFighterType *pType,unsigned int dwCurrentTime);
};