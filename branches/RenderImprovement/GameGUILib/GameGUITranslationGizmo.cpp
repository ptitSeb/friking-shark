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
#include "GameGUITranslationGizmo.h"

CGameGUITranslationGizmo::CGameGUITranslationGizmo()
{
	m_bInteractiveAxises[0]=m_bInteractiveAxises[1]=m_bInteractiveAxises[2]=true;
	m_bVisibleAxises[0]=m_bVisibleAxises[1]=m_bVisibleAxises[2]=true;
	m_nSelectedElement=-1;
	m_dSize=1.0;
	m_nSelectionBase=0;
}

CGameGUITranslationGizmo::~CGameGUITranslationGizmo(){}

void 	CGameGUITranslationGizmo::SetPosition(CVector vPosition){m_vPosition=vPosition;}
CVector CGameGUITranslationGizmo::GetPosition(){return m_vPosition;}

void CGameGUITranslationGizmo::SetSize(double dSize){m_dSize=dSize;}

void CGameGUITranslationGizmo::SetInteractiveAxises(bool bX,bool bY,bool bZ)
{
	m_bInteractiveAxises[0]=bX;
	m_bInteractiveAxises[1]=bY;
	m_bInteractiveAxises[2]=bZ;
}

void CGameGUITranslationGizmo::SetVisibleAxises(bool bX,bool bY,bool bZ)
{
	m_bVisibleAxises[0]=bX;
	m_bVisibleAxises[1]=bY;
	m_bVisibleAxises[2]=bZ;
}

void CGameGUITranslationGizmo::RenderAxis(IGenericRender *piRender,CVector vDir,CVector vUp,CVector vColor)
{
	CVector vPoint1=m_vPosition,vPoint2=m_vPosition+vDir*m_dSize;
	piRender->PushState();
	piRender->ActivateSolid();
	piRender->ActivateDepth();
	piRender->RenderLine(vPoint1,vPoint2,0xFFFF,vColor,1);
	piRender->RenderArrowHead(vPoint2,vDir,vUp,m_dSize*0.2,m_dSize*0.05,m_dSize*0.05,vColor,1);

	piRender->DeactivateDepth();
	piRender->DeactivateSolid();
	piRender->RenderLine(vPoint1,vPoint2,0x1111,vColor,1);
	piRender->RenderArrowHead(vPoint2,vDir,vUp,m_dSize*0.2,m_dSize*0.05,m_dSize*0.05,vColor,1);

	piRender->PopState();
}

void CGameGUITranslationGizmo::RenderAxisPlane(IGenericRender *piRender,CVector vDir,CVector vUp,CVector vColor)
{
	CVector pVertexes[3],pVertexesFlipped[3];
	pVertexes[0]=m_vPosition;
	pVertexes[1]=m_vPosition+vDir*(m_dSize*0.5);
	pVertexes[2]=m_vPosition+vUp*(m_dSize*0.5);
	pVertexesFlipped[2]=m_vPosition;
	pVertexesFlipped[1]=m_vPosition+vDir*(m_dSize*0.5);
	pVertexesFlipped[0]=m_vPosition+vUp*(m_dSize*0.5);

	piRender->PushState();
	piRender->ActivateBlending();
	piRender->DeactivateSolid();
	piRender->DeactivateDepth();
	piRender->RenderPolygon(3,pVertexes,vColor,0.5);
	piRender->RenderPolygon(3,pVertexesFlipped,vColor,0.5);

	piRender->ActivateSolid();
	piRender->ActivateDepth();
	piRender->RenderPolygon(3,pVertexes,vColor,0.5);
	piRender->RenderPolygon(3,pVertexesFlipped,vColor,0.5);
	piRender->PopState();
}

void CGameGUITranslationGizmo::Render(IGenericRender *piRender, IGenericCamera *piCamera)
{
	if(m_bVisibleAxises[0]){RenderAxis(piRender,AxisPosX,AxisPosY,m_nSelectedElement==0?CVector(1,0,0):CVector(0.5,0,0));}
	if(m_bVisibleAxises[1]){RenderAxis(piRender,AxisPosY,AxisPosZ,m_nSelectedElement==1?CVector(0,0,1):CVector(0,0,0.5));}
	if(m_bVisibleAxises[2]){RenderAxis(piRender,AxisPosZ,AxisPosX,m_nSelectedElement==2?CVector(1,1,0):CVector(0.5,0.5,0));}
	if(m_bVisibleAxises[0] && m_bVisibleAxises[1]){RenderAxisPlane(piRender,AxisPosX,AxisPosY,m_nSelectedElement==3?CVector(0,0,1):CVector(0,0,0.5));}
	if(m_bVisibleAxises[0] && m_bVisibleAxises[2]){RenderAxisPlane(piRender,AxisPosX,AxisPosZ,m_nSelectedElement==4?CVector(1,0,0):CVector(0.5,0,0));}
	if(m_bVisibleAxises[1] && m_bVisibleAxises[2]){RenderAxisPlane(piRender,AxisPosY,AxisPosZ,m_nSelectedElement==5?CVector(1,1,0):CVector(0.5,0.5,0));}
}

