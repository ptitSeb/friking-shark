#pragma once

class CIAEntityBase;

enum ESBArriveSpeed
{
	eSBArriveSpeed_Fast=1,
	eSBArriveSpeed_Normal,
	eSBArriveSpeed_Slow
};

enum eSBArriveSpeed_Normal;

class CSteeringBehaviours
{
	CVector m_vWanderTarget;
	CVector m_vLastWanderTarget;

public:

	CVector Seek(CIAEntityBase *pEntity,CVector &vPos);
	CVector Flee(CIAEntityBase *pEntity,CVector &vPos);
	CVector Arrive(CIAEntityBase *pEntity,CVector &vPos,ESBArriveSpeed eArriveSpeed);
	CVector Pursue(CIAEntityBase *pEntity,CIAEntityBase *pTarget,CVector *pOutEstimatedPosition);
	CVector Evade(CIAEntityBase *pEntity,CIAEntityBase *pTarget,CVector *pOutEstimatedPosition);
	CVector Wander(CIAEntityBase *pEntity,double dDistance,double dRadius,double dJitter,double dTimeFraction);
	CVector Interpose(CIAEntityBase *pEntity,CIAEntityBase *pTarget1,CIAEntityBase *pTarget2,CVector *pOutEstimatedPosition);
	CVector OffsetPursue(CIAEntityBase *pEntity,CIAEntityBase *pTarget1,CVector &vOffset);

	void RenderWanderLocal(CIAEntityBase *pEntity,double dDistance,double dRadius,double dJitter);
	void RenderWanderGlobal(CIAEntityBase *pEntity,double dDistance,double dRadius,double dJitter);

	CSteeringBehaviours(void);
	~CSteeringBehaviours(void);
};
