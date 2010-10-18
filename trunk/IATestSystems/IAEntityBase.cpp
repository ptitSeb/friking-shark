#include "StdAfx.h"
#include ".\iaentitybase.h"
#include ".\IATestUtils.h"

CIAEntityBase::CIAEntityBase()
{
	m_dwRenderFlags=RENDER_FLAGS_DEFAULT;
	m_dSize=10;
	m_dMass=m_dSize;
	m_dMaxVelocity=200;
	m_dMaxForce=m_dMaxVelocity*m_dMass;
	m_pSeekTarget=NULL;
	m_pFleeTarget=NULL;
	m_pArriveTarget=NULL;
	m_eArriveSpeed=eSBArriveSpeed_Normal;		
	m_pPursueTarget=NULL;
	m_pEvadeTarget=NULL;
	m_pInterposeTarget1=NULL;
	m_pInterposeTarget2=NULL;
	m_pOffsetPursueTarget=NULL;

	m_bWanderEnabled=false;
	m_dWanderDistance=30;
	m_dWanderRadius=20;
	m_dWanderJitter=5;

	m_vHeading=CVector(1,0,0);
}

CIAEntityBase::~CIAEntityBase()
{
}

void CIAEntityBase::Render()
{
	glPushMatrix();

	CVector vAngles;
	AnglesFromVector(m_vVelocity,&vAngles);

	glTranslated(m_vPosition.c[0],m_vPosition.c[1],m_vPosition.c[2]);
	glRotated(vAngles.c[YAW],0,1,0);
	glRotated(vAngles.c[PITCH],0,0,1);
	glRotated(vAngles.c[ROLL],1,0,0);

	CVector vForce=m_vForce;
	CVector vVelocity=m_vVelocity;
	CVector vMins(0-m_dSize,0-m_dSize,0-m_dSize);
	CVector vMaxs(m_dSize,m_dSize,m_dSize);
	double dForce=vForce.N();
	double dVelocity=vVelocity.N();

	CVector vAhead=m_vVelocity;
	vAhead.N();
	RenderTriangle(CVector(1,0,0),CVector(0,1,0),m_dSize*2,m_dSize*1.5,m_vColor);

	if(m_bWanderEnabled)
	{
		m_Behaviours.RenderWanderLocal(this,m_dWanderDistance,m_dWanderRadius,m_dWanderJitter);		
	}

	glPopMatrix();

	glPushMatrix();
	glTranslated(m_vPosition.c[0],m_vPosition.c[1],m_vPosition.c[2]);
	if(m_dwRenderFlags&RENDER_FLAGS_FORCE){RenderAxis(vForce,m_dMaxVelocity,RGB(0,255,0));}
	if(m_dwRenderFlags&RENDER_FLAGS_VELOCITY){RenderAxis(vVelocity,dVelocity,RGB(255,0,255));}
	glPopMatrix();

	if(m_pPursueTarget)
	{
		RenderBox(m_vPursueEstimatedPosition-CVector(5,5,5),m_vPursueEstimatedPosition+CVector(5,5,5),m_vColor);
	}
	if(m_pEvadeTarget)
	{
		RenderBox(m_vEvadeEstimatedPosition-CVector(5,5,5),m_vEvadeEstimatedPosition+CVector(5,5,5),m_vColor);
	}
	if(m_pInterposeTarget1 && m_pInterposeTarget2)
	{
		RenderBox(m_vInterposeEstimatedPosition-CVector(5,5,5),m_vInterposeEstimatedPosition+CVector(5,5,5),m_vColor);
	}
	if(m_bWanderEnabled)
	{
		m_Behaviours.RenderWanderGlobal(this,m_dWanderDistance,m_dWanderRadius,m_dWanderJitter);		
	}

}

void CIAEntityBase::ProcessIA(double dTimeFraction)
{
	m_vForce=CVector(0,0,0);
	if(m_pSeekTarget){m_vForce+=m_Behaviours.Seek(this,m_pSeekTarget->GetPosition());}
	if(m_pFleeTarget){m_vForce+=m_Behaviours.Flee(this,m_pFleeTarget->GetPosition());}
	if(m_pArriveTarget){m_vForce+=m_Behaviours.Arrive(this,m_pArriveTarget->GetPosition(),m_eArriveSpeed);}
	if(m_pPursueTarget){m_vForce+=m_Behaviours.Pursue(this,m_pPursueTarget,&m_vPursueEstimatedPosition);}
	if(m_pEvadeTarget){m_vForce+=m_Behaviours.Evade(this,m_pEvadeTarget,&m_vEvadeEstimatedPosition);}
	if(m_bWanderEnabled){m_vForce+=m_Behaviours.Wander(this,m_dWanderDistance,m_dWanderRadius,m_dWanderJitter,dTimeFraction)*m_dMass;}
	if(m_pOffsetPursueTarget){m_vForce+=m_Behaviours.OffsetPursue(this,m_pOffsetPursueTarget,m_vOffsetPursueOffset);}
	if(m_pInterposeTarget1 && m_pInterposeTarget2){m_vForce+=m_Behaviours.Interpose(this,m_pInterposeTarget1,m_pInterposeTarget2,&m_vInterposeEstimatedPosition);}
	m_vForce*=m_dMaxForce;
}

