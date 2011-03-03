#include "./stdafx.h"
#include "GameRunTimeLib.h"
#include "GameGUILib.h"
#include "GamePlayWindow.h"

CGamePlayWindow::CGamePlayWindow(void)
{
}

CGamePlayWindow::~CGamePlayWindow(void)
{
}
bool CGamePlayWindow::InitWindow(IGameWindow *piParent,bool bPopup)
{
	bool bResult=CGameWindowBase::InitWindow(piParent,bPopup);
	return bResult;
}

void CGamePlayWindow::DestroyWindow()
{
	m_PlayAreaManager.Detach();
	m_Render.Detach();

	CGameWindowBase::DestroyWindow();
}
void CGamePlayWindow::OnDraw(IGenericRender *piRender)
{	
	if(m_Render.m_piRender==NULL)
	{
		m_Render.Attach("GameSystem","GameRender");
	}
	if(m_PlayAreaManager.m_piPlayAreaManager==NULL)
	{
		m_PlayAreaManager.Attach("GameSystem","PlayAreaManager");
	}
	if(m_Render.m_piRender==NULL){return;}
	if(m_PlayAreaManager.m_piPlayAreaManager==NULL){return;}

	IGenericCamera *piCamera=m_PlayAreaManager.m_piPlayAreaManager->GetCamera();
	if(piCamera)
	{
		double dPlayAreaAspectRatio=piCamera->GetAspectRatio();
		double cx=m_rRealRect.h*dPlayAreaAspectRatio;
		double dx=(m_rRealRect.w-cx)*0.5;

		double dNearPlane=0,dFarPlane=0;
		double dViewAngle=piCamera->GetViewAngle();
		CVector vAngles,vPosition;

		piCamera->GetClippingPlanes(dNearPlane,dFarPlane);
		vAngles=piCamera->GetAngles();
		vPosition=piCamera->GetPosition();

		piRender->SetViewport(m_rRealRect.x+dx,m_rRealRect.y,cx,m_rRealRect.h);
		piRender->SetPerspectiveProjection(dViewAngle,dNearPlane,100000);
		piRender->SetCamera(vPosition,vAngles.c[YAW],vAngles.c[PITCH],vAngles.c[ROLL]);

		m_Render.m_piRender->Render(piRender,piCamera);
	}
	REL(piCamera);
}
