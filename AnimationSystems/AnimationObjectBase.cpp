//  Friking Shark, a Flying Shark arcade remake.
//  Copyright (C) 2011  Javier Martin Garcia (javiermartingarcia@gmail.com)
//	
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


#include "./stdafx.h"
#include "AnimationSystems.h"
#include "AnimationObjectBase.h"

CAnimationObjectTypeBase::CAnimationObjectTypeBase(){}

CAnimationObjectBase::CAnimationObjectBase(CAnimationObjectTypeBase *pType,IAnimation *piAnimation){m_bActive=false;m_piAnimation=piAnimation;m_pTypeBase=pType;}
string  CAnimationObjectBase::GetName(){return m_pTypeBase->m_sObjectName;}
void    CAnimationObjectBase::Activate(unsigned int dwCurrentTime){m_bActive=true;}
void    CAnimationObjectBase::Deactivate(){m_bActive=false;}
bool    CAnimationObjectBase::IsActive(){return m_bActive;}
IAnimation *CAnimationObjectBase::GetAnimation(){return m_piAnimation;}
bool CAnimationObjectBase::ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval){return false;}
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
bool CAnimationObjectTypeBase::PrepareResources(){return true;}
CTraceInfo CAnimationObjectTypeBase::DesignGetTrace( const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 )
{
	CTraceInfo info;
	info.m_dTraceFraction=1.0;
	info.m_vTracePos=p2;
	return info;
}