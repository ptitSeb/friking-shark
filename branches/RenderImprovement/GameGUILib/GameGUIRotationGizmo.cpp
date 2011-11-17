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
#include "GameGUILib.h"
#include "GameGUIRotationGizmo.h"

#define ROTATION_GIZMO_SEGMENTS 72

CGameGUIRotationGizmo::CGameGUIRotationGizmo()
{
	m_bInteractiveAngles[YAW]=m_bInteractiveAngles[PITCH]=m_bInteractiveAngles[ROLL]=true;
	m_bVisibleAngles[YAW]=m_bVisibleAngles[PITCH]=m_bVisibleAngles[ROLL]=true;
	m_nSelectedElement=-1;
	m_dRadius=1.0;
	m_nSelectionBase=0;
	UpdateAxises();
}

CGameGUIRotationGizmo::~CGameGUIRotationGizmo(){}

void 	CGameGUIRotationGizmo::SetPosition(CVector vPosition){m_vPosition=vPosition;}
void 	CGameGUIRotationGizmo::SetAngles(CVector vAngles){m_vAngles=vAngles;UpdateAxises();}
CVector CGameGUIRotationGizmo::GetAngles(){return m_vAngles;}

void CGameGUIRotationGizmo::SetRadius(double dRadius){m_dRadius=dRadius;}

void CGameGUIRotationGizmo::SetInteractiveAngles(bool bYaw,bool bPitch,bool bRoll)
{
	m_bInteractiveAngles[YAW]=bYaw;
	m_bInteractiveAngles[PITCH]=bPitch;
	m_bInteractiveAngles[ROLL]=bRoll;
}

void CGameGUIRotationGizmo::SetVisibleAngles(bool bYaw,bool bPitch,bool bRoll)
{
	m_bVisibleAngles[YAW]=bYaw;
	m_bVisibleAngles[PITCH]=bPitch;
	m_bVisibleAngles[ROLL]=bRoll;
}

void CGameGUIRotationGizmo::RenderAngle(IGenericRender *piRender,IGenericCamera *piCamera,double dCurrentAngle,CVector vX,CVector vY,CVector vColor,double dRadius,bool bSelection)
{
	CVector vZ=vX^vY;
	CPlane vClipPlane=CPlane(piCamera->GetForwardVector(),m_vPosition);

	if(!bSelection)
	{
		piRender->PushState();
		piRender->DeactivateDepth();
		piRender->DeactivateSolid();
	}
	CVector vPoints[ROTATION_GIZMO_SEGMENTS];
	unsigned int nSegments=ROTATION_GIZMO_SEGMENTS;
	double dSegmentSize=2.0*PI/(double)nSegments;
	for(unsigned int x=0;x<nSegments;x++)
	{
		vPoints[x]=m_vPosition+vX*(cos(dSegmentSize*(double)x)*dRadius)+vY*(sin(dSegmentSize*(double)x)*dRadius);
	}
	unsigned int nFrontPoints=0,nBackPoints=0;
	CVector vFrontPoints[ROTATION_GIZMO_SEGMENTS*2];
	CVector vBackPoints[ROTATION_GIZMO_SEGMENTS*2];
	for(unsigned int x=0;x<nSegments;x++)
	{
		CVector vPoint1=vPoints[x];
		CVector vPoint2=(x==nSegments-1)?vPoints[0]:vPoints[x+1];
		
		if(vClipPlane.GetSide(vPoint1)<0 && vClipPlane.GetSide(vPoint2)<0)
		{
			vFrontPoints[nFrontPoints++]=vPoint1;
			vFrontPoints[nFrontPoints++]=vPoint2;
		}
		else
		{
			vBackPoints[nBackPoints++]=vPoint1;
			vBackPoints[nBackPoints++]=vPoint2;
		}
	}
	if(nFrontPoints){piRender->RenderLines(nFrontPoints/2,vFrontPoints,0xFFFF,vColor,1);}
	if(nBackPoints){piRender->RenderLines(nBackPoints/2,vBackPoints,0x1111,vColor,1);}
	if(!bSelection)
	{
		piRender->PopState();
	}
}

void CGameGUIRotationGizmo::Render(IGenericRender *piRender, IGenericCamera *piCamera)
{
	if(m_bVisibleAngles[YAW])  {RenderAngle(piRender,piCamera,m_vAngles.c[YAW],m_vYawAxis1,m_vYawAxis2,m_nSelectedElement==YAW?CVector(1,0,0):CVector(0.5,0,0),m_dRadius,false);}
	if(m_bVisibleAngles[PITCH]){RenderAngle(piRender,piCamera,m_vAngles.c[PITCH],m_vPitchAxis1,m_vPitchAxis2,m_nSelectedElement==PITCH?CVector(0,0,1):CVector(0,0,0.5),m_dRadius*0.95,false);}
	if(m_bVisibleAngles[ROLL]) {RenderAngle(piRender,piCamera,m_vAngles.c[ROLL],m_vRollAxis1,m_vRollAxis2,m_nSelectedElement==ROLL?CVector(1,1,0):CVector(0.5,0.5,0),m_dRadius*0.9,false);}
}

void CGameGUIRotationGizmo::UpdateAxises()
{
	CMatrix matrix1,matrix2,rot1,rot2;
	CVector vAxis1,vAxis2;
	rot1.R(AxisPosY,DegreesToRadians(m_vAngles.c[YAW]));
	matrix1*=rot1;
	rot2.R(AxisPosZ,DegreesToRadians(m_vAngles.c[PITCH]));
	matrix2*=rot2;
	matrix2*=rot1;

	m_vYawAxis1=AxisPosX;
	m_vYawAxis2=AxisNegZ;
	m_vPitchAxis1=AxisPosY;m_vPitchAxis1*=matrix1;
	m_vPitchAxis2=AxisNegX;m_vPitchAxis2*=matrix1;
	m_vRollAxis1=AxisPosY;m_vRollAxis1*=matrix2;
	m_vRollAxis2=AxisPosZ;m_vRollAxis2*=matrix2;
}

