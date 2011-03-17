#include "./stdafx.h"
#include "GameGUILib.h"
#include "GameGUIRotationGizmo.h"

CGameGUIRotationGizmo::CGameGUIRotationGizmo()
{
	m_bInteractiveAngles[YAW]=m_bInteractiveAngles[PITCH]=m_bInteractiveAngles[ROLL]=true;
	m_bVisibleAngles[YAW]=m_bVisibleAngles[PITCH]=m_bVisibleAngles[ROLL]=true;
	m_nSelectedElement=-1;
	m_dRadius=1.0;
	m_nSelectionBase=0;
}

CGameGUIRotationGizmo::~CGameGUIRotationGizmo(){}

void 	CGameGUIRotationGizmo::SetPosition(CVector vPosition){m_vPosition=vPosition;}
void 	CGameGUIRotationGizmo::SetAngles(CVector vAngles){m_vAngles=vAngles;}
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

void CGameGUIRotationGizmo::RenderAngle(IGenericRender *piRender,IGenericCamera *piCamera,double dCurrentAngle,CVector vUp,CVector vRight,CVector vColor)
{
	piRender->PushState();

	CPlane vClipPlane=CPlane(piCamera->GetForwardVector(),m_vPosition);
	CVector vForward=vUp^vRight;

	piRender->DeactivateDepth();
	piRender->DeactivateSolid();
	piRender->SetColor(vColor,1);
	CVector vPoints[36];
	double dSegmentSize=2.0*PI/36.0;
	for(unsigned int x=0;x<36;x++)
	{
		vPoints[x]=m_vPosition+vRight*(cos(dSegmentSize*(double)x)*m_dRadius)+vForward*(sin(dSegmentSize*(double)x)*m_dRadius);
	}
	for(int x=0;x<36;x++)
	{
		CVector vPoint1=vPoints[x];
		CVector vPoint2=(x==35)?vPoints[0]:vPoints[x+1];
		if(vClipPlane.GetSide(vPoint1)<0 && vClipPlane.GetSide(vPoint2)<0)
		{
			piRender->RenderLine(vPoint1,vPoint2,vColor,0xFFFF);
		}
		else
		{
			piRender->RenderLine(vPoint1,vPoint2,vColor*0.5,0x1111);
		}
	}
	piRender->PopState();
}

void CGameGUIRotationGizmo::Render(IGenericRender *piRender, IGenericCamera *piCamera)
{
	CMatrix matrix,rot;
	CVector vAxis1,vAxis2;
	if(m_bVisibleAngles[YAW])  {RenderAngle(piRender,piCamera,m_vAngles.c[YAW],AxisPosY,AxisPosX,m_nSelectedElement==YAW?CVector(0,0,1):CVector(0,0,0.5));}
	rot.R(AxisPosY,m_vAngles.c[YAW]);
	matrix*=rot;
	vAxis1=AxisPosZ;vAxis1*=matrix;
	vAxis2=AxisPosY;vAxis2*=matrix;
	if(m_bVisibleAngles[PITCH]){RenderAngle(piRender,piCamera,m_vAngles.c[PITCH],vAxis1,vAxis2,m_nSelectedElement==PITCH?CVector(1,1,0):CVector(0.5,0.5,0));}
	rot.R(AxisPosZ,m_vAngles.c[PITCH]);
	matrix*=rot;
	vAxis1=AxisPosX;vAxis1*=matrix;
	vAxis2=AxisPosZ;vAxis2*=matrix;
	if(m_bVisibleAngles[ROLL]) {RenderAngle(piRender,piCamera,m_vAngles.c[ROLL],vAxis1,vAxis2,m_nSelectedElement==ROLL?CVector(1,0,0):CVector(0.5,0,0));}
}

int CGameGUIRotationGizmo::SelectionRender(int nBaseId,IGenericRender *piRender, IGenericCamera *piCamera)
{
	m_nSelectionBase=nBaseId;
	if(m_bInteractiveAngles[YAW])  {piRender->SetSelectionId(nBaseId+YAW);RenderAngle(piRender,piCamera,m_vAngles.c[YAW],AxisPosY,AxisPosX,CVector(1,0,0));}
	if(m_bInteractiveAngles[PITCH]){piRender->SetSelectionId(nBaseId+PITCH);RenderAngle(piRender,piCamera,m_vAngles.c[PITCH],AxisPosZ,AxisPosY,CVector(0,0,1));}
	if(m_bInteractiveAngles[ROLL]) {piRender->SetSelectionId(nBaseId+ROLL);RenderAngle(piRender,piCamera,m_vAngles.c[ROLL],AxisPosX,AxisPosZ,CVector(1,1,0));}
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
	m_vRotationOrigin=AnglesFromRay(pMouseRay,piCamera);
	m_vRotationStartAngles=m_vAngles;
	return true;
}

CVector	CGameGUIRotationGizmo::AnglesFromRay(CLine *pMouseRay,IGenericCamera *piCamera)
{
	CPlane plane;
	if(m_nSelectedElement==YAW)       {plane=CPlane(AxisPosY,m_vAngles);}
	else if(m_nSelectedElement==PITCH){plane=CPlane(AxisPosZ,m_vAngles);}
	else if(m_nSelectedElement==ROLL) {plane=CPlane(AxisPosX,m_vAngles);}
	
	// Corte del mouse ray con el plano seleccionado
	double dSide1=plane.GetSide(pMouseRay->m_Points[YAW]);
	double dSide2=plane.GetSide(pMouseRay->m_Points[PITCH]);
	double dLength=(dSide1-dSide2);
	double dFraction=dLength?dSide1/dLength:0;
	CVector vPos=pMouseRay->m_Points[YAW]+(pMouseRay->m_Points[PITCH]-pMouseRay->m_Points[YAW])*dFraction;
	return m_vAngles;
}

CVector	CGameGUIRotationGizmo::ProcessRotation(CLine *pMouseRay,IGenericCamera *piCamera)
{
	if(m_nSelectedElement==-1){return m_vAngles;}
/*
	CVector vPos=AnglesFromRay(pMouseRay,piCamera);	
	// Aplicacion del movimiento dependiendo de los ejes seleccionados
	if(m_nSelectedElement==0 || m_nSelectedElement==3 || m_nSelectedElement==4){m_vAngles.c[YAW]=m_vRotationStartAngles.c[YAW]+(vPos.c[YAW]-m_vRotationOrigin.c[YAW]);}
	if(m_nSelectedElement==1 || m_nSelectedElement==3 || m_nSelectedElement==5){m_vAngles.c[PITCH]=m_vRotationStartAngles.c[PITCH]+(vPos.c[PITCH]-m_vRotationOrigin.c[PITCH]);}
	if(m_nSelectedElement==2 || m_nSelectedElement==4 || m_nSelectedElement==5){m_vAngles.c[ROLL]=m_vRotationStartAngles.c[ROLL]+(vPos.c[ROLL]-m_vRotationOrigin.c[ROLL]);}
	*/
	
	return m_vAngles;
}
CVector CGameGUIRotationGizmo::EndRotation(){return m_vAngles;}

