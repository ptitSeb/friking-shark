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
#include <GL/glu.h>
#include "resource.h"
#include "GameRunTimeLib.h"
#include "GameGUILib.h"
#include "IATestMainWindow.h"
#include "IATestUtils.h"

float dBaseSize=1000.0;
float dBaseThickness=2.0;

extern CSystemModuleHelper *g_pSystemModuleHelper;

CIATestMainWindow::CIATestMainWindow(void)
{
	m_bPauseOnNextFrame=false;

	memset(m_pdModelMatrix,0,sizeof(m_pdModelMatrix));
	memset(m_pdProjectionMatrix,0,sizeof(m_pdProjectionMatrix));
	memset(m_pnViewport,0,sizeof(m_pnViewport));

	m_piCurrentScenario=NULL;
	m_bVisible=true;
	m_pSelectedEntity=NULL;
	m_dwNextAcceptedPauseKeyTime=0;

	InitializeChildren();
}

CIATestMainWindow::~CIATestMainWindow(void)
{
}
bool CIATestMainWindow::InitWindow(IGameWindow *piParent,bool bPopup)
{
	bool bOk=CGameWindowBase::InitWindow(piParent,bPopup);
	if(bOk)
	{
		m_FrameManager.Attach("GameGUI","FrameManager");
	}
	if(bOk)
	{
		CVector vPos=CVector(dBaseSize/2.0,dBaseSize/2.0,dBaseSize*1.5);
		CVector vAngles=CVector(90,0,0);
		m_Viewport.Attach("GameGUI","Viewport");
		m_Viewport.m_piViewport->SetCaption("IA Test");
		m_Camera.Create("GameGUI","Camera","");
 		m_Camera.m_piCamera->SetPosition(vPos);
		m_Camera.m_piCamera->SetAngles(vAngles);

		SGameRect sRect;
		sRect.x=0;
		sRect.y=0;
		sRect.w=1;
		sRect.h=1;
		m_eReferenceSystem=eGameGUIReferenceSystem_Relative;
		SetRect(&sRect);

		m_piGUIManager->SetFocus(this);
	}

	ITestScenario *piDefaultScenario=NULL;
	AddScenario(new CTestScenarioSeek);
	AddScenario(new CTestScenarioFlee);
	AddScenario(new CTestScenarioArrive);
	AddScenario(new CTestScenarioPursue);
	AddScenario(new CTestScenarioEvade);
	AddScenario(new CTestScenarioWander);
	AddScenario(new CTestScenarioInterpose);
	AddScenario(piDefaultScenario=new CTestScenarioOffsetPursue);

	ActivateScenario(piDefaultScenario);

	return bOk;
}
void CIATestMainWindow::DestroyWindow()
{
	RemoveAllEntities();
	RemoveAllScenarios();

	CGameWindowBase::DestroyWindow();
}


void CIATestMainWindow::ProcessInput(double dTimeFraction,double dRealTimeFraction)
{
	if(m_piGUIManager->IsKeyDown(GK_PAUSE)){ProcessKey(KEY_PAUSE,dTimeFraction,dRealTimeFraction);}
	if(m_piGUIManager->IsKeyDown('P')){ProcessKey(KEY_PROCESS_ONE_FRAME,dTimeFraction,dRealTimeFraction);}
	if(m_piGUIManager->IsKeyDown(GK_UP) || m_piGUIManager->IsKeyDown(GK_NUMPAD8) || m_piGUIManager->IsKeyDown('W')){ProcessKey(KEY_FORWARD,dTimeFraction,dRealTimeFraction);}
	if(m_piGUIManager->IsKeyDown(GK_DOWN) || m_piGUIManager->IsKeyDown(GK_NUMPAD2) || m_piGUIManager->IsKeyDown('S')){ProcessKey(KEY_BACK,dTimeFraction,dRealTimeFraction);}
	if(m_piGUIManager->IsKeyDown(GK_LEFT) || m_piGUIManager->IsKeyDown(GK_NUMPAD4) || m_piGUIManager->IsKeyDown('A')){ProcessKey(KEY_LEFT,dTimeFraction,dRealTimeFraction);}
	if(m_piGUIManager->IsKeyDown(GK_RIGHT) || m_piGUIManager->IsKeyDown(GK_NUMPAD6) || m_piGUIManager->IsKeyDown('D')){ProcessKey(KEY_RIGHT,dTimeFraction,dRealTimeFraction);}
	if(m_piGUIManager->IsKeyDown(GK_NUMPAD9) || m_piGUIManager->IsKeyDown('R')){ProcessKey(KEY_UP,dTimeFraction,dRealTimeFraction);}
	if(m_piGUIManager->IsKeyDown(GK_NUMPAD3) || m_piGUIManager->IsKeyDown('F')){ProcessKey(KEY_DOWN,dTimeFraction,dRealTimeFraction);}
}

void CIATestMainWindow::ProcessKey(unsigned short nKey,double dTimeFraction,double dRealTimeFraction)
{
	double dMovementInspectionSpeed=500.0;
	double dForwardSpeed=dMovementInspectionSpeed*dTimeFraction;
	double dCameraForwardSpeed=dMovementInspectionSpeed*dRealTimeFraction;
	if(m_piGUIManager->IsKeyDown(GK_LSHIFT)){dForwardSpeed*=3.0;}
	if(m_pSelectedEntity)
	{
		if(nKey==KEY_FORWARD)	{CVector vPos=m_pSelectedEntity->GetPosition()+m_Camera.m_piCamera->GetUpVector()*(dForwardSpeed);m_pSelectedEntity->SetPosition(vPos);}
		else if(nKey==KEY_BACK)	{CVector vPos=m_pSelectedEntity->GetPosition()-m_Camera.m_piCamera->GetUpVector()*(dForwardSpeed);m_pSelectedEntity->SetPosition(vPos);}
		else if(nKey==KEY_LEFT)	{CVector vPos=m_pSelectedEntity->GetPosition()-m_Camera.m_piCamera->GetRightVector()*(dForwardSpeed);m_pSelectedEntity->SetPosition(vPos);}
		else if(nKey==KEY_RIGHT){CVector vPos=m_pSelectedEntity->GetPosition()+m_Camera.m_piCamera->GetRightVector()*(dForwardSpeed);m_pSelectedEntity->SetPosition(vPos);}
	}
	else
	{
		if(nKey==KEY_FORWARD)	{CVector vPos=m_Camera.m_piCamera->GetPosition()+m_Camera.m_piCamera->GetUpVector()*(dCameraForwardSpeed);m_Camera.m_piCamera->SetPosition(vPos);}
		else if(nKey==KEY_BACK)	{CVector vPos=m_Camera.m_piCamera->GetPosition()-m_Camera.m_piCamera->GetUpVector()*(dCameraForwardSpeed);m_Camera.m_piCamera->SetPosition(vPos);}
		else if(nKey==KEY_LEFT)	{CVector vPos=m_Camera.m_piCamera->GetPosition()-m_Camera.m_piCamera->GetRightVector()*(dCameraForwardSpeed);m_Camera.m_piCamera->SetPosition(vPos);}
		else if(nKey==KEY_RIGHT){CVector vPos=m_Camera.m_piCamera->GetPosition()+m_Camera.m_piCamera->GetRightVector()*(dCameraForwardSpeed);m_Camera.m_piCamera->SetPosition(vPos);}
		else if(nKey==KEY_UP)	{CVector vPos=m_Camera.m_piCamera->GetPosition()+m_Camera.m_piCamera->GetForwardVector()*(dCameraForwardSpeed);m_Camera.m_piCamera->SetPosition(vPos);}
		else if(nKey==KEY_DOWN)	{CVector vPos=m_Camera.m_piCamera->GetPosition()-m_Camera.m_piCamera->GetForwardVector()*(dCameraForwardSpeed);m_Camera.m_piCamera->SetPosition(vPos);}
	}                                     

	if(nKey==KEY_PAUSE)
	{
		if(m_FrameManager.m_piFrameManager->GetCurrentRealTime()>m_dwNextAcceptedPauseKeyTime)
		{
			m_dwNextAcceptedPauseKeyTime=m_FrameManager.m_piFrameManager->GetCurrentRealTime()+100;
			m_FrameManager.m_piFrameManager->TogglePauseOnNextFrame();
		}
	}
	if(nKey==KEY_PROCESS_ONE_FRAME)
	{
		if(m_FrameManager.m_piFrameManager->GetCurrentRealTime()>m_dwNextAcceptedPauseKeyTime)
		{
			m_dwNextAcceptedPauseKeyTime=m_FrameManager.m_piFrameManager->GetCurrentRealTime()+10;
			m_bPauseOnNextFrame=true;
			m_FrameManager.m_piFrameManager->SetPauseOnNextFrame(false);
		}
	}
}

void CIATestMainWindow::OnDraw(IGenericRender *piRender)
{
	if(!m_FrameManager.m_piFrameManager)
	{
		return;
	}

	m_FrameManager.m_piFrameManager->ProcessFrame();
	double dTimeFraction=m_FrameManager.m_piFrameManager->GetTimeFraction();
	double dRealTimeFraction=m_FrameManager.m_piFrameManager->GetRealTimeFraction();

	if(m_bPauseOnNextFrame)
	{
		m_FrameManager.m_piFrameManager->SetPauseOnNextFrame(true);
		m_bPauseOnNextFrame=false;
	}

	ProcessInput(dTimeFraction,dRealTimeFraction);

	if(m_FrameManager.m_piFrameManager->GetTimeFraction()>0)
	{
		ProcessPhysics(dTimeFraction);
		ProcessIA(dTimeFraction);
	}

	double dAspectRatio=m_rRealRect.h/m_rRealRect.w;
	double dNearPlane=0,dFarPlane=0;
	double dViewAngle=m_Camera.m_piCamera->GetViewAngle();
	CVector vAngles,vPosition;

	m_Camera.m_piCamera->SetAspectRatio(dAspectRatio);
	m_Camera.m_piCamera->GetClippingPlanes(dNearPlane,dFarPlane);
	vAngles=m_Camera.m_piCamera->GetAngles();
	vPosition=m_Camera.m_piCamera->GetPosition();


	piRender->SetPerspectiveProjection(dViewAngle,dNearPlane,100000);
	piRender->SetCamera(vPosition,vAngles.c[YAW],vAngles.c[PITCH],vAngles.c[ROLL]);

	glGetDoublev(GL_MODELVIEW_MATRIX,(double*)m_pdModelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX,(double*)m_pdProjectionMatrix);
	glGetIntegerv(GL_VIEWPORT,m_pnViewport);

	RenderBox(CVector(0,0,-dBaseThickness),CVector(dBaseSize,dBaseSize,0),CVector(0.3,0.3,0.3));
	RenderAxises();

	char A[200];
	sprintf(A,"Fps: %.02f",m_FrameManager.m_piFrameManager->GetCurrentFps());
	m_piSTFps->SetText(A);

	sprintf(A,"Time: %.02f",((double)m_FrameManager.m_piFrameManager->GetCurrentTime())/1000.0);
	if(m_FrameManager.m_piFrameManager->IsPaused())
	{
		strcat(A," (Paused)");
	}
	m_piSTTime->SetText(A);

	CVector entityPos=m_pSelectedEntity?m_pSelectedEntity->GetPosition():CVector(0,0,0);
	sprintf(A,"Pos: %.02f , %.02f , %.02f",entityPos.c[0],entityPos.c[1],entityPos.c[2]);
	m_piSTEntityPos->SetText(A);

	CVector entityVel=m_pSelectedEntity?m_pSelectedEntity->GetVelocity():CVector(0,0,0);
	sprintf(A,"Vel: %.02f , %.02f , %.02f - %.02f/%.02f ",entityVel.c[0],entityVel.c[1],entityVel.c[2],(double)entityVel,m_pSelectedEntity?m_pSelectedEntity->GetMaxVelocity():0);
	m_piSTEntityVel->SetText(A);

	map<string,CIAEntityBase *>::iterator i;
	for(i=m_mEntities.begin();i!=m_mEntities.end();i++)
	{
		i->second->Render();
	}
}

void CIATestMainWindow::UpdateScenarioButtons()
{
	map<ITestScenario *,CButtonWrapper *>::iterator iScenario;
	for(iScenario=m_mScenarios.begin();iScenario!=m_mScenarios.end();iScenario++)
	{
		if(iScenario->first==m_piCurrentScenario)
		{
			iScenario->second->m_piButton->SetBackgroundColor(CVector(0.7,0.7,0.7),1);
		}
		else
		{
			iScenario->second->m_piButton->SetBackgroundColor(CVector(0.5,0.5,0.5),1);
		}
	}
}

void CIATestMainWindow::OnButtonClicked(IGameGUIButton *piControl)
{
	bool bScenarioButton=false;
	map<ITestScenario *,CButtonWrapper *>::iterator iScenario;
	for(iScenario=m_mScenarios.begin();iScenario!=m_mScenarios.end();iScenario++)
	{
		if(iScenario->second->m_piButton==piControl)
		{
			if(iScenario->first!=m_piCurrentScenario)
			{
				bScenarioButton=true;
				ActivateScenario(iScenario->first);
			}
		}
	}
	if(bScenarioButton){return;}

	m_pSelectedEntity=NULL;
	map<string,CButtonWrapper *>::iterator i;
	for(i=m_mEntityButtons.begin();i!=m_mEntityButtons.end();i++)
	{
		CButtonWrapper *pButton=i->second;
		if(pButton->m_piButton==piControl)
		{
			if(m_mEntities.find(i->first)!=m_mEntities.end())
			{
				m_pSelectedEntity=m_mEntities[i->first];
			}

			pButton->m_piButton->SetBackgroundColor(CVector(0.7,0.7,0.7),1);
		}
		else
		{
			pButton->m_piButton->SetBackgroundColor(CVector(0.5,0.5,0.5),1);
		}
	}
}

void CIATestMainWindow::ProcessPhysics(double dTimeFraction)
{
	map<string,CIAEntityBase*>::iterator i;
	for(i=m_mEntities.begin();i!=m_mEntities.end();i++)
	{
		ProcessPhysics(i->second,dTimeFraction);
	}
}

void CIATestMainWindow::ProcessPhysics(CIAEntityBase *pEntity,double dTimeFraction)
{
	CVector vForce=pEntity->GetForce();
	if(vForce>pEntity->GetMaxForce())
	{
		vForce.N();
		vForce*=pEntity->GetMaxForce();
	}

	CVector vAcceleration=vForce/pEntity->GetMass();
	CVector vVelocity=pEntity->GetVelocity()+vAcceleration*dTimeFraction;
	double dVel=vVelocity.N();

	if(dVel>pEntity->GetMaxVelocity())
	{
		dVel=pEntity->GetMaxVelocity();
	}
	vVelocity*=dVel;

	CVector vDisplacement=vVelocity*dTimeFraction;
	CVector vPosition=pEntity->GetPosition()+vDisplacement;

	// Restrict position to z=0 and to the play area;
	vPosition.c[2]=0;
	if(vPosition.c[0]>dBaseSize){vPosition.c[0]=vPosition.c[0]-dBaseSize;}
	if(vPosition.c[1]>dBaseSize){vPosition.c[1]=vPosition.c[1]-dBaseSize;}
	if(vPosition.c[0]<0){vPosition.c[0]=dBaseSize+vPosition.c[0];}
	if(vPosition.c[1]<0){vPosition.c[1]=dBaseSize+vPosition.c[1];}

	if (vVelocity > 0.00000001)
	{    
		CVector vHeading=vVelocity;
		vHeading.N();
		pEntity->SetHeading(vHeading);
	}

	pEntity->SetPosition(vPosition);
	pEntity->SetVelocity(vVelocity);

}


void CIATestMainWindow::ProcessIA(double dTimeFraction)
{
	map<string,CIAEntityBase*>::iterator i;
	for(i=m_mEntities.begin();i!=m_mEntities.end();i++)
	{
		CIAEntityBase *pEntity=i->second;
		pEntity->ProcessIA(dTimeFraction);
	}
}
void CIATestMainWindow::OnMouseDown(int nButton,double x,double y)
{
	if(m_pSelectedEntity)
	{
		CVector vPos=UnProject(x,y);
		m_pSelectedEntity->SetPosition(vPos);
	}
}

CVector	CIATestMainWindow::UnProject(double x, double y)
{
	CVector vScreen;
	CVector vUnprojected;
	gluProject(0,0,0,m_pdModelMatrix,m_pdProjectionMatrix,m_pnViewport,&vScreen.c[0],&vScreen.c[1],&vScreen.c[2]);
	gluUnProject(x,y,vScreen.c[2],m_pdModelMatrix,m_pdProjectionMatrix,m_pnViewport,&vUnprojected.c[0],&vUnprojected.c[1],&vUnprojected.c[2]);
	return vUnprojected;
}	

void CIATestMainWindow::AddEntity(std::string sName,CIAEntityBase *pEntity,bool bAddButton)
{
	m_mEntities[sName]=pEntity;
	if(bAddButton)
	{
		AddEntityButton(sName,false);
	}
}

void CIATestMainWindow::RemoveAllEntities()
{
	m_pSelectedEntity=NULL;
	map<string,CButtonWrapper *>::iterator i;
	for(i=m_mEntityButtons.begin();i!=m_mEntityButtons.end();i++)
	{
		CButtonWrapper *pButton=i->second;
		pButton->m_piButton->DestroyWindow();
		delete pButton;
	}
	
	map<string,CIAEntityBase *>::iterator i2;
	for(i2=m_mEntities.begin();i2!=m_mEntities.end();i2++)
	{
		CIAEntityBase *pEntity=i2->second;
		delete pEntity;
	}
	
	m_mEntityButtons.clear();
	m_mEntities.clear();
	m_pSelectedEntity=NULL;
}

void CIATestMainWindow::AddEntityButton(std::string sName,bool bSelect)
{
	SGameRect buttonRect(2,2,100,20);
	buttonRect.y+=(buttonRect.h+5.0)*m_mEntityButtons.size();

	CButtonWrapper *pButton=new CButtonWrapper;
	pButton->Create("GameGUI","CGameGUIButton",sName);
	pButton->m_piButton->InitWindow(this,false);
	pButton->m_piButton->SetText(sName);
	pButton->m_piButton->SetHorizontalAlignment(eTextAlignment_Center);
	pButton->m_piButton->SetVerticalAlignment(eTextAlignment_Center);
	pButton->m_piButton->SetTextColor(CVector(0,0,0),1);
	pButton->m_piButton->SetBackgroundColor(CVector(0.5,0.5,0.5),1);
	pButton->m_piButton->SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
	pButton->m_piButton->SetRect(&buttonRect);
	pButton->m_piButton->Activate(true);
	SUBSCRIBE_TO_CAST(pButton->m_piButton,IGameGUIButtonEvents);
	m_mEntityButtons[sName]=pButton;

	if(bSelect){OnButtonClicked(pButton->m_piButton);}
}


void CIATestMainWindow::AddScenario(ITestScenario *piScenario)
{
	SGameRect buttonRect(2,2,100,20);
	buttonRect.y=m_rRealRect.h-(buttonRect.h+2);
	buttonRect.x+=(buttonRect.w+5.0/2)*m_mScenarios.size();

	std::string sName=piScenario->GetName();
	CButtonWrapper *pButton=new CButtonWrapper;
	pButton->Create("GameGUI","CGameGUIButton",sName);
	pButton->m_piButton->InitWindow(this,false);
	pButton->m_piButton->SetText(sName);
	pButton->m_piButton->SetHorizontalAlignment(eTextAlignment_Center);
	pButton->m_piButton->SetVerticalAlignment(eTextAlignment_Center);
	pButton->m_piButton->SetTextColor(CVector(0,0,0),1);
	pButton->m_piButton->SetBackgroundColor(CVector(0.5,0.5,0.5),1);
	pButton->m_piButton->SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
	pButton->m_piButton->SetRect(&buttonRect);
	pButton->m_piButton->Activate(true);
	SUBSCRIBE_TO_CAST(pButton->m_piButton,IGameGUIButtonEvents);
	m_mScenarios[piScenario]=pButton;
}

void CIATestMainWindow::RemoveAllScenarios()
{
	if(m_piCurrentScenario){m_piCurrentScenario->Finalize();}

	map<ITestScenario *,CButtonWrapper *>::iterator iScenario;
	for(iScenario=m_mScenarios.begin();iScenario!=m_mScenarios.end();iScenario++)
	{
		CButtonWrapper *pButton=iScenario->second;
		ITestScenario *piScenario=iScenario->first;
		pButton->m_piButton->DestroyWindow();
		delete pButton;
		delete piScenario;
	}
	m_piCurrentScenario=NULL;
	m_mScenarios.clear();
}

void CIATestMainWindow::ActivateScenario(ITestScenario *piScenario)
{
	if(m_piCurrentScenario)
	{
		m_piCurrentScenario->Finalize();
	}	
	RemoveAllEntities();
	AddEntityButton("Camera",true);
	if(piScenario)
	{
		m_piCurrentScenario=piScenario;
		m_piCurrentScenario->Initialize(this);
	}
	UpdateScenarioButtons();
}

void CIATestMainWindow::UpdateRealRect()
{
	CGameWindowBase::UpdateRealRect();
	SGameRect rRect;
	map<ITestScenario *,CButtonWrapper *>::iterator iScenario;
	for(iScenario=m_mScenarios.begin();iScenario!=m_mScenarios.end();iScenario++)
	{
		CButtonWrapper *pButton=iScenario->second;
		pButton->m_piButton->GetRect(&rRect);
		rRect.y=m_rRealRect.h-rRect.h+2;
		pButton->m_piButton->SetRect(&rRect);
	}
}