int CGameGUIRotationGizmo::SelectionRender(int nBaseId,IGenericRender *piRender, IGenericCamera *piCamera)
{
	m_nSelectionBase=nBaseId;
	
	if(m_bInteractiveAngles[YAW])  {piRender->SetSelectionId(nBaseId+YAW);RenderAngle(piRender,piCamera,m_vAngles.c[YAW],m_vYawAxis1,m_vYawAxis2,m_nSelectedElement==YAW?CVector(0,0,1):CVector(0,0,0.5),m_dRadius,true);}
	if(m_bInteractiveAngles[PITCH]){piRender->SetSelectionId(nBaseId+PITCH);RenderAngle(piRender,piCamera,m_vAngles.c[PITCH],m_vPitchAxis1,m_vPitchAxis2,m_nSelectedElement==PITCH?CVector(1,1,0):CVector(0.5,0.5,0),m_dRadius*0.95,true);}
	if(m_bInteractiveAngles[ROLL]) {piRender->SetSelectionId(nBaseId+ROLL);RenderAngle(piRender,piCamera,m_vAngles.c[ROLL],m_vRollAxis1,m_vRollAxis2,m_nSelectedElement==ROLL?CVector(1,0,0):CVector(0.5,0,0),m_dRadius*0.9,true);}
		
	return m_nSelectionBase+6;
}

void CGameGUIRotationGizmo::Select(int nSelectedElementId)
{
	m_nSelectedElement=-1;
	if(nSelectedElementId>=m_nSelectionBase && nSelectedElementId<m_nSelectionBase+3)
	{
		m_nSelectedElement=nSelectedElementId-m_nSelectionBase;
	}
}

bool CGameGUIRotationGizmo::BeginRotation(CLine *pMouseRay,IGenericCamera *piCamera)
{
	if(m_nSelectedElement==-1){return false;}
	m_vRotationOrigin=AnglesFromRay(pMouseRay,piCamera);
	m_vRotationStartAngles=m_vAngles;
	return true;
}

CVector	CGameGUIRotationGizmo::AnglesFromRay(CLine *pMouseRay,IGenericCamera *piCamera)
{
	CPlane plane;
	if(m_nSelectedElement==YAW)       {plane=CPlane(AxisPosY,m_vPosition);}
	else if(m_nSelectedElement==PITCH){plane=CPlane(AxisPosZ,m_vPosition);}
	else if(m_nSelectedElement==ROLL) {plane=CPlane(AxisPosX,m_vPosition);}
	
	// Corte del mouse ray con el plano seleccionado
	double dSide1=plane.GetSide(pMouseRay->m_Points[0]);
	double dSide2=plane.GetSide(pMouseRay->m_Points[1]);
	double dLength=(dSide1-dSide2);
	double dFraction=dLength?dSide1/dLength:0;
	CVector vPos=pMouseRay->m_Points[0]+(pMouseRay->m_Points[1]-pMouseRay->m_Points[0])*dFraction;
	
	CVector vTemp;
	if(m_nSelectedElement==YAW)
	{
		CMatrix m;
		m.Ref(m_vYawAxis1,m_vYawAxis2,m_vYawAxis1^m_vYawAxis2);
		vPos-=m_vPosition;
		vPos*=m;
		vTemp.c[YAW]=RadiansToDegrees(atan2(vPos.c[1],vPos.c[0]));
	}
	if(m_nSelectedElement==PITCH)
	{
		CMatrix m;
		m.Ref(m_vPitchAxis1,m_vPitchAxis2,m_vPitchAxis1^m_vPitchAxis2);
		vPos-=m_vPosition;
		vPos*=m;
		vTemp.c[PITCH]=RadiansToDegrees(atan2(vPos.c[1],vPos.c[0]));
	}
	if(m_nSelectedElement==ROLL)
	{
		CMatrix m;
		m.Ref(m_vRollAxis1,m_vRollAxis2,m_vRollAxis1^m_vRollAxis2);
		vPos-=m_vPosition;
		vPos*=m;
		vTemp.c[ROLL]=RadiansToDegrees(atan2(vPos.c[1],vPos.c[0]));
	}
	return vTemp;
}

CVector	CGameGUIRotationGizmo::ProcessRotation(CLine *pMouseRay,IGenericCamera *piCamera)
{
	if(m_nSelectedElement==-1){return m_vAngles;}

	CVector vAngles=AnglesFromRay(pMouseRay,piCamera);	
	// Aplicacion del movimiento dependiendo de los ejes seleccionados
	if(m_nSelectedElement==0){m_vAngles.c[YAW]=m_vRotationStartAngles.c[YAW]+(vAngles.c[YAW]-m_vRotationOrigin.c[YAW]);}
	if(m_nSelectedElement==1){m_vAngles.c[PITCH]=m_vRotationStartAngles.c[PITCH]+(vAngles.c[PITCH]-m_vRotationOrigin.c[PITCH]);}
	if(m_nSelectedElement==2){m_vAngles.c[ROLL]=m_vRotationStartAngles.c[ROLL]+(vAngles.c[ROLL]-m_vRotationOrigin.c[ROLL]);}
	for(int c=0;c<3;c++)
	{
		if(((int)fabs(m_vAngles.c[c]))%90<10)
		{
			m_vAngles.c[c]=(((int)(m_vAngles.c[c]))/90)*90;
		}
	}
	UpdateAxises();
	return m_vAngles;
}
CVector CGameGUIRotationGizmo::EndRotation(){return m_vAngles;}

