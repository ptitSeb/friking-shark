#include "./stdafx.h"
#include "AnimationSystems.h"
#include "AnimationObjectBase.h"

CAnimationObjectTypeBase::CAnimationObjectTypeBase(){}

CAnimationObjectBase::CAnimationObjectBase(CAnimationObjectTypeBase *pType,IAnimation *piAnimation){m_bActive=false;m_piAnimation=piAnimation;m_pTypeBase=pType;}
string  CAnimationObjectBase::GetName(){return m_pTypeBase->m_sObjectName;}
void    CAnimationObjectBase::Activate(DWORD dwCurrentTime){m_bActive=true;}
void    CAnimationObjectBase::Deactivate(){m_bActive=false;}
bool    CAnimationObjectBase::IsActive(){return m_bActive;}
IAnimation *CAnimationObjectBase::GetAnimation(){return m_piAnimation;}
bool CAnimationObjectBase::ProcessFrame(IPhysicManager *pPhysicManager,DWORD dwCurrentTime,double dInterval){return false;}
void CAnimationObjectBase::CustomRender(IGenericRender *piRender,IGenericCamera *piCamera){}void CAnimationObjectTypeBase::DesignRender( IGenericRender *pRender,CVector &vPosition,CVector &vAngles,bool bSelected ){}
CTraceInfo CAnimationObjectBase::GetTrace( const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 )
{
	CTraceInfo info;
	info.m_dTraceFraction=1.0;
	info.m_vTracePos=p2;
	return info;
}
void CAnimationObjectTypeBase::DesignGetBBox( CVector *pvMins,CVector *pvMaxs ){}
double CAnimationObjectTypeBase::DesignGetRadius(){return 0;}

CTraceInfo CAnimationObjectTypeBase::DesignGetTrace( const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 )
{
	CTraceInfo info;
	info.m_dTraceFraction=1.0;
	info.m_vTracePos=p2;
	return info;
}