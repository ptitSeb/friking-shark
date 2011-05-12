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
#include "GameGUIBBoxGizmo.h"

CGameGUIBBoxGizmo::CGameGUIBBoxGizmo()
{
	m_nSelectedElement=-1;
	m_nSelectionBase=0;
	m_dArrowSize=1;
}

CGameGUIBBoxGizmo::~CGameGUIBBoxGizmo(){}

void 	CGameGUIBBoxGizmo::SetPosition(CVector vPosition){m_vPosition=vPosition;}
CVector CGameGUIBBoxGizmo::GetPosition(){return m_vPosition;}

void CGameGUIBBoxGizmo::SetBounds(CVector vMins,CVector vMaxs){m_vMins=vMins;m_vMaxs=vMaxs;m_vCenter=(m_vMins+m_vMaxs)*0.5;}
void CGameGUIBBoxGizmo::GetBounds(CVector *pvMins,CVector *pvMaxs){*pvMins=m_vMins;*pvMaxs=m_vMaxs;}

void CGameGUIBBoxGizmo::SetArrowSize(double dSize){m_dArrowSize=dSize;}

void CGameGUIBBoxGizmo::RenderArrow(IGenericRender *piRender,CVector vPosition,CVector vDirection,CVector vUp,CVector vColor)
{
	CVector vPoint1=vPosition;
	CVector vPoint2=vPosition+vDirection*m_dArrowSize;
	
	piRender->PushState();
	piRender->ActivateSolid();
	piRender->ActivateDepth();
	piRender->ActivateBlending();
	piRender->SetColor(vColor,0.5);

	piRender->RenderLine(vPoint1,vPoint2,vColor,0xFFFF);
	piRender->SetColor(vColor,1);
	piRender->RenderArrowHead(vPoint2,vDirection,vUp,m_dArrowSize*0.2,m_dArrowSize*0.05,m_dArrowSize*0.05);

	piRender->DeactivateDepth();
	piRender->DeactivateSolid();
	piRender->RenderLine(vPoint1,vPoint2,vColor,0x1111);
	piRender->SetColor(vColor,1);
	piRender->RenderArrowHead(vPoint2,vDirection,vUp,m_dArrowSize*0.2,m_dArrowSize*0.05,m_dArrowSize*0.05);

	piRender->PopState();
}

void CGameGUIBBoxGizmo::Render(IGenericRender *piRender, IGenericCamera *piCamera)
{
	piRender->RenderBBox(m_vPosition,Origin,m_vMins,m_vMaxs,CVector(1,1,1));
	RenderArrow(piRender,m_vPosition+CVector(m_vMaxs.c[0],m_vCenter.c[1],m_vCenter.c[2]),AxisPosX,AxisPosY,m_nSelectedElement==0?CVector(1,0,0):CVector(0.5,0,0));
	RenderArrow(piRender,m_vPosition+CVector(m_vMins.c[0],m_vCenter.c[1],m_vCenter.c[2]),AxisNegX,AxisPosY,m_nSelectedElement==1?CVector(1,0,0):CVector(0.5,0,0));
	RenderArrow(piRender,m_vPosition+CVector(m_vCenter.c[0],m_vMaxs.c[1],m_vCenter.c[2]),AxisPosY,AxisPosZ,m_nSelectedElement==2?CVector(0,0,1):CVector(0,0,0.5));
	RenderArrow(piRender,m_vPosition+CVector(m_vCenter.c[0],m_vMins.c[1],m_vCenter.c[2]),AxisNegY,AxisPosZ,m_nSelectedElement==3?CVector(0,0,1):CVector(0,0,0.5));
	RenderArrow(piRender,m_vPosition+CVector(m_vCenter.c[0],m_vCenter.c[1],m_vMaxs.c[2]),AxisPosZ,AxisPosY,m_nSelectedElement==4?CVector(1,1,0):CVector(0.5,0.5,0));
	RenderArrow(piRender,m_vPosition+CVector(m_vCenter.c[0],m_vCenter.c[1],m_vMins.c[2]),AxisNegZ,AxisPosY,m_nSelectedElement==5?CVector(1,1,0):CVector(0.5,0.5,0));
}

int CGameGUIBBoxGizmo::SelectionRender(int nBaseId,IGenericRender *piRender, IGenericCamera *piCamera)
{
	m_nSelectionBase=nBaseId;
	piRender->SetSelectionId(nBaseId+0);RenderArrow(piRender,m_vPosition+CVector(m_vMaxs.c[0],m_vCenter.c[1],m_vCenter.c[2]),AxisPosX,AxisPosY,m_nSelectedElement==0?CVector(1,0,0):CVector(0.5,0,0));
	piRender->SetSelectionId(nBaseId+1);RenderArrow(piRender,m_vPosition+CVector(m_vMins.c[0],m_vCenter.c[1],m_vCenter.c[2]),AxisNegX,AxisPosY,m_nSelectedElement==1?CVector(1,0,0):CVector(0.5,0,0));
	piRender->SetSelectionId(nBaseId+2);RenderArrow(piRender,m_vPosition+CVector(m_vCenter.c[0],m_vMaxs.c[1],m_vCenter.c[2]),AxisPosY,AxisPosZ,m_nSelectedElement==2?CVector(0,0,1):CVector(0,0,0.5));
	piRender->SetSelectionId(nBaseId+3);RenderArrow(piRender,m_vPosition+CVector(m_vCenter.c[0],m_vMins.c[1],m_vCenter.c[2]),AxisNegY,AxisPosZ,m_nSelectedElement==3?CVector(0,0,1):CVector(0,0,0.5));
	piRender->SetSelectionId(nBaseId+4);RenderArrow(piRender,m_vPosition+CVector(m_vCenter.c[0],m_vCenter.c[1],m_vMaxs.c[2]),AxisPosZ,AxisPosY,m_nSelectedElement==4?CVector(1,1,0):CVector(0.5,0.5,0));
	piRender->SetSelectionId(nBaseId+5);RenderArrow(piRender,m_vPosition+CVector(m_vCenter.c[0],m_vCenter.c[1],m_vMins.c[2]),AxisNegZ,AxisPosY,m_nSelectedElement==5?CVector(1,1,0):CVector(0.5,0.5,0));
	return m_nSelectionBase+6;
}

void CGameGUIBBoxGizmo::Select(int nSelectedElementId)
{
	m_nSelectedElement=-1;
	if(nSelectedElementId>=m_nSelectionBase && nSelectedElementId<m_nSelectionBase+6)
	{
		m_nSelectedElement=nSelectedElementId-m_nSelectionBase;
	}
}

bool CGameGUIBBoxGizmo::BeginBBox(CLine *pMouseRay,IGenericCamera *piCamera)
{
	if(m_nSelectedElement==-1){return false;}
	m_vTranslationOrigin=PositionFromRay(pMouseRay,piCamera);
	m_vTranslationStartMins=m_vMins;
	m_vTranslationStartMaxs=m_vMaxs;
	return true;
}

CVector	CGameGUIBBoxGizmo::PositionFromRay(CLine *pMouseRay,IGenericCamera *piCamera)
{
	CPlane plane;	
	// Seleccion del plano de corte con el mouse ray
	// Si el elemento seleccionado es un solo eje se selecciona a partir de los planos en los que participa
	// dependiendo del angulo que hace el eje que forma cada plano con la camara. 
	// De esta forma se evita seleccionar un plano muy paralelo al mouse ray.

	if(m_nSelectedElement==0)     {plane=CPlane(fabs(AxisPosZ*piCamera->GetForwardVector())<0.5?AxisPosY:AxisPosZ,m_vPosition+AxisPosX*m_vMaxs.c[0]);}
	else if(m_nSelectedElement==1){plane=CPlane(fabs(AxisPosZ*piCamera->GetForwardVector())<0.5?AxisPosY:AxisPosZ,m_vPosition+AxisPosX*m_vMins.c[0]);}
	else if(m_nSelectedElement==2){plane=CPlane(fabs(AxisPosZ*piCamera->GetForwardVector())<0.5?AxisPosX:AxisPosZ,m_vPosition+AxisPosY*m_vMaxs.c[1]);}
	else if(m_nSelectedElement==3){plane=CPlane(fabs(AxisPosZ*piCamera->GetForwardVector())<0.5?AxisPosX:AxisPosZ,m_vPosition+AxisPosY*m_vMaxs.c[1]);}
	else if(m_nSelectedElement==4){plane=CPlane(fabs(AxisPosX*piCamera->GetForwardVector())<0.5?AxisPosY:AxisPosX,m_vPosition+AxisPosZ*m_vMaxs.c[2]);}
	else if(m_nSelectedElement==5){plane=CPlane(fabs(AxisPosX*piCamera->GetForwardVector())<0.5?AxisPosY:AxisPosX,m_vPosition+AxisPosZ*m_vMaxs.c[2]);}

	// Corte del mouse ray con el plano seleccionado
	double dSide1=plane.GetSide(pMouseRay->m_Points[0]);
	double dSide2=plane.GetSide(pMouseRay->m_Points[1]);
	double dLength=(dSide1-dSide2);
	double dFraction=dLength?dSide1/dLength:0;
	return pMouseRay->m_Points[0]+(pMouseRay->m_Points[1]-pMouseRay->m_Points[0])*dFraction;

}

void CGameGUIBBoxGizmo::ProcessBBox(CLine *pMouseRay,IGenericCamera *piCamera)
{
	if(m_nSelectedElement==-1){return;}

	CVector vPos=PositionFromRay(pMouseRay,piCamera);

	// Aplicacion del movimiento dependiendo de los ejes seleccionados
	if(m_nSelectedElement==0){m_vMaxs.c[0]=m_vTranslationStartMaxs.c[0]+(vPos.c[0]-m_vTranslationOrigin.c[0]);}
	if(m_nSelectedElement==1){m_vMins.c[0]=m_vTranslationStartMins.c[0]+(vPos.c[0]-m_vTranslationOrigin.c[0]);}
	if(m_nSelectedElement==2){m_vMaxs.c[1]=m_vTranslationStartMaxs.c[1]+(vPos.c[1]-m_vTranslationOrigin.c[1]);}
	if(m_nSelectedElement==3){m_vMins.c[1]=m_vTranslationStartMins.c[1]+(vPos.c[1]-m_vTranslationOrigin.c[1]);}
	if(m_nSelectedElement==4){m_vMaxs.c[2]=m_vTranslationStartMaxs.c[2]+(vPos.c[2]-m_vTranslationOrigin.c[2]);}
	if(m_nSelectedElement==5){m_vMins.c[2]=m_vTranslationStartMins.c[2]+(vPos.c[2]-m_vTranslationOrigin.c[2]);}
	m_vCenter=(m_vMins+m_vMaxs)*0.5;
}

void CGameGUIBBoxGizmo::EndBBox(){}