int CGameGUITranslationGizmo::SelectionRender(int nBaseId,IGenericRender *piRender, IGenericCamera *piCamera)
{
	m_nSelectionBase=nBaseId;
	if(m_bInteractiveAxises[0]){piRender->SetSelectionId(nBaseId+0);RenderAxis(piRender,AxisPosX,AxisPosY,CVector(1,0,0));}
	if(m_bInteractiveAxises[1]){piRender->SetSelectionId(nBaseId+1);RenderAxis(piRender,AxisPosY,AxisPosZ,CVector(0,0,1));}
	if(m_bInteractiveAxises[2]){piRender->SetSelectionId(nBaseId+2);RenderAxis(piRender,AxisPosZ,AxisPosX,CVector(1,1,0));}
	if(m_bVisibleAxises[0] && m_bVisibleAxises[1]){piRender->SetSelectionId(nBaseId+3);RenderAxisPlane(piRender,AxisPosX,AxisPosY,CVector(0,0,0.7));}
	if(m_bVisibleAxises[0] && m_bVisibleAxises[2]){piRender->SetSelectionId(nBaseId+4);RenderAxisPlane(piRender,AxisPosX,AxisPosZ,CVector(0.7,0,0));}
	if(m_bVisibleAxises[1] && m_bVisibleAxises[2]){piRender->SetSelectionId(nBaseId+5);RenderAxisPlane(piRender,AxisPosY,AxisPosZ,CVector(0.7,0.7,0));}
	return m_nSelectionBase+6;
}

void CGameGUITranslationGizmo::Select(int nSelectedElementId)
{
	m_nSelectedElement=-1;
	if(nSelectedElementId>=m_nSelectionBase && nSelectedElementId<m_nSelectionBase+6)
	{
		m_nSelectedElement=nSelectedElementId-m_nSelectionBase;
	}
}

bool CGameGUITranslationGizmo::BeginTranslation(CLine *pMouseRay,IGenericCamera *piCamera)
{
	if(m_nSelectedElement==-1){return false;}
	m_vTranslationOrigin=PositionFromRay(pMouseRay,piCamera);
	m_vTranslationStartPosition=m_vPosition;
	return true;
}

CVector	CGameGUITranslationGizmo::PositionFromRay(CLine *pMouseRay,IGenericCamera *piCamera)
{
	CPlane plane;	
	// Seleccion del plano de corte con el mouse ray
	// Si el elemento seleccionado es un solo eje se selecciona a partir de los planos en los que participa
	// dependiendo del angulo que hace el eje que forma cada plano con la camara. 
	// De esta forma se evita seleccionar un plano muy paralelo al mouse ray.

	if(m_nSelectedElement==0)     {plane=CPlane(fabs(AxisPosZ*piCamera->GetForwardVector())<0.5?AxisPosY:AxisPosZ,m_vPosition);}
	else if(m_nSelectedElement==1){plane=CPlane(fabs(AxisPosZ*piCamera->GetForwardVector())<0.5?AxisPosX:AxisPosZ,m_vPosition);}
	else if(m_nSelectedElement==2){plane=CPlane(fabs(AxisPosX*piCamera->GetForwardVector())<0.5?AxisPosY:AxisPosX,m_vPosition);}
	else if(m_nSelectedElement==3){plane=CPlane(AxisPosZ,m_vPosition);}
	else if(m_nSelectedElement==4){plane=CPlane(AxisPosY,m_vPosition);}
	else if(m_nSelectedElement==5){plane=CPlane(AxisPosX,m_vPosition);}

	// Corte del mouse ray con el plano seleccionado
	double dSide1=plane.GetSide(pMouseRay->m_Points[0]);
	double dSide2=plane.GetSide(pMouseRay->m_Points[1]);
	double dLength=(dSide1-dSide2);
	double dFraction=dLength?dSide1/dLength:0;
	return pMouseRay->m_Points[0]+(pMouseRay->m_Points[1]-pMouseRay->m_Points[0])*dFraction;
}

CVector	CGameGUITranslationGizmo::ProcessTranslation(CLine *pMouseRay,IGenericCamera *piCamera)
{
	if(m_nSelectedElement==-1){return m_vPosition;}

	CVector vPos=PositionFromRay(pMouseRay,piCamera);	
	// Aplicacion del movimiento dependiendo de los ejes seleccionados
	if(m_nSelectedElement==0 || m_nSelectedElement==3 || m_nSelectedElement==4){m_vPosition.c[0]=m_vTranslationStartPosition.c[0]+(vPos.c[0]-m_vTranslationOrigin.c[0]);}
	if(m_nSelectedElement==1 || m_nSelectedElement==3 || m_nSelectedElement==5){m_vPosition.c[1]=m_vTranslationStartPosition.c[1]+(vPos.c[1]-m_vTranslationOrigin.c[1]);}
	if(m_nSelectedElement==2 || m_nSelectedElement==4 || m_nSelectedElement==5){m_vPosition.c[2]=m_vTranslationStartPosition.c[2]+(vPos.c[2]-m_vTranslationOrigin.c[2]);}

	for(int c=0;c<3;c++)
	{
		if(fabs(m_vPosition.c[c]-m_vTranslationStartPosition.c[c])<1){m_vPosition.c[c]=m_vTranslationStartPosition.c[c];}
	}
	return m_vPosition;
}
CVector CGameGUITranslationGizmo::EndTranslation(){return m_vPosition;}

