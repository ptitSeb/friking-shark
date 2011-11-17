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
#include "resource.h"
#include "GameRunTimeLib.h"
#include "GameGUILib.h"
#include "ScenarioEditorMainWindow.h"

#define SELECT_FORMATION_BASE_INDEX 0x800
#define SELECT_ENTITY_BASE_INDEX    0x400
#define SELECT_PLAY_AREA_INDEX		0x000
#define SELECT_TAKEOFF_BASE			0x100
#define SELECT_LANDING_BASE			0x200
#define SELECT_CHECKPOINT_BASE		0x300

#include "../GameManagers/GameManagers.h"

extern CSystemModuleHelper *g_pSystemModuleHelper;

CScenarioEditorMainWindow::CScenarioEditorMainWindow(void)
{
	m_eColorCallBack=eColorCallBackSun;
	m_bPauseOnNextFrame=false;
	m_bMovingObject=false;
	m_bMovingRoutePoint=false;
	m_bMovingPlayerRoutePoint=false;
	m_bMovingCheckPoint=false;
	m_bAutoGenerateBSP=true;
	m_bAutoUpdateBSP=false;
	m_bRenderPlayArea=false;
	m_d3DFontSize=0;
	m_dMouseTraceDistance=0;
	m_dDifficulty=0;
	m_bGodMode=true;
	m_bUseAmmo=false;
	m_bStats=false;
	m_nLastStatsTime=0;
	
	m_bInspectionMode=false;
	m_bShowTerrainPanel=true;
	m_bShowOptionsPanel=false;
	m_bShowEntitiesPanel=false;
	m_bShowEntityPanel=false;
	m_bShowFormationsPanel=false;
	m_bShowFormationPanel=false;
	m_bShowFilePanel=false;
	m_bShowGeneralPanel=true;
	m_bShowWaterPanel=false;
	m_bShowFogPanel=false;
	m_bShowSunPanel=false;
	m_bShowSkyPanel=false;
	m_bShowPlayAreaPanel=false;
	m_nSelectedRoutePoint=-1;
	m_nSelectedLandingRoutePoint=-1;
	m_nSelectedTakeOffRoutePoint=-1;
	m_nStartingWeapon=1;
	
	m_dwNexControlKey=0;
	m_bTextures=1;
	m_bFog=1;
	m_bShadows=1;
	m_bSolid=1;
	m_bLighting=1;
	m_bNormalMaps=1;
	m_bSkyShadow=1;
	m_bBlend=1;
	m_nSelectedHeightLayer=-1;
	m_nSelectedColorLayer=-1;
	m_nSelectedEntityLayer=-1;
	m_nSelectedEntity=-1;
	m_nSelectedFormation=-1;
	m_nSelectedCheckPoint=-1;
	m_bMovingCameraPosition=false;

	InitializeChildren();
	m_piGameSystem=NULL;
	m_bSimulationStarted=false;
	m_eShadingModel=eShadingModel_Balanced;
}

CScenarioEditorMainWindow::~CScenarioEditorMainWindow(void)
{
}

bool CScenarioEditorMainWindow::InitWindow(IGameWindow *piParent,bool bPopup)
{
	bool bOk=CGameWindowBase::InitWindow(piParent,bPopup);
	if(bOk)
	{
		m_ColorSelector.Attach("GameGUI","ColorSelector");
		m_ObjectSelector.Attach("GameGUI","ObjectSelector");
		m_ObjectListSelector.Attach("GameGUI","ObjectListSelector");
		m_Viewport.Attach("GameGUI","Viewport");
		m_Render.Attach("GameGUI","Render");
		m_Viewport.m_piViewport->SetCaption("Scenario Editor");
		m_Camera.Create("GameGUI","Camera","");

		SGameRect sRect;
		sRect.x=0;
		sRect.y=0;
		sRect.w=1;
		sRect.h=1;
		m_eReferenceSystem=eGameGUIReferenceSystem_Relative;
		SetRect(&sRect);

		m_piGUIManager->SetFocus(this);
	}

	m_piSTEntityLayerObjectLabel->Activate(false);
	m_piSTEntityObjectLabel->Activate(false);
	m_piSTEntityBonusObjectLabel->Activate(false);
	m_piSTFormationObjectLabel->Activate(false);
	m_piSTFormationBonusObjectLabel->Activate(false);
	m_piSTFormationAlternativeObjectLabel->Activate(false);
	m_piSTFormationAlternativeBonusObjectLabel->Activate(false);

	UpdateLayerPanel();

	CSystemLoaderHelper     systemLoader;
	m_piGameSystem=systemLoader.LoadSystem("Scripts/GameSystem.cfg","GameSystem");
	m_GameControllerWrapper.Attach("GameSystem","GameController");
	if(m_GameControllerWrapper.m_piGameController)
	{
	  m_GameControllerWrapper.m_piGameController->SetupGame();
	  m_GameControllerWrapper.m_piGameController->CreateScenario();
	}

	m_PlayAreaManagerWrapper.Attach("GameSystem","PlayAreaManager");
	m_PlayerManagerWrapper.Attach("GameSystem","PlayerManager");
	m_MusicManagerWrapper.Attach("GameSystem","MusicManager");
	m_FrameManager.Attach("GameSystem","FrameManager");
	m_WorldManagerWrapper.Attach("GameSystem","WorldManager");
	m_EntityManagerWrapper.Attach("GameSystem","EntityManager");
	m_SoundManagerWrapper.Attach("GameGUI","SoundManager");
	m_PlayerProfile.Create("GameSystem","CPlayerProfile","");
	if(m_PlayerManagerWrapper.m_piPlayerManager)
	{
		m_PlayerManagerWrapper.m_piPlayerManager->SetPlayerProfile(m_PlayerProfile.m_piPlayerProfile);
	}
	
	
	//OpenScenario("/home/javi/workspace/AndroidPort/Demo/Resources/Level1.ges");
	return bOk;
}

void CScenarioEditorMainWindow::DestroyWindow()
{
	StopGameSimulation();
	Reset();
	if(m_GameControllerWrapper.m_piGameController){m_GameControllerWrapper.m_piGameController->EndGame();}
	m_SoundManagerWrapper.Detach();
	m_PlayerManagerWrapper.Detach();
	m_MusicManagerWrapper.Detach();
	m_PlayAreaManagerWrapper.Detach();
	m_GameControllerWrapper.Detach();
	m_WorldManagerWrapper.Detach();
	m_PlayerProfile.Detach();

	m_FrameManager.Detach();
	if(m_piGameSystem){m_piGameSystem->Destroy();}
	REL(m_piGameSystem);

	m_ColorSelector.Detach();
	m_ObjectSelector.Detach();
	m_ObjectListSelector.Detach();	
	m_Viewport.Detach();
	m_Render.Detach();
	m_Camera.Detach();

	UNSUBSCRIBE_FROM_ALL();
	CGameWindowBase::DestroyWindow();
}

void CScenarioEditorMainWindow::Reset()
{
	StopGameSimulation();
	if(m_GameControllerWrapper.m_piGameController){m_GameControllerWrapper.m_piGameController->CloseScenario();}
	if(m_piSTEntityLayerObjectLabel){m_piSTEntityLayerObjectLabel->SetObject(NULL);}
	if(m_piSTEntityObjectLabel){m_piSTEntityObjectLabel->SetObject(NULL);}
	if(m_piSTEntityBonusObjectLabel){m_piSTEntityBonusObjectLabel->SetObject(NULL);}
	if(m_piSTFormationObjectLabel){m_piSTFormationObjectLabel->SetObject(NULL);}
	if(m_piSTFormationBonusObjectLabel){m_piSTFormationBonusObjectLabel->SetObject(NULL);}
	if(m_piSTFormationAlternativeObjectLabel){m_piSTFormationAlternativeObjectLabel->SetObject(NULL);}
	if(m_piSTFormationAlternativeBonusObjectLabel){m_piSTFormationAlternativeBonusObjectLabel->SetObject(NULL);}
	UpdateColorLayerControls();
	UpdateHeightLayerControls();
	UpdateEntityLayerControls();
	UpdateEntityControls();
	UpdateFormationControls();

	m_sFile="";
	m_nSelectedColorLayer=-1;
	m_nSelectedHeightLayer=-1;
	m_nSelectedEntityLayer=-1;
	m_nSelectedEntity=-1;
	m_nSelectedFormation=-1;
	m_nSelectedCheckPoint=-1;
	m_bShowFilePanel=false;
	m_bShowGeneralPanel=true;
	m_bShowTerrainPanel=true;
	m_bShowEntitiesPanel=false;
	m_bShowEntityPanel=false;
	m_bShowFormationsPanel=false;
	m_bShowFormationPanel=false;
	m_bShowWaterPanel=false;
	m_bShowFogPanel=false;
	m_bShowSunPanel=false;
	m_bShowSkyPanel=false;
	m_bShowPlayAreaPanel=false;
	m_bShowOptionsPanel=false;
	m_bSimulationStarted=false;
	m_bInspectionMode=false;
	m_bMovingRoutePoint=false;
	m_nSelectedRoutePoint=-1;
	
	if(m_piLSEntityList){m_piLSEntityList->SetSelectedElement(m_nSelectedEntity);}
	if(m_piLSFormationList){m_piLSFormationList->SetSelectedElement(m_nSelectedFormation);}
	
	UpdateCaption();
}

void CScenarioEditorMainWindow::ProcessInput(double dTimeFraction,double dRealTimeFraction)
{
	if(!m_piGUIManager->HasFocus(this)){return;}
	
	if(!m_bSimulationStarted || m_bInspectionMode)
	{
		if(m_piGUIManager->IsKeyDown(GK_UP) || m_piGUIManager->IsKeyDown(GK_NUMPAD8) || m_piGUIManager->IsKeyDown('W')){ProcessKey(KEY_FORWARD,dTimeFraction,dRealTimeFraction);}
		if(m_piGUIManager->IsKeyDown(GK_DOWN) || m_piGUIManager->IsKeyDown(GK_NUMPAD2) || m_piGUIManager->IsKeyDown('S')){ProcessKey(KEY_BACK,dTimeFraction,dRealTimeFraction);}
		if(m_piGUIManager->IsKeyDown(GK_LEFT) || m_piGUIManager->IsKeyDown(GK_NUMPAD4) || m_piGUIManager->IsKeyDown('A')){ProcessKey(KEY_LEFT,dTimeFraction,dRealTimeFraction);}
		if(m_piGUIManager->IsKeyDown(GK_RIGHT) || m_piGUIManager->IsKeyDown(GK_NUMPAD6) || m_piGUIManager->IsKeyDown('D')){ProcessKey(KEY_RIGHT,dTimeFraction,dRealTimeFraction);}
		if(m_piGUIManager->IsKeyDown(GK_NUMPAD9) || m_piGUIManager->IsKeyDown('R')){ProcessKey(KEY_UP,dTimeFraction,dRealTimeFraction);}
		if(m_piGUIManager->IsKeyDown(GK_NUMPAD3) || m_piGUIManager->IsKeyDown('F')){ProcessKey(KEY_DOWN,dTimeFraction,dRealTimeFraction);}
	}
}

void CScenarioEditorMainWindow::ProcessKey(unsigned short nKey,double dTimeFraction,double dRealTimeFraction)
{
	double dMovementInspectionSpeed=500.0;
	double dForwardSpeed=dMovementInspectionSpeed*dTimeFraction;
	double dCameraForwardSpeed=dMovementInspectionSpeed*dRealTimeFraction;
	if(m_piGUIManager->IsKeyDown(GK_LSHIFT)){dForwardSpeed*=3.0;}
	if(m_piGUIManager->IsKeyDown(GK_LCONTROL))
	{
		CVector vChange;
		if(nKey==KEY_FORWARD)	{vChange.c[PITCH]-=dCameraForwardSpeed*0.3;}
		else if(nKey==KEY_BACK)	{vChange.c[PITCH]+=dCameraForwardSpeed*0.3;}
		else if(nKey==KEY_LEFT)	{vChange.c[YAW]+=dCameraForwardSpeed*0.3;}
		else if(nKey==KEY_RIGHT)	{vChange.c[YAW]-=dCameraForwardSpeed*0.3;}
		CVector vAngles=m_Camera.m_piCamera->GetAngles()+vChange;
		m_Camera.m_piCamera->SetAngles(vAngles);
	}
	else
	{
		if(nKey==KEY_UP)	{CVector vCameraPos=m_Camera.m_piCamera->GetPosition()+m_Camera.m_piCamera->GetUpVector()*(dCameraForwardSpeed);m_Camera.m_piCamera->SetPosition(vCameraPos);}
		else if(nKey==KEY_DOWN)	{CVector vCameraPos=m_Camera.m_piCamera->GetPosition()-m_Camera.m_piCamera->GetUpVector()*(dCameraForwardSpeed);m_Camera.m_piCamera->SetPosition(vCameraPos);}
		else if(nKey==KEY_LEFT)	{CVector vCameraPos=m_Camera.m_piCamera->GetPosition()-m_Camera.m_piCamera->GetRightVector()*(dCameraForwardSpeed);m_Camera.m_piCamera->SetPosition(vCameraPos);}
		else if(nKey==KEY_RIGHT){CVector vCameraPos=m_Camera.m_piCamera->GetPosition()+m_Camera.m_piCamera->GetRightVector()*(dCameraForwardSpeed);m_Camera.m_piCamera->SetPosition(vCameraPos);}
		else if(nKey==KEY_FORWARD)	{CVector vCameraPos=m_Camera.m_piCamera->GetPosition()+m_Camera.m_piCamera->GetForwardVector()*(dCameraForwardSpeed);m_Camera.m_piCamera->SetPosition(vCameraPos);}
		else if(nKey==KEY_BACK)	{CVector vCameraPos=m_Camera.m_piCamera->GetPosition()-m_Camera.m_piCamera->GetForwardVector()*(dCameraForwardSpeed);m_Camera.m_piCamera->SetPosition(vCameraPos);}
	}

}
void CScenarioEditorMainWindow::SetupRenderOptions(IGenericRender *piRender,IGenericCamera *piCamera)
{
	double cx=m_rRealRect.h*piCamera->GetAspectRatio();
	double dx=(m_rRealRect.w-cx)*0.5;

	double dNearPlane=0,dFarPlane=0;
	CVector vAngles,vPosition;
	vAngles=piCamera->GetAngles();
	vPosition=piCamera->GetPosition();
	piCamera->GetClippingPlanes(dNearPlane,dFarPlane);
	double dViewAngle=piCamera->GetViewAngle();

	piRender->SetViewport(dx,0,cx,m_rRealRect.h);
	piRender->SetPerspectiveProjection(dViewAngle,dNearPlane,dFarPlane);
	piRender->SetCamera(vPosition,vAngles.c[YAW],vAngles.c[PITCH],vAngles.c[ROLL]);
}

void CScenarioEditorMainWindow::RenderPlayArea(bool bSelectionRender)
{
	m_Render.m_piRender->PushState();
	m_Render.m_piRender->ActivateBlending();
	
	CVector vPlayAreaMins,vPlayAreaMaxs,vStart,vEnd;
	SPlayAreaConfig sPlayAreaConfig;
	m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetPlayAreaConfig(&sPlayAreaConfig);
	m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&vStart,&vEnd);
	m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayAreaPlaneAt(m_vPlayMovementPosition,&vPlayAreaMins,&vPlayAreaMaxs);
	CVector vCenter=(vPlayAreaMaxs+vPlayAreaMins)*0.5;
	CVector vSize=(vPlayAreaMaxs-vPlayAreaMins);
	
	m_Render.m_piRender->ActivateSolid();
	if(bSelectionRender){m_Render.m_piRender->SetSelectionId(SELECT_PLAY_AREA_INDEX);}
	
	m_Render.m_piRender->RenderRect(vCenter,AxisPosX,AxisPosZ,vSize.c[0],vSize.c[2],ColorWhite,0.1);
	//Left scroll
	m_Render.m_piRender->RenderRect(vCenter-CVector(0,0,vSize.c[2]*0.5+sPlayAreaConfig.dCameraScroll*0.5),AxisPosX,AxisPosZ,vSize.c[0],sPlayAreaConfig.dCameraScroll,ColorWhite,0.05);
	//Right scroll
	m_Render.m_piRender->RenderRect(vCenter+CVector(0,0,vSize.c[2]*0.5+sPlayAreaConfig.dCameraScroll*0.5),AxisPosX,AxisPosZ,vSize.c[0],sPlayAreaConfig.dCameraScroll,ColorWhite,0.05);
	
	m_Render.m_piRender->DeactivateDepth();
	m_Render.m_piRender->DeactivateSolid();
	
	m_Render.m_piRender->RenderRect(vCenter,AxisPosX,AxisPosZ,vSize.c[0],vSize.c[2],ColorWhite,1.0);
	m_Render.m_piRender->RenderRect(vCenter-CVector(0,0,vSize.c[2]*0.5+sPlayAreaConfig.dCameraScroll*0.5),AxisPosX,AxisPosZ,vSize.c[0],sPlayAreaConfig.dCameraScroll,ColorWhite,1.0);
	m_Render.m_piRender->RenderRect(vCenter+CVector(0,0,vSize.c[2]*0.5+sPlayAreaConfig.dCameraScroll*0.5),AxisPosX,AxisPosZ,vSize.c[0],sPlayAreaConfig.dCameraScroll,ColorWhite,1.0);
	m_Render.m_piRender->PopState();
}

void CScenarioEditorMainWindow::RenderPlayerRoutes(bool bSelectionRender)
{
	SPlayerConfig sPlayerConfig;
	m_PlayerManagerWrapper.m_piPlayerManager->GetPlayerConfig(&sPlayerConfig);
	
	m_Render.m_piRender->PushState();
	m_Render.m_piRender->ActivateBlending();
	if(!bSelectionRender){m_Render.m_piRender->DeactivateDepth();}
	
	for(int x=0;x<4;x++)
	{
		if(sPlayerConfig.bPlayerTakeOffEnabled)
		{
			if(bSelectionRender){m_Render.m_piRender->SetSelectionId(SELECT_TAKEOFF_BASE+x);}
			m_Render.m_piRender->RenderPoint(sPlayerConfig.pvPlayerTakeOffPoints[x],m_nSelectedTakeOffRoutePoint==x?15:10,CVector(0,0,1),1.0);
		}
		if(sPlayerConfig.bPlayerLandingEnabled)
		{
			if(bSelectionRender){m_Render.m_piRender->SetSelectionId(SELECT_LANDING_BASE+x);}
			m_Render.m_piRender->RenderPoint(sPlayerConfig.pvPlayerLandingPoints[x],m_nSelectedLandingRoutePoint==x?15:10,CVector(1,0,0),1.0);
		}
	}
	if(!bSelectionRender)
	{
		for(int x=0;x<3;x++)
		{
			if(sPlayerConfig.bPlayerTakeOffEnabled)
			{
				m_Render.m_piRender->RenderLine(sPlayerConfig.pvPlayerTakeOffPoints[x],sPlayerConfig.pvPlayerTakeOffPoints[x+1],0xFFFF,ColorBlue,1);
			}
			if(sPlayerConfig.bPlayerLandingEnabled)
			{
				m_Render.m_piRender->RenderLine(sPlayerConfig.pvPlayerLandingPoints[x],sPlayerConfig.pvPlayerLandingPoints[x+1],0xFFFF,ColorRed,1);
			}
		}
	}
	
	m_Render.m_piRender->PopState();
}

void CScenarioEditorMainWindow::RenderCheckPoints(bool bSelectionRender)
{
	SPlayerConfig sPlayerConfig;
	m_PlayerManagerWrapper.m_piPlayerManager->GetPlayerConfig(&sPlayerConfig);
	
	m_Render.m_piRender->PushState();
	m_Render.m_piRender->ActivateBlending();
	if(!bSelectionRender){m_Render.m_piRender->DeactivateDepth();}
	
	for(unsigned int x=0;x<m_vCheckPoints.size();x++)
	{
		if(bSelectionRender){m_Render.m_piRender->SetSelectionId(SELECT_CHECKPOINT_BASE+x);}
		m_Render.m_piRender->RenderPoint(m_vCheckPoints[x]->m_piPlayAreaCheckPoint->GetCheckPointPosition(),m_nSelectedCheckPoint==(int)x?15:10,CVector(1,0,1),1.0);
		if(!bSelectionRender && m_nSelectedCheckPoint==(int)x)
		{
			CVector vPlayAreaMins,vPlayAreaMaxs,vStart,vEnd;
			m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayAreaPlaneAt(m_vCheckPoints[x]->m_piPlayAreaCheckPoint->GetCheckPointPosition(),&vPlayAreaMins,&vPlayAreaMaxs);
			CVector vCenter=(vPlayAreaMaxs+vPlayAreaMins)*0.5;
			CVector vSize=(vPlayAreaMaxs-vPlayAreaMins);
			
			CVector vColor(1,0,1);
			m_Render.m_piRender->ActivateSolid();
			m_Render.m_piRender->RenderRect(vCenter,AxisPosX,AxisPosZ,vSize.c[0],vSize.c[2],vColor,0.1);

			m_Render.m_piRender->DeactivateSolid();
			m_Render.m_piRender->RenderRect(vCenter,AxisPosX,AxisPosZ,vSize.c[0],vSize.c[2],vColor,1.0);
		}
	}
	if(!bSelectionRender)
	{
		m_Render.m_piRender->RenderLine(sPlayerConfig.pvPlayerTakeOffPoints[3],sPlayerConfig.pvPlayerLandingPoints[0],0xFFFF,CVector(1,0,1),1);
	}
	
	m_Render.m_piRender->PopState();
}


void CScenarioEditorMainWindow::OnDraw(IGenericRender *piRender)
{
	if(!m_FrameManager.m_piFrameManager)
	{
		return;
	}
	m_Camera.m_piCamera->SetAspectRatio(m_rRealRect.w/m_rRealRect.h);

	m_FrameManager.m_piFrameManager->ProcessFrame();
	ProcessInput(m_FrameManager.m_piFrameManager->GetTimeFraction(),m_FrameManager.m_piFrameManager->GetRealTimeFraction());
	
	if(m_bPauseOnNextFrame){m_FrameManager.m_piFrameManager->SetPauseOnNextFrame(true);m_bPauseOnNextFrame=false;}
	
	m_Render.m_piRender->PushOptions();
	m_Render.m_piRender->PushState();
	m_Render.m_piRender->ActivateDepth();
	
	m_bTextures?m_Render.m_piRender->EnableTextures():m_Render.m_piRender->DisableTextures();
	m_bSolid?m_Render.m_piRender->EnableSolid():m_Render.m_piRender->DisableSolid();
	m_bBlend?m_Render.m_piRender->EnableBlending():m_Render.m_piRender->DisableBlending();
	m_bLighting?m_Render.m_piRender->EnableLighting():m_Render.m_piRender->DisableLighting();
	m_bNormalMaps?m_Render.m_piRender->EnableNormalMaps():m_Render.m_piRender->DisableNormalMaps();
	m_bSkyShadow?m_Render.m_piRender->EnableSkyShadow():m_Render.m_piRender->DisableSkyShadow();
	m_bShadows?m_Render.m_piRender->EnableShadows():m_Render.m_piRender->DisableShadows();
	m_bFog?m_Render.m_piRender->EnableHeightFog():m_Render.m_piRender->DisableHeightFog();
	m_bStats?m_Render.m_piRender->EnableStagedRenderingStats():m_Render.m_piRender->DisableStagedRenderingStats();

	if(m_bSimulationStarted)
	{		
		if(!m_bInspectionMode){m_PlayerManagerWrapper.m_piPlayerManager->ProcessInput(m_piGUIManager,m_FrameManager.m_piFrameManager->GetCurrentTime(),m_FrameManager.m_piFrameManager->GetTimeFraction());m_Render.m_piRender->DisableAutoShadowVolume();}
		m_GameControllerWrapper.m_piGameController->ProcessFrame(m_FrameManager.m_piFrameManager->GetCurrentTime(),m_FrameManager.m_piFrameManager->GetTimeFraction());
		IGenericCamera *piPlayCamera=m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCamera();
		IGenericCamera *piCamera=NULL;
		piCamera=ADD(m_bInspectionMode?m_Camera.m_piCamera:piPlayCamera);
		if(piCamera)
		{
			SetupRenderOptions(piRender,piCamera);
			piRender->StartStagedRendering();
			piRender->PushState();
			piRender->SetShadingModel(m_eShadingModel);
			m_WorldManagerWrapper.m_piWorldManager->SetupRenderingEnvironment(piRender);
			m_EntityManagerWrapper.m_piEntityManager->RenderEntities(piRender,piCamera);
			piRender->PopState();
			piRender->EndStagedRendering();
		}
		REL(piCamera);
		REL(piPlayCamera);
	}
	else
	{
		SetupRenderOptions(piRender,m_Camera.m_piCamera);
		piRender->StartStagedRendering();
		piRender->PushState();
		piRender->SetShadingModel(m_eShadingModel);
		
		m_WorldManagerWrapper.m_piWorldManager->SetupRenderingEnvironment(piRender);
		m_WorldManagerWrapper.m_piTerrain->DesignRender(piRender);
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->DesignRender(piRender);
		if(m_nSelectedFormation!=-1)
		{
			CVector vPos=m_vFormationControls[m_nSelectedFormation]->m_piPlayAreaFormation->GetPosition();
			m_vFormationControls[m_nSelectedFormation]->m_piDesignObject->DesignRender(piRender,vPos,Origin,true);
		}
		if(m_nSelectedEntity!=-1)
		{
			m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->DesignRender(piRender,true);
		}
		
		RenderRoute(piRender,m_nSelectedEntity,m_nSelectedRoutePoint);
		
		piRender->PopState();
		piRender->EndStagedRendering();

		if(m_nSelectedEntity!=-1)
		{
			double dFontSize=0;
			IGenericFont *piFont=NULL;
			GetFont(&piFont,&dFontSize);
			if(m_d3DFontSize>0){dFontSize=m_d3DFontSize;}
			char sDescr[128];
			unsigned int nDescrLen=0;
			unsigned int nCount=m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetCount();
			int nDelay=m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetDelay();
			if(nCount>1)
			{
				nDescrLen+=sprintf(sDescr+nDescrLen,"x%d",nCount);
				unsigned int nInterval=m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetInterval();
				if(nInterval){nDescrLen+=sprintf(sDescr+nDescrLen," (%.02f s)",((double)nInterval)/1000.0);}		
			}
			if(nDelay){nDescrLen+=sprintf(sDescr+nDescrLen," + %.02f s",((double)nDelay)/1000.0);}
			if(nDescrLen)
			{
				CVector vPos=m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetPosition();
				IEntityType *piType=NULL;
				m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetEntityType(&piType);
				if(piType)
				{
					vPos+=m_Camera.m_piCamera->GetUpVector()*piType->DesignGetRadius();
					vPos-=m_Camera.m_piCamera->GetRightVector()*piType->DesignGetRadius();
				}
				if(piFont){piFont->RenderText(piRender,dFontSize,vPos,sDescr,ColorWhite,1);}
				REL(piType);
			}
			REL(piFont);		
		}
	}

	m_Render.m_piRender->PopOptions();
	m_Render.m_piRender->PopState();


	if(m_bRenderPlayArea)
	{
		RenderPlayArea(false);
		RenderPlayerRoutes(false);
		RenderCheckPoints(false);
	}


	CVector vSunPosition;
	m_Render.m_piRender->GetSunLight(&vSunPosition,NULL,NULL,NULL,NULL);
	m_Render.m_piRender->RenderPoint(vSunPosition,10,CVector(0,0,1),1.0);
	m_Render.m_piRender->RenderLine(vSunPosition,vSunPosition+CVector(0,-vSunPosition.c[1],0),0x8888,CVector(0,0,1),1);
	
	if(m_piSTFps)
	{
		char A[200];
		sprintf(A,"Fps: %.02f",m_FrameManager.m_piFrameManager->GetCurrentFps());
		m_piSTFps->SetText(A);
	}
	if(m_piSTGameTime)
	{
		int nTime=0,nTotalMins=0,nTotalSecs=0;
		
		SPlayAreaConfig sPlayAreaConfig;
		if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign)
		{
			CVector vStart,vEnd,vPos;
			vPos=m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayMovementPosition();
			m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&vStart,&vEnd);
			m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetPlayAreaConfig(&sPlayAreaConfig);
			
			int nTotalTime=(vEnd.c[0]-vStart.c[0])/sPlayAreaConfig.dCameraSpeed;
			nTotalMins=nTotalTime/60;
			nTotalSecs=nTotalTime%60;
			
			nTime+=(m_vPlayMovementPosition.c[0]-vStart.c[0])/sPlayAreaConfig.dCameraSpeed;
		}
		
		if(m_bSimulationStarted && m_FrameManager.m_piFrameManager && m_PlayAreaManagerWrapper.m_piPlayAreaManager)
		{
			nTime+=m_FrameManager.m_piFrameManager->GetCurrentTime()/1000.0;
		}
		int nMins=nTime/60;
		int nSecs=nTime%60;
		char A[200];
		sprintf(A,"%02d:%02d - %02d:%02d",nMins,nSecs,nTotalMins,nTotalSecs);
		m_piSTGameTime->SetText(A);
	}
	if(m_piSTOptionsVolume && m_SoundManagerWrapper.m_piSoundManager)
	{
		char A[200];
		sprintf(A,"Volume: %d%%",m_SoundManagerWrapper.m_piSoundManager->GetMasterVolume());
		m_piSTOptionsVolume->SetText(A);
	}	
	if(m_piSTOptionsSoundFxVolume && m_SoundManagerWrapper.m_piSoundManager)
	{
		char A[200];
		sprintf(A," FX:    %d%%",m_SoundManagerWrapper.m_piSoundManager->GetGroupVolume("SoundFX"));
		m_piSTOptionsSoundFxVolume->SetText(A);
	}	
	if(m_piSTOptionsMusicVolume && m_SoundManagerWrapper.m_piSoundManager)
	{
		char A[200];
		sprintf(A," Music: %d%%",m_SoundManagerWrapper.m_piSoundManager->GetGroupVolume("Music"));
		m_piSTOptionsMusicVolume->SetText(A);
	}	
	if(m_piSTOptionsDifficulty && m_SoundManagerWrapper.m_piSoundManager)
	{
		char A[200];
		sprintf(A,"Difficulty: %.f",m_dDifficulty);
		m_piSTOptionsDifficulty->SetText(A);
		m_PlayerProfile.m_piPlayerProfile->SetDifficulty(m_dDifficulty);
	}
	if(m_piBTOptionsGod && m_SoundManagerWrapper.m_piSoundManager)
	{
		char A[200];
		sprintf(A,"God: %s",m_bGodMode?"On":"Off");
		m_piBTOptionsGod->SetText(A);
	}
	if(m_piBTOptionsAmmo && m_SoundManagerWrapper.m_piSoundManager)
	{
		char A[200];
		sprintf(A,"Ammo: %s",m_bUseAmmo?"On":"Off");
		m_piBTOptionsAmmo->SetText(A);
	}
	if(m_piSTOptionsMouseTraceDistance)
	{
		char A[200];
		sprintf(A,"Trace Dist: %.f",m_dMouseTraceDistance);
		m_piSTOptionsMouseTraceDistance->SetText(A);
	}
	if(m_bStats && m_nLastStatsTime+1000<GetTimeStamp())
	{
		m_Render.m_piRender->DumpStagedRenderingStats();
		m_nLastStatsTime=GetTimeStamp();		
	}
	UpdateLayerPanel();
	
	m_piGUIManager->RestoreViewport();
}

void CScenarioEditorMainWindow::ProcessFileNew()
{
	if(ConfirmDialog("Reset all and start a new project?","New project",eMessageDialogType_Warning))
	{
		Reset();
		if(m_GameControllerWrapper.m_piGameController)
		{		
			m_GameControllerWrapper.m_piGameController->CreateScenario();	
		}
		
		if(m_PlayAreaManagerWrapper.m_piPlayAreaManager)
		{
			CVector vStart,vEnd;
			m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&vStart,&vEnd);
			m_vPlayMovementPosition=vStart;
		}
		
		UpdateColorLayerControls();
		UpdateHeightLayerControls();
		UpdateEntityLayerControls();
		UpdateEntityControls();
		UpdateFormationControls();
		UpdateLayerPanel();
		UpdateCaption();
		CenterCamera();
		m_bShowFilePanel=false;
	}
}

void CScenarioEditorMainWindow::ProcessFileOpen()
{
	std::string sScenario="./";
	if(OpenFileDialog("Load scenario...",".ges",&sScenario))
	{
		unsigned int nStartTime=GetTimeStamp();
		OpenScenario(sScenario);
		RTTRACE("CScenarioEditorMainWindow::ProcessFileOpen -> Scenario %s loaded (%d ms)",sScenario.c_str(),GetTimeStamp()-nStartTime);
	}
}

void CScenarioEditorMainWindow::ProcessFileSave()
{
	std::string sScenario=m_sFile;
	bool bSave=true;
	if(m_sFile.length()==0)
	{
		bSave=SaveFileDialog("Save scenario...","Scenario files (*.ges)\0*.ges\0\0",&sScenario,true);
	}
	if(bSave)
	{
		m_GameControllerWrapper.m_piGameController->SaveScenario(sScenario);	
		m_sFile=sScenario;
	}
}

void CScenarioEditorMainWindow::ProcessFileSaveAs()
{
	std::string sScenario=m_sFile;
	if(SaveFileDialog("Save scenario...","Scenario files (*.ges)\0*.ges\0\0",&sScenario,true))
	{
		m_GameControllerWrapper.m_piGameController->SaveScenario(sScenario);	
		m_sFile=sScenario;
		UpdateCaption();
	}
}
void CScenarioEditorMainWindow::ProcessFileExit()
{
	if(ConfirmDialog("Do you really want to exit?","Scenario Editor",eMessageDialogType_Question))
	{
		m_piGUIManager->ExitGUILoop();
	}
}

void CScenarioEditorMainWindow::OnSelectionChanged(IGameGUIList *piControl,int nElement,std::string sElement)
{
	if(piControl==m_piLSEntityList)
	{
		m_nSelectedEntityLayer=-1;
		m_nSelectedEntity=nElement;
		m_nSelectedFormation=-1;
		if(m_piLSFormationList){m_piLSFormationList->SetSelectedElement(m_nSelectedFormation);}
		m_piGUIManager->SetFocus(this);
	}
	if(piControl==m_piLSFormationList)
	{
		m_nSelectedFormation=nElement;
		m_nSelectedEntity=-1;
		m_nSelectedEntityLayer=-1;
		if(m_piLSEntityList){m_piLSEntityList->SetSelectedElement(m_nSelectedEntity);}
		m_piGUIManager->SetFocus(this);
	}
}

void CScenarioEditorMainWindow::OnSelectionDoubleCliked(IGameGUIList *piControl,int nElement,std::string sElement)
{
	if(piControl==m_piLSEntityList)
	{
		m_nSelectedEntityLayer=-1;
		m_nSelectedEntity=nElement;
		m_nSelectedFormation=-1;
		if(m_piLSFormationList){m_piLSFormationList->SetSelectedElement(m_nSelectedFormation);}
		m_piGUIManager->SetFocus(this);
		CenterCamera();
	}
	if(piControl==m_piLSFormationList)
	{
		m_nSelectedFormation=nElement;
		m_nSelectedEntity=-1;
		m_nSelectedEntityLayer=-1;
		if(m_piLSEntityList){m_piLSEntityList->SetSelectedElement(m_nSelectedEntity);}
		m_piGUIManager->SetFocus(this);
		CenterCamera();
	}
}


void CScenarioEditorMainWindow::OnButtonClicked(IGameGUIButton *piControl)
{
	if(m_piBTOptionsIncreaseVolume==piControl && m_SoundManagerWrapper.m_piSoundManager)
	{
	  int volume=m_SoundManagerWrapper.m_piSoundManager->GetMasterVolume();
	  volume+=5;
	  m_SoundManagerWrapper.m_piSoundManager->SetMasterVolume(volume);
	}
	if(m_piBTOptionsDecreaseVolume==piControl && m_SoundManagerWrapper.m_piSoundManager)
	{
	  int volume=m_SoundManagerWrapper.m_piSoundManager->GetMasterVolume();
	  volume-=5;
	  if(volume<0){volume=0;}
	  m_SoundManagerWrapper.m_piSoundManager->SetMasterVolume(volume);
	}
	if(m_piBTOptionsIncreaseSoundFxVolume==piControl && m_SoundManagerWrapper.m_piSoundManager)
	{
		int volume=m_SoundManagerWrapper.m_piSoundManager->GetGroupVolume("SoundFX");
		volume+=5;
		m_SoundManagerWrapper.m_piSoundManager->SetGroupVolume("SoundFX",volume);
	}
	if(m_piBTOptionsDecreaseSoundFxVolume==piControl && m_SoundManagerWrapper.m_piSoundManager)
	{
		int volume=m_SoundManagerWrapper.m_piSoundManager->GetGroupVolume("SoundFX");
		volume-=5;
		if(volume<0){volume=0;}
		m_SoundManagerWrapper.m_piSoundManager->SetGroupVolume("SoundFX",volume);
	}
	if(m_piBTOptionsIncreaseMusicVolume==piControl && m_SoundManagerWrapper.m_piSoundManager)
	{
		int volume=m_SoundManagerWrapper.m_piSoundManager->GetGroupVolume("Music");
		volume+=5;
		m_SoundManagerWrapper.m_piSoundManager->SetGroupVolume("Music",volume);
	}
	if(m_piBTOptionsDecreaseMusicVolume==piControl && m_SoundManagerWrapper.m_piSoundManager)
	{
		int volume=m_SoundManagerWrapper.m_piSoundManager->GetGroupVolume("Music");
		volume-=5;
		if(volume<0){volume=0;}
		m_SoundManagerWrapper.m_piSoundManager->SetGroupVolume("Music",volume);
	}
	if(m_piBTOptionsIncreaseDifficulty==piControl && m_SoundManagerWrapper.m_piSoundManager)
	{
		m_dDifficulty+=1.0;
	}
	if(m_piBTOptionsDecreaseDifficulty==piControl && m_SoundManagerWrapper.m_piSoundManager)
	{
		m_dDifficulty-=1.0;
		if(m_dDifficulty<0){m_dDifficulty=0;}
	}	
	if(m_piBTOptionsGod==piControl)
	{
		m_bGodMode=!m_bGodMode;
		
		IEntity *piPlayerEntity=m_EntityManagerWrapper.m_piEntityManager->FindEntity("Player");
		IPlayer *piPlayer=dynamic_cast<IPlayer*>(piPlayerEntity);
		if(piPlayer){piPlayer->SetGodMode(m_bGodMode);}
	}
	if(m_piBTOptionsAmmo==piControl)
	{
		m_bUseAmmo=!m_bUseAmmo;
	
		IEntity *piPlayerEntity=m_EntityManagerWrapper.m_piEntityManager->FindEntity("Player");
		IWeapon *piBombWeapon=piPlayerEntity?piPlayerEntity->GetWeapon(1):NULL;
		if(piBombWeapon){piBombWeapon->SetAmmo(m_bUseAmmo?3:10000);}
	}
	if(m_piBTOptionsIncreaseMouseTraceDistance==piControl)
	{
		m_dMouseTraceDistance++;
	}
	if(m_piBTOptionsDecreaseMouseTraceDistance==piControl && m_dMouseTraceDistance>0)
	{
		m_dMouseTraceDistance--;
		if(m_dMouseTraceDistance<0){m_dMouseTraceDistance=0;}
	}
	
	if(m_piBTFileNew==piControl)
	{
		ProcessFileNew();
		m_bShowFilePanel=false;
	}
	if(m_piBTFileOpen==piControl)
	{
		ProcessFileOpen();
		m_bShowFilePanel=false;
	}
	if(m_piBTFileSave==piControl)
	{
		ProcessFileSave();
		m_bShowFilePanel=false;
	}
	if(m_piBTFileSaveAs==piControl)
	{
		ProcessFileSaveAs();
		m_bShowFilePanel=false;
	}
	if(m_piBTFileExit==piControl)
	{
		ProcessFileExit();
		m_bShowFilePanel=false;
	}
	if(m_piBTGeneralChangeModel==piControl)
	{
		std::string sBaseModel="./Models/";
		if(OpenFileDialog("Select model...",".ase",&sBaseModel))
		{
			m_WorldManagerWrapper.m_piTerrain->SetTerrainBaseModel(sBaseModel);
		}
		UpdateTexturization();
		CenterCamera();
	}
	if(m_piBTGeneralChangeMusic==piControl)
	{
		std::string sMusic="./Sounds/";
		if(OpenFileDialog("Select music...",".wav;.ogg",&sMusic))
		{
			m_MusicManagerWrapper.m_piMusicDesign->SetMusic(sMusic);
		}
	}
	if(m_piBTGeneralChangeIntroMusic==piControl)
	{
		std::string sMusic="./Sounds/";
		if(OpenFileDialog("Select intro music...",".wav;.ogg",&sMusic))
		{
			m_MusicManagerWrapper.m_piMusicDesign->SetIntroMusic(sMusic);
		}
	}
	
	if(m_piBTGeneralChangeColorMap==piControl)
	{
		std::string sColorMap="./Textures/";
		if(OpenFileDialog("Select layer texture...",".jpg;.jpeg;.bmp;.png",&sColorMap))
		{
			m_WorldManagerWrapper.m_piTerrain->SetTerrainColorMap(sColorMap);
			UpdateTexturization();	
		}
	}
	if(m_piBTShowOptionsPanel==piControl){m_bShowOptionsPanel=!m_bShowOptionsPanel;}
	if(m_piBTShowTerrainPanel==piControl){m_bShowTerrainPanel=!m_bShowTerrainPanel;m_bShowEntitiesPanel=false;m_bShowFormationsPanel=false;}
	if(m_piBTShowEntitiesPanel==piControl){m_bShowEntitiesPanel=!m_bShowEntitiesPanel;m_bShowTerrainPanel=false;m_bShowFormationsPanel=false;}
	if(m_piBTShowFormationsPanel==piControl){m_bShowFormationsPanel=!m_bShowFormationsPanel;m_bShowEntitiesPanel=false;m_bShowTerrainPanel=false;}
	if(m_piBTShowFilePanel==piControl){m_bShowFilePanel=!m_bShowFilePanel;}
	if(m_piBTShowGeneralProperties==piControl)
	{
		m_bShowGeneralPanel=true;
		m_bShowWaterPanel=false;
		m_bShowFogPanel=false;
		m_bShowSunPanel=false;
		m_bShowSkyPanel=false;
		m_bShowPlayAreaPanel=false;
		m_nSelectedColorLayer=-1;
		m_nSelectedHeightLayer=-1;
	}
	if(m_piBTShowWaterProperties==piControl)
	{
		m_bShowGeneralPanel=false;
		m_bShowWaterPanel=true;
		m_bShowFogPanel=false;
		m_bShowSunPanel=false;
		m_bShowSkyPanel=false;
		m_bShowPlayAreaPanel=false;
		m_nSelectedColorLayer=-1;
		m_nSelectedHeightLayer=-1;
	}
	if(m_piBTShowFogProperties==piControl)
	{
		m_bShowGeneralPanel=false;
		m_bShowWaterPanel=false;
		m_bShowFogPanel=true;
		m_bShowSunPanel=false;
		m_bShowSkyPanel=false;
		m_bShowPlayAreaPanel=false;
		m_nSelectedColorLayer=-1;
		m_nSelectedHeightLayer=-1;
	}
	if(m_piBTShowSunProperties==piControl)
	{
		m_bShowGeneralPanel=false;
		m_bShowWaterPanel=false;
		m_bShowFogPanel=false;
		m_bShowSunPanel=true;
		m_bShowSkyPanel=false;
		m_bShowPlayAreaPanel=false;
		m_nSelectedColorLayer=-1;
		m_nSelectedHeightLayer=-1;
	}
	if(m_piBTShowSkyProperties==piControl)
	{
		m_bShowGeneralPanel=false;
		m_bShowWaterPanel=false;
		m_bShowFogPanel=false;
		m_bShowSunPanel=false;
		m_bShowSkyPanel=true;
		m_bShowPlayAreaPanel=false;
		m_nSelectedColorLayer=-1;
		m_nSelectedHeightLayer=-1;
	}
	if(m_piBTShowPlayAreaProperties==piControl)
	{
		m_bShowGeneralPanel=false;
		m_bShowWaterPanel=false;
		m_bShowFogPanel=false;
		m_bShowSunPanel=false;
		m_bShowSkyPanel=false;
		m_bShowPlayAreaPanel=true;
		m_nSelectedColorLayer=-1;
		m_nSelectedHeightLayer=-1;
	}
	if(m_piBTOptionsBlend==piControl){m_bBlend=!m_bBlend;}
	if(m_piBTOptionsTextures==piControl){m_bTextures=!m_bTextures;}
	if(m_piBTOptionsSolid==piControl){m_bSolid=!m_bSolid;}
	if(m_piBTOptionsShadows==piControl){m_bShadows=!m_bShadows;}
	if(m_piBTOptionsLighting==piControl){m_bLighting=!m_bLighting;}
	if(m_piBTOptionsFog==piControl){m_bFog=!m_bFog;}
	if(m_piBTNewEntity==piControl)
	{
		unsigned long nSelectedEntityType=0;
		std::vector<IDesignObject *> vEntityTypes;
		std::vector<IDesignObject *> vFilteredEntities;
		GetSystemObjects("EntityTypes",&vEntityTypes);
		
		for(unsigned long x=0;x<vEntityTypes.size();x++)
		{
			SEntityTypeConfig sConfig;
			IDesignObject *piEntityType=vEntityTypes[x];
			IEntityTypeDesign *piDesign=QI(IEntityTypeDesign,piEntityType);
			piDesign->GetEntityTypeConfig(&sConfig);
			if(sConfig.nPlacement!=ENTITY_PLACEMENT_AIR)
			{
				vFilteredEntities.push_back(piEntityType);
			}
			REL(piDesign);
		}
		
		if(m_ObjectListSelector.m_piObjectListSelector->SelectObject("Select Entity",this,&vFilteredEntities,&nSelectedEntityType))
		{
			ISystemObject *piObject=QI(ISystemObject,vFilteredEntities[nSelectedEntityType]);
			IEntityType *piEntityType=QI(IEntityType,vFilteredEntities[nSelectedEntityType]);
			std::string sEntityType=piObject?piObject->GetName():"";

			unsigned long nObjectIndex=m_PlayAreaManagerWrapper.m_piPlayAreaDesign->AddElement("CPlayAreaEntity");
			IPlayAreaElement *piElement=NULL;
			m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetElement(nObjectIndex,&piElement);
			IPlayAreaEntity *piEntity=QI(IPlayAreaEntity,piElement);
			if(piEntity && piEntityType)
			{
				CVector vPos;
				GetTerrainCoordinatesFromCursorPos(m_rRealRect.w/2,m_rRealRect.h/2,false,&vPos);
				piEntity->SetPosition(vPos);
				piEntity->SetEntityType(piEntityType);
			}
			REL(piObject);
			REL(piEntity);
			REL(piElement);
			REL(piEntityType);
			UpdateEntityControls();
			m_nSelectedEntity=m_vEntityControls.size()-1;
			m_nSelectedEntityLayer=-1;	
			m_nSelectedFormation=-1;
			if(m_piLSEntityList){m_piLSEntityList->SetSelectedElement(m_nSelectedEntity);}
			if(m_piLSFormationList){m_piLSFormationList->SetSelectedElement(m_nSelectedFormation);}
		}
		for(unsigned long x=0;x<vEntityTypes.size();x++){IDesignObject *piEntityType=vEntityTypes[x];REL(piEntityType);}
	}
	if(m_piBTNewEntityLayer==piControl)
	{
		SEntityLayer layer;
		unsigned long nSelectedEntityType=0;
		std::vector<IDesignObject *> vEntityTypes;
		GetSystemObjects("EntityTypes",&vEntityTypes);
		if(m_ObjectListSelector.m_piObjectListSelector->SelectObject("Select Entity",this,&vEntityTypes,&nSelectedEntityType))
		{
			ISystemObject *piObject=QI(ISystemObject,vEntityTypes[nSelectedEntityType]);
			layer.sEntityType=piObject?piObject->GetName():"";
			REL(piObject);

			m_PlayAreaManagerWrapper.m_piPlayAreaDesign->AddEntityLayer(&layer);
			UpdateEntityLayerControls();
			UpdateTexturization();	
			m_nSelectedEntityLayer=m_vEntityLayerControls.size()-1;
			m_nSelectedEntity=-1;	
			m_nSelectedFormation=-1;
			if(m_piLSEntityList){m_piLSEntityList->SetSelectedElement(m_nSelectedEntity);}
			if(m_piLSFormationList){m_piLSFormationList->SetSelectedElement(m_nSelectedFormation);}
		}
		for(unsigned long x=0;x<vEntityTypes.size();x++){IDesignObject *piEntityType=vEntityTypes[x];REL(piEntityType);}

	}
	if(m_piBTNewFormation==piControl)
	{
		unsigned long nSelectedFormationType=0;
		std::vector<IDesignObject *> vFormationTypes;
		GetSystemObjects("FormationTypes",&vFormationTypes);
		if(m_ObjectSelector.m_piObjectSelector->SelectObject(this,&vFormationTypes,&nSelectedFormationType,64.0,64.0))
		{
			ISystemObject *piObject=QI(ISystemObject,vFormationTypes[nSelectedFormationType]);
			IFormationType *piFormationType=QI(IFormationType,vFormationTypes[nSelectedFormationType]);
			std::string sFormationType=piObject?piObject->GetName():"";

			unsigned long nObjectIndex=m_PlayAreaManagerWrapper.m_piPlayAreaDesign->AddElement("CPlayAreaFormation");
			IPlayAreaElement *piElement=NULL;
			m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetElement(nObjectIndex,&piElement);
			IPlayAreaFormation *piFormation=QI(IPlayAreaFormation,piElement);
			if(piFormation && piFormationType)
			{
				CVector vPos;
				GetAirPlaneCoordinatesFromCursorPos(m_rRealRect.w/2,m_rRealRect.h/2,&vPos);
				piFormation->SetPosition(vPos);
				piFormation->SetFormationType(piFormationType);
			}
			REL(piObject);
			REL(piFormation);
			REL(piElement);
			REL(piFormationType);
			UpdateFormationControls();
			m_nSelectedFormation=m_vFormationControls.size()-1;
			m_nSelectedEntity=-1;
			m_nSelectedEntityLayer=-1;
			if(m_piLSEntityList){m_piLSEntityList->SetSelectedElement(m_nSelectedEntity);}
			if(m_piLSFormationList){m_piLSFormationList->SetSelectedElement(m_nSelectedFormation);}
		}
		for(unsigned long x=0;x<vFormationTypes.size();x++){IDesignObject *piFormationType=vFormationTypes[x];REL(piFormationType);}
	}
	if(m_piBTNewColorLayer==piControl)
	{
		STerrainColorLayer layer;
		layer.dHorizontalResolution=1;
		layer.dVerticalResolution=1;
		std::string sTexture="./Textures/";
		if(OpenFileDialog("Select layer texture...",".jpg;.jpeg;.bmp;.png",&sTexture))
		{
			layer.sTextureFile=sTexture;
			m_WorldManagerWrapper.m_piTerrain->AddTerrainColorLayer(&layer);
			UpdateColorLayerControls();
			UpdateTexturization();	
			m_nSelectedColorLayer=m_vColorLayerControls.size()-1;
			m_nSelectedHeightLayer=-1;
			m_bShowGeneralPanel=false;
			m_bShowWaterPanel=false;
			m_bShowFogPanel=false;
			m_bShowSunPanel=false;
			m_bShowSkyPanel=false;
			m_bShowPlayAreaPanel=false;
		}
	}
	if(m_piBTNewHeightLayer==piControl)
	{
		STerrainHeightLayer layer;
		layer.dHorizontalResolution=1;
		layer.dVerticalResolution=1;
		if(m_vHeightLayerControls.size())
		{
			layer.dMinHeight=1.0;
			layer.dMaxHeight=1.0;
		}
		else
		{
			layer.dMinHeight=0;
			layer.dMaxHeight=1.0;
		}
		std::string sTexture="./Textures/";
		if(OpenFileDialog("Select layer texture...",".jpg;.jpeg;.bmp;.png",&sTexture))
		{
			layer.sTextureFile=sTexture;
			m_WorldManagerWrapper.m_piTerrain->AddTerrainHeightLayer(&layer);
			UpdateHeightLayerControls();
			UpdateTexturization();	
			m_nSelectedColorLayer=-1;
			m_nSelectedHeightLayer=m_vHeightLayerControls.size()-1;
			m_bShowGeneralPanel=false;
			m_bShowWaterPanel=false;
			m_bShowFogPanel=false;
			m_bShowSunPanel=false;
			m_bShowSkyPanel=false;
			m_bShowPlayAreaPanel=false;
		}
	}

	for(unsigned int x=0;x<m_vHeightLayerControls.size();x++)
	{
		if(m_vHeightLayerControls[x]->m_BTListRow.m_piButton==piControl)
		{
			m_nSelectedHeightLayer=x;
			m_nSelectedColorLayer=-1;
			m_bShowGeneralPanel=false;
			m_bShowWaterPanel=false;
			m_bShowFogPanel=false;
			m_bShowSunPanel=false;
			m_bShowSkyPanel=false;
			m_bShowPlayAreaPanel=false;
		}
	}
	for(unsigned int x=0;x<m_vColorLayerControls.size();x++)
	{
		if(m_vColorLayerControls[x]->m_BTListRow.m_piButton==piControl)
		{
			m_nSelectedHeightLayer=-1;
			m_nSelectedColorLayer=x;
			m_bShowGeneralPanel=false;
			m_bShowWaterPanel=false;
			m_bShowFogPanel=false;
			m_bShowSunPanel=false;
			m_bShowSkyPanel=false;
			m_bShowPlayAreaPanel=false;
		}
	}
	for(unsigned int x=0;x<m_vEntityLayerControls.size();x++)
	{
		if(m_vEntityLayerControls[x]->m_BTListRow.m_piButton==piControl)
		{
			m_nSelectedEntityLayer=x;
			m_nSelectedEntity=-1;
			m_nSelectedFormation=-1;
			if(m_piLSEntityList){m_piLSEntityList->SetSelectedElement(m_nSelectedEntity);}
			if(m_piLSFormationList){m_piLSFormationList->SetSelectedElement(m_nSelectedFormation);}
		}
	}
	SHeightLayerControls *pHeightLayer=NULL;
	SColorLayerControls  *pColorLayer=NULL;
	SEntityLayerControls *pEntityLayer=NULL;
	SEntityControls		 *pEntity=NULL;
	SFormationControls		 *pFormation=NULL;

	if(m_nSelectedHeightLayer!=-1){pHeightLayer=m_vHeightLayerControls[m_nSelectedHeightLayer];}
	if(m_nSelectedColorLayer!=-1){pColorLayer=m_vColorLayerControls[m_nSelectedColorLayer];}
	if(m_nSelectedEntityLayer!=-1){pEntityLayer=m_vEntityLayerControls[m_nSelectedEntityLayer];}
	if(m_nSelectedEntity!=-1){pEntity=m_vEntityControls[m_nSelectedEntity];}
	if(m_nSelectedFormation!=-1){pFormation=m_vFormationControls[m_nSelectedFormation];}

	if(pHeightLayer)
	{
		bool bChange=false;
		STerrainHeightLayer heightLayer;
		m_WorldManagerWrapper.m_piTerrain->GetTerrainHeightLayer(m_nSelectedHeightLayer,&heightLayer,NULL,NULL);

		if(piControl==m_piBTHeightLayerRemove)
		{
			m_WorldManagerWrapper.m_piTerrain->RemoveTerrainHeightLayer(m_nSelectedHeightLayer);
			UpdateHeightLayerControls();
			UpdateTexturization();
		}
		else if(piControl==m_piBTHeightLayerMoveUp)
		{
			if(m_nSelectedHeightLayer<(int)(m_vHeightLayerControls.size()-1))
			{
				m_WorldManagerWrapper.m_piTerrain->MoveTerrainHeightLayer(m_nSelectedHeightLayer,true);
				m_nSelectedHeightLayer++;
				UpdateHeightLayerControls();
				UpdateTexturization();
			}
		}
		else if(piControl==m_piBTHeightLayerMoveDown)
		{
			if(m_nSelectedHeightLayer>0)
			{
				m_WorldManagerWrapper.m_piTerrain->MoveTerrainHeightLayer(m_nSelectedHeightLayer,false);
				m_nSelectedHeightLayer--;
				UpdateHeightLayerControls();
				UpdateTexturization();
			}
		} 
		else if(piControl==m_piBTHeightLayerSample)
		{
			std::string sTexture="./Textures/";
			if(OpenFileDialog("Open texture...",".jpg;.jpeg;.bmp;.png",&sTexture))
			{
				heightLayer.sTextureFile=sTexture;
				bChange=true;
			}
		}
		else if(piControl==m_piBTHeightLayerNormalMapSample)
		{
			std::string sNormalMap="./Textures/";
			if(OpenFileDialog("Open normal map...",".jpg;.jpeg;.bmp;.png",&sNormalMap))
			{
				heightLayer.sNormalMapFile=sNormalMap;
				bChange=true;
			}
		}
		else if(piControl==m_piBTHeightLayerIncreaseHorzResolution)
		{
			heightLayer.dHorizontalResolution+=0.5;
			bChange=true;
		}
		else if(piControl==m_piBTHeightLayerDecreaseHorzResolution)
		{
			heightLayer.dHorizontalResolution-=0.5;
			bChange=true;
		}
		else if(piControl==m_piBTHeightLayerIncreaseVertResolution)
		{
			heightLayer.dVerticalResolution+=0.5;
			bChange=true;
		}
		else if(piControl==m_piBTHeightLayerDecreaseVertResolution)
		{
			heightLayer.dVerticalResolution-=0.5;
			bChange=true;
		}	
		else if(piControl==m_piBTHeightLayerIncreaseMaxHeight)
		{
			heightLayer.dMaxHeight+=0.02;
			if(heightLayer.dMaxHeight>1){heightLayer.dMaxHeight=1;}
			bChange=true;
		}
		else if(piControl==m_piBTHeightLayerDecreaseMaxHeight)
		{
			heightLayer.dMaxHeight-=0.02;
			// Limitamos el height de 0 a 1 y nos aseguramos de que elmin y max de esta capa no solapan
			if(heightLayer.dMaxHeight<0){heightLayer.dMaxHeight=0;}
			if(heightLayer.dMinHeight>heightLayer.dMaxHeight){heightLayer.dMinHeight=heightLayer.dMaxHeight;}
			bChange=true;
		}
		else if(piControl==m_piBTHeightLayerIncreaseMinHeight)
		{
			heightLayer.dMinHeight+=0.02;
			// Limitamos el height de 0 a 1 y nos aseguramos de que elmin y max de esta capa no solapan
			if(heightLayer.dMinHeight>1){heightLayer.dMinHeight=1;}
			if(heightLayer.dMinHeight>heightLayer.dMaxHeight){heightLayer.dMaxHeight=heightLayer.dMinHeight;}
			bChange=true;
		}
		else if(piControl==m_piBTHeightLayerDecreaseMinHeight)
		{
			heightLayer.dMinHeight-=0.02;
			// Limitamos el height de 0 a 1
			if(heightLayer.dMinHeight<0){heightLayer.dMinHeight=0;}
			bChange=true;
		}
		else if(piControl==m_piBTHeightLayerIncreaseMargin)
		{
			heightLayer.dDecayMargin+=0.01;
			if(heightLayer.dDecayMargin>1){heightLayer.dDecayMargin=1;}
			bChange=true;
		}
		else if(piControl==m_piBTHeightLayerDecreaseMargin)
		{
			heightLayer.dDecayMargin-=0.01;
			if(heightLayer.dDecayMargin<0){heightLayer.dDecayMargin=0;}
			bChange=true;
		}
		if(bChange)
		{
			m_WorldManagerWrapper.m_piTerrain->UpdateTerrainHeightLayer(m_nSelectedHeightLayer,&heightLayer);
			UpdateTexturization();
		}
	}
	
	if(pColorLayer)
	{
		bool bChange=false;
		STerrainColorLayer colorLayer;
		m_WorldManagerWrapper.m_piTerrain->GetTerrainColorLayer(m_nSelectedColorLayer,&colorLayer,NULL,NULL);

		if(piControl==m_piBTColorLayerRemove)
		{
			m_WorldManagerWrapper.m_piTerrain->RemoveTerrainColorLayer(m_nSelectedColorLayer);
			UpdateColorLayerControls();
			UpdateTexturization();
		}
		else if(piControl==m_piBTColorLayerMoveUp)
		{
			if(m_nSelectedColorLayer<(int)(m_vColorLayerControls.size()-1))
			{
				m_WorldManagerWrapper.m_piTerrain->MoveTerrainColorLayer(m_nSelectedColorLayer,true);
				m_nSelectedColorLayer++;
				UpdateColorLayerControls();
				UpdateTexturization();
			}
		}
		else if(piControl==m_piBTColorLayerMoveDown)
		{
			if(m_nSelectedColorLayer>0)
			{
				m_WorldManagerWrapper.m_piTerrain->MoveTerrainColorLayer(m_nSelectedColorLayer,false);
				m_nSelectedColorLayer--;
				UpdateColorLayerControls();
				UpdateTexturization();
			}
		} 
		else if(piControl==m_piBTColorLayerSample)
		{
			std::string sTexture="./Textures/";
			if(OpenFileDialog("Open texture...",".jpg;.jpeg;.bmp;.png",&sTexture))
			{
				colorLayer.sTextureFile=sTexture;
				bChange=true;
			}
		}
		else if(piControl==m_piBTColorLayerNormalMapSample)
		{
			std::string sNormalMap="./Textures/";
			if(OpenFileDialog("Open normal map...",".jpg;.jpeg;.bmp;.png",&sNormalMap))
			{
				colorLayer.sNormalMapFile=sNormalMap;
				bChange=true;
			}
		}
		else if(piControl==m_piBTColorLayerAbruptEnd)
		{
			colorLayer.bAbruptEnd=!colorLayer.bAbruptEnd;
			bChange=true;
		}
		else if(piControl==m_piBTColorLayerColorSample)
		{
			IGenericTexture *piTexture=NULL;
			m_WorldManagerWrapper.m_piTerrain->GetTerrainColorMap(NULL,&piTexture);
			if(m_ColorSelector.m_piColorSelector->SelectColor(this,piTexture,&colorLayer.vColor))
			{
				bChange=true;
			}
			REL(piTexture);
		}
		else if(piControl==m_piBTColorLayerIncreaseHorzResolution)
		{
			colorLayer.dHorizontalResolution+=0.5;
			bChange=true;
		}
		else if(piControl==m_piBTColorLayerDecreaseHorzResolution)
		{
			colorLayer.dHorizontalResolution-=0.5;
			bChange=true;
		}
		else if(piControl==m_piBTColorLayerIncreaseVertResolution)
		{
			colorLayer.dVerticalResolution+=0.5;
			bChange=true;
		}
		else if(piControl==m_piBTColorLayerDecreaseVertResolution)
		{
			colorLayer.dVerticalResolution-=0.5;
			bChange=true;
		}	
		if(bChange)
		{
			m_WorldManagerWrapper.m_piTerrain->UpdateTerrainColorLayer(m_nSelectedColorLayer,&colorLayer);
			UpdateTexturization();
		}
	}
	if(pEntityLayer)
	{
		bool bChange=false;
		SEntityLayer entityLayer;
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetEntityLayer(m_nSelectedEntityLayer,&entityLayer,NULL);

		if(piControl==m_piBTEntityLayerRemove)
		{
			m_PlayAreaManagerWrapper.m_piPlayAreaDesign->RemoveEntityLayer(m_nSelectedEntityLayer);
			UpdateEntityLayerControls();
			UpdateTexturization();
		}
		else if(piControl==m_piBTEntityLayerSample)
		{
			unsigned long nSelectedEntityType=0;
			std::vector<IDesignObject *> vEntityTypes;
			GetSystemObjects("EntityTypes",&vEntityTypes);
			if(m_ObjectListSelector.m_piObjectListSelector->SelectObject("Select Entity",this,&vEntityTypes,&nSelectedEntityType))
			{
				ISystemObject *piObject=QI(ISystemObject,vEntityTypes[nSelectedEntityType]);
				entityLayer.sEntityType=piObject?piObject->GetName():"";
				REL(piObject);
				bChange=true;
			}
			for(unsigned long x=0;x<vEntityTypes.size();x++){IDesignObject *piEntityType=vEntityTypes[x];REL(piEntityType);}
		}
		else if(piControl==m_piBTEntityLayerColorSample)
		{
			IGenericTexture *piTexture=NULL;
			m_WorldManagerWrapper.m_piTerrain->GetTerrainColorMap(NULL,&piTexture);
			if(m_ColorSelector.m_piColorSelector->SelectColor(this,piTexture,&entityLayer.vColor))
			{
				bChange=true;
			}
			REL(piTexture);
		}
		else if(piControl==m_piBTEntityLayerIncreaseSeparation)
		{
			entityLayer.dSeparation+=0.05;
			bChange=true;
		}
		else if(piControl==m_piBTEntityLayerDecreaseSeparation)
		{
			entityLayer.dSeparation-=0.05;
			if(entityLayer.dSeparation<0.05){entityLayer.dSeparation=0.05;}
			bChange=true;
		}
		else if(piControl==m_piBTEntityLayerIncreasePositionJitter)
		{
			entityLayer.dPositionJitter+=0.05;
			bChange=true;
		}
		else if(piControl==m_piBTEntityLayerDecreasePositionJitter)
		{
			entityLayer.dPositionJitter-=0.05;
			if(entityLayer.dPositionJitter<0){entityLayer.dPositionJitter=0;}
			bChange=true;
		}
		else if(piControl==m_piBTEntityLayerIncreaseYawJitter)
		{
			entityLayer.dYawJitter+=15.0;
			bChange=true;
		}
		else if(piControl==m_piBTEntityLayerDecreaseYawJitter)
		{
			entityLayer.dYawJitter-=15.0;
			if(entityLayer.dYawJitter<0){entityLayer.dYawJitter=360+entityLayer.dYawJitter;}
			bChange=true;
		}
		if(bChange)
		{
			m_PlayAreaManagerWrapper.m_piPlayAreaDesign->UpdateEntityLayer(m_nSelectedEntityLayer,&entityLayer);
			UpdateTexturization();
		}
	}
	if(pEntity)
	{
		if(piControl==m_piBTEntityRemove)
		{
			m_PlayAreaManagerWrapper.m_piPlayAreaDesign->RemoveElement(m_vEntityControls[m_nSelectedEntity]->m_nPlayAreaElementId);
			UpdateEntityControls();
		}
		else if(piControl==m_piBTEntitySample)
		{
			unsigned long nSelectedEntityType=0;
			std::vector<IDesignObject *> vEntityTypes;
			GetSystemObjects("EntityTypes",&vEntityTypes);
			if(m_ObjectListSelector.m_piObjectListSelector->SelectObject("Select Entity",this,&vEntityTypes,&nSelectedEntityType))
			{
				IEntityType *piEntityType=QI(IEntityType,vEntityTypes[nSelectedEntityType]);
				pEntity->m_piPlayAreaEntity->SetEntityType(piEntityType);
				REL(piEntityType);

				UpdateEntityControls();
			}
			for(unsigned long x=0;x<vEntityTypes.size();x++){IDesignObject *piEntityType=vEntityTypes[x];REL(piEntityType);}
		}
		else if(piControl==m_piBTEntityBonusSample)
		{
			unsigned long nSelectedEntityType=0;
			std::vector<IDesignObject *> vEntityTypes;
			GetSystemObjects("EntityTypes",&vEntityTypes);
			if(m_ObjectListSelector.m_piObjectListSelector->SelectObject("Select Bonus",this,&vEntityTypes,&nSelectedEntityType))
			{
				IEntityType *piEntityType=QI(IEntityType,vEntityTypes[nSelectedEntityType]);
				pEntity->m_piPlayAreaEntity->SetBonusType(piEntityType);
				REL(piEntityType);
				
				UpdateEntityControls();
			}
			for(unsigned long x=0;x<vEntityTypes.size();x++){IDesignObject *piEntityType=vEntityTypes[x];REL(piEntityType);}
		}		
		else if(piControl==m_piBTEntityIncreaseYaw)
		{
			CVector vAngles=pEntity->m_piPlayAreaEntity->GetAngles();
			vAngles.c[YAW]+=15.0;
			if(vAngles.c[YAW]>=360){vAngles.c[YAW]=vAngles.c[YAW]-360;}
			pEntity->m_piPlayAreaEntity->SetAngles(vAngles);
		}
		else if(piControl==m_piBTEntityDecreaseYaw)
		{
			CVector vAngles=pEntity->m_piPlayAreaEntity->GetAngles();
			vAngles.c[YAW]-=15.0;
			if(vAngles.c[YAW]<0){vAngles.c[YAW]=360+vAngles.c[YAW];}
			pEntity->m_piPlayAreaEntity->SetAngles(vAngles);
		}
		else if(piControl==m_piBTEntityClearRoute)
		{
			pEntity->m_piPlayAreaEntity->ClearRoute();
			m_nSelectedRoutePoint=-1;
		}
		else if(piControl==m_piBTEntityIncreaseCount)
		{
			int nCount=pEntity->m_piPlayAreaEntity->GetCount();
			pEntity->m_piPlayAreaEntity->SetCount(nCount+1);
		}
		else if(piControl==m_piBTEntityDecreaseCount)
		{
			int nCount=pEntity->m_piPlayAreaEntity->GetCount();
			if(nCount>1){pEntity->m_piPlayAreaEntity->SetCount(nCount-1);}
		}
		else if(piControl==m_piBTEntityIncreaseDelay)
		{
			int nIncrement=100;
			if(m_piGUIManager->IsKeyDown(GK_LCONTROL)){nIncrement=1000;}
			if(m_piGUIManager->IsKeyDown(GK_LSHIFT)){nIncrement=5000;}
			int nDelay=pEntity->m_piPlayAreaEntity->GetDelay();
			pEntity->m_piPlayAreaEntity->SetDelay(nDelay+nIncrement);
		}
		else if(piControl==m_piBTEntityDecreaseDelay)
		{
			int nIncrement=100;
			if(m_piGUIManager->IsKeyDown(GK_LCONTROL)){nIncrement=1000;}
			if(m_piGUIManager->IsKeyDown(GK_LSHIFT)){nIncrement=5000;}
			int nDelay=pEntity->m_piPlayAreaEntity->GetDelay();
			pEntity->m_piPlayAreaEntity->SetDelay(nDelay-nIncrement);
		}
		else if(piControl==m_piBTEntityIncreaseBonusOnChild)
		{
			int nBonusOnChild=pEntity->m_piPlayAreaEntity->GetBonusOnChild();
			pEntity->m_piPlayAreaEntity->SetBonusOnChild(nBonusOnChild+1);
		}
		else if(piControl==m_piBTEntityDecreaseBonusOnChild)
		{
			int nBonusOnChild=pEntity->m_piPlayAreaEntity->GetBonusOnChild();
			if(nBonusOnChild>=0){pEntity->m_piPlayAreaEntity->SetBonusOnChild(nBonusOnChild-1);}
		}
		else if(piControl==m_piBTEntityIncreaseRouteDelay)
		{
			int nIncrement=100;
			if(m_piGUIManager->IsKeyDown(GK_LCONTROL)){nIncrement=1000;}
			if(m_piGUIManager->IsKeyDown(GK_LSHIFT)){nIncrement=5000;}
			int nRouteDelay=pEntity->m_piPlayAreaEntity->GetRouteDelay();
			pEntity->m_piPlayAreaEntity->SetRouteDelay(nRouteDelay+nIncrement);
		}
		else if(piControl==m_piBTEntityDecreaseRouteDelay)
		{
			int nIncrement=100;
			if(m_piGUIManager->IsKeyDown(GK_LCONTROL)){nIncrement=1000;}
			if(m_piGUIManager->IsKeyDown(GK_LSHIFT)){nIncrement=5000;}
			int nRouteDelay=pEntity->m_piPlayAreaEntity->GetRouteDelay();
			pEntity->m_piPlayAreaEntity->SetRouteDelay(nRouteDelay>=nIncrement?nRouteDelay-nIncrement:0);
		}
		else if(piControl==m_piBTEntityIncreaseInterval)
		{
			int nIncrement=100;
			if(m_piGUIManager->IsKeyDown(GK_LCONTROL)){nIncrement=1000;}
			if(m_piGUIManager->IsKeyDown(GK_LSHIFT)){nIncrement=5000;}
			int nInterval=pEntity->m_piPlayAreaEntity->GetInterval();
			pEntity->m_piPlayAreaEntity->SetInterval(nInterval+nIncrement);
		}		
		else if(piControl==m_piBTEntityDecreaseInterval)
		{
			int nIncrement=100;
			if(m_piGUIManager->IsKeyDown(GK_LCONTROL)){nIncrement=1000;}
			if(m_piGUIManager->IsKeyDown(GK_LSHIFT)){nIncrement=5000;}
			int nInterval=pEntity->m_piPlayAreaEntity->GetInterval();
			pEntity->m_piPlayAreaEntity->SetInterval(nInterval>=nIncrement?nInterval-nIncrement:0);
		}
	}
	if(pFormation)
	{
		if(piControl==m_piBTFormationRemove)
		{
			m_PlayAreaManagerWrapper.m_piPlayAreaDesign->RemoveElement(m_vFormationControls[m_nSelectedFormation]->m_nPlayAreaElementId);
			UpdateFormationControls();
		}
		else if(piControl==m_piBTFormationSample)
		{
			SPlayAreaConfig sPlayAreaConfig;
			m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetPlayAreaConfig(&sPlayAreaConfig);
			
			unsigned long nSelectedFormationType=0;
			std::vector<IDesignObject *> vFormationTypes;
			GetSystemObjects("FormationTypes",&vFormationTypes);
			if(m_ObjectSelector.m_piObjectSelector->SelectObject(this,&vFormationTypes,&nSelectedFormationType,64.0,64.0))
			{
				IFormationType *piFormationType=QI(IFormationType,vFormationTypes[nSelectedFormationType]);
				pFormation->m_piPlayAreaFormation->SetFormationType(piFormationType);
				REL(piFormationType);

				UpdateFormationControls();
			}
			for(unsigned long x=0;x<vFormationTypes.size();x++){IDesignObject *piFormationType=vFormationTypes[x];REL(piFormationType);}
		}
		else if(piControl==m_piBTFormationBonusSample)
		{
			unsigned long nSelectedEntityType=0;
			std::vector<IDesignObject *> vEntityTypes;
			GetSystemObjects("EntityTypes",&vEntityTypes);
			if(m_ObjectListSelector.m_piObjectListSelector->SelectObject("Select Bonus",this,&vEntityTypes,&nSelectedEntityType))
			{
				IEntityType *piEntityType=QI(IEntityType,vEntityTypes[nSelectedEntityType]);
				pFormation->m_piPlayAreaFormation->SetBonusType(piEntityType);
				REL(piEntityType);

				UpdateFormationControls();
			}
			for(unsigned long x=0;x<vEntityTypes.size();x++){IDesignObject *piEntityType=vEntityTypes[x];REL(piEntityType);}
		}
		else if(piControl==m_piBTFormationAlternativeSample)
		{
			unsigned long nSelectedFormationType=0;
			std::vector<IDesignObject *> vFormationTypes;
			GetSystemObjects("FormationTypes",&vFormationTypes);
			if(m_ObjectSelector.m_piObjectSelector->SelectObject(this,&vFormationTypes,&nSelectedFormationType,64.0,64.0))
			{
				IFormationType *piFormationType=QI(IFormationType,vFormationTypes[nSelectedFormationType]);
				pFormation->m_piPlayAreaFormation->SetAlternativeFormationType(piFormationType);
				REL(piFormationType);

				UpdateFormationControls();
			}
			for(unsigned long x=0;x<vFormationTypes.size();x++){IDesignObject *piFormationType=vFormationTypes[x];REL(piFormationType);}
		}
		else if(piControl==m_piBTFormationAlternativeBonusSample)
		{
			unsigned long nSelectedEntityType=0;
			std::vector<IDesignObject *> vEntityTypes;
			GetSystemObjects("EntityTypes",&vEntityTypes);
			if(m_ObjectListSelector.m_piObjectListSelector->SelectObject("Select Bonus",this,&vEntityTypes,&nSelectedEntityType))
			{
				IEntityType *piEntityType=QI(IEntityType,vEntityTypes[nSelectedEntityType]);
				pFormation->m_piPlayAreaFormation->SetAlternativeBonusType(piEntityType);
				REL(piEntityType);

				UpdateFormationControls();
			}
			for(unsigned long x=0;x<vEntityTypes.size();x++){IDesignObject *piEntityType=vEntityTypes[x];REL(piEntityType);}
		}
		else if(m_piBTFormationIncreaseCondition==piControl)
		{
			unsigned int nValue=0;
			pFormation->m_piPlayAreaFormation->GetCondition(NULL,&nValue);
			nValue++;
			pFormation->m_piPlayAreaFormation->SetCondition(nValue?ePlayerStateCondition_MaxPrimaryWeaponLevel:ePlayerStateCondition_None,nValue);
		}
		else if(m_piBTFormationDecreaseCondition==piControl)
		{
			unsigned int nValue=0;
			pFormation->m_piPlayAreaFormation->GetCondition(NULL,&nValue);
			if(nValue>0){nValue--;}
			pFormation->m_piPlayAreaFormation->SetCondition(nValue?ePlayerStateCondition_MaxPrimaryWeaponLevel:ePlayerStateCondition_None,nValue);
		}

	}
	bool bWaterChanged=false;
	STerrainWater sWater;
	m_WorldManagerWrapper.m_piTerrain->GetTerrainWater(&sWater,NULL,NULL);

	if(piControl==m_piBTWaterEnable)
	{
		sWater.bEnabled=!sWater.bEnabled;
		bWaterChanged=true;
	}
	else if(piControl==m_piBTWaterAuto)
	{
		bWaterChanged=true;

		IGenericModel *piModel=NULL;
		if(m_WorldManagerWrapper.m_piTerrain){m_WorldManagerWrapper.m_piTerrain->GetTerrainBaseModel(NULL,&piModel);}
		if(piModel)
		{
			CVector vBaseModelMins,vBaseModelMaxs;
			CVector vModelSize=piModel->GetFrameSize(0,0);
			piModel->GetFrameBBox(0,0,&vBaseModelMins,&vBaseModelMaxs);
		
			CVector vWaterMins,vWaterMaxs;
			
			for(unsigned int nBuffer=0;nBuffer<piModel->GetFrameRenderBuffers(0,0);nBuffer++)
			{
				unsigned long nBaseModelVertexes=0;
				float        *pBaseModelFaceCursor=NULL;
				
				piModel->GetRenderBufferVertexes(0,0,nBuffer,&nBaseModelVertexes,&pBaseModelFaceCursor);
				
				bool bFirstVertex=true;
				for(unsigned int nVertex=0;nVertex<nBaseModelVertexes;nVertex++)
				{
					CVector vVertex(pBaseModelFaceCursor[0],pBaseModelFaceCursor[1],pBaseModelFaceCursor[2]);
					if(vVertex.c[1]<=sWater.vMaxs.c[1])
					{
						if(bFirstVertex || vVertex.c[0]<vWaterMins.c[0]){vWaterMins.c[0]=vVertex.c[0];}
						if(bFirstVertex || vVertex.c[0]>vWaterMaxs.c[0]){vWaterMaxs.c[0]=vVertex.c[0];}
						if(bFirstVertex || vVertex.c[2]<vWaterMins.c[2]){vWaterMins.c[2]=vVertex.c[2];}
						if(bFirstVertex || vVertex.c[2]>vWaterMaxs.c[2]){vWaterMaxs.c[2]=vVertex.c[2];}
						bFirstVertex=false;
					}
					pBaseModelFaceCursor+=3;
				}
			}
			sWater.vMins.c[0]=vWaterMins.c[0];
			sWater.vMins.c[2]=vWaterMins.c[2];
			sWater.vMaxs.c[0]=vWaterMaxs.c[0];
			sWater.vMaxs.c[2]=vWaterMaxs.c[2];
		}
		REL(piModel);
	}
	else if(piControl==m_piBTWaterSample)
	{
		std::string sTexture="./Textures/";
		if(OpenFileDialog("Open texture...",".jpg;.jpeg;.bmp;.png",&sTexture))
		{
			sWater.sTextureFile1=sTexture;
			bWaterChanged=true;
		}
	}
	else if(piControl==m_piBTWaterSecondSample)
	{
		std::string sTexture="./Textures/";
		if(OpenFileDialog("Open texture...",".jpg;.jpeg;.bmp;.png",&sTexture))
		{
			sWater.sTextureFile2=sTexture;
			bWaterChanged=true;
		}
	}	
	else if(piControl==m_piBTWaterIncreaseHorzResolution)
	{
		sWater.dHorizontalResolution+=0.5;
		bWaterChanged=true;
	}
	else if(piControl==m_piBTWaterDecreaseHorzResolution)
	{
		sWater.dHorizontalResolution-=0.5;
		bWaterChanged=true;
	}
	else if(piControl==m_piBTWaterIncreaseVertResolution)
	{
		sWater.dVerticalResolution+=0.5;
		bWaterChanged=true;
	}
	else if(piControl==m_piBTWaterDecreaseVertResolution)
	{
		sWater.dVerticalResolution-=0.5;
		bWaterChanged=true;
	}
	else if(piControl==m_piBTWaterIncreaseHeight)
	{
		CVector vBaseModelMins,vBaseModelMaxs;
		IGenericModel *piModel=NULL;
		if(m_WorldManagerWrapper.m_piTerrain){m_WorldManagerWrapper.m_piTerrain->GetTerrainBaseModel(NULL,&piModel);}
		if(piModel){piModel->GetFrameBBox(0,0,&vBaseModelMins,&vBaseModelMaxs);}
		REL(piModel);
			
		sWater.vMaxs.c[1]+=(vBaseModelMaxs.c[1]-vBaseModelMins.c[1])*0.02;
		sWater.vMins.c[1]=vBaseModelMins.c[1];
		if(sWater.vMaxs.c[1]>vBaseModelMaxs.c[1]){sWater.vMaxs.c[1]=vBaseModelMaxs.c[1];}
		if(sWater.vMaxs.c[1]<vBaseModelMins.c[1]){sWater.vMaxs.c[1]=vBaseModelMins.c[1];}
		bWaterChanged=true;
	}
	else if(piControl==m_piBTWaterDecreaseHeight)
	{
		CVector vBaseModelMins,vBaseModelMaxs;
		IGenericModel *piModel=NULL;
		if(m_WorldManagerWrapper.m_piTerrain){m_WorldManagerWrapper.m_piTerrain->GetTerrainBaseModel(NULL,&piModel);}
		if(piModel){piModel->GetFrameBBox(0,0,&vBaseModelMins,&vBaseModelMaxs);}
		REL(piModel);
		
		sWater.vMaxs.c[1]-=(vBaseModelMaxs.c[1]-vBaseModelMins.c[1])*0.02;
		sWater.vMins.c[1]=vBaseModelMins.c[1];
		if(sWater.vMaxs.c[1]>vBaseModelMaxs.c[1]){sWater.vMaxs.c[1]=vBaseModelMaxs.c[1];}
		if(sWater.vMaxs.c[1]<vBaseModelMins.c[1]){sWater.vMaxs.c[1]=vBaseModelMins.c[1];}
		bWaterChanged=true;
	}
	else if(piControl==m_piBTWaterIncreaseOpacity)
	{
		sWater.dOpacity+=0.05;
		if(sWater.dOpacity>1){sWater.dOpacity=1;}
		bWaterChanged=true;
	}
	else if(piControl==m_piBTWaterDecreaseOpacity)
	{
		sWater.dOpacity-=0.05;
		if(sWater.dOpacity<0){sWater.dOpacity=0;}
		bWaterChanged=true;
	}
	else if(piControl==m_piBTWaterIncreaseSpeed)
	{
		sWater.dSpeed+=0.005;
		bWaterChanged=true;
	}
	else if(piControl==m_piBTWaterDecreaseSpeed)
	{
		sWater.dSpeed-=0.005;
		bWaterChanged=true;
	}
	if(bWaterChanged)
	{
		m_WorldManagerWrapper.m_piTerrain->SetTerrainWater(&sWater);
		UpdateTexturization();
	}

	bool bSkyChanged=false;
	STerrainSky sSky;
	m_WorldManagerWrapper.m_piTerrain->GetTerrainSky(&sSky,NULL);

	if(piControl==m_piBTSkyEnable)
	{
		sSky.bEnabled=!sSky.bEnabled;
		bSkyChanged=true;
	}
	else if(piControl==m_piBTSkySample)
	{
		std::string sTexture="./Textures/";
		if(OpenFileDialog("Open texture...",".jpg;.jpeg;.bmp;.png",&sTexture))
		{
			sSky.sTextureFile=sTexture;
			bSkyChanged=true;
		}
	}
	else if(piControl==m_piBTSkyIncreaseHorzResolution)
	{
		sSky.dHorizontalResolution+=0.5;
		bSkyChanged=true;
	}
	else if(piControl==m_piBTSkyDecreaseHorzResolution)
	{
		sSky.dHorizontalResolution-=0.5;
		bSkyChanged=true;
	}
	else if(piControl==m_piBTSkyIncreaseVertResolution)
	{
		sSky.dVerticalResolution+=0.5;
		bSkyChanged=true;
	}
	else if(piControl==m_piBTSkyDecreaseVertResolution)
	{
		sSky.dVerticalResolution-=0.5;
		bSkyChanged=true;
	}
	else if(piControl==m_piBTSkyIncreaseOpacity)
	{
		sSky.dOpacity+=0.05;
		if(sSky.dOpacity>1){sSky.dOpacity=1;}
		bSkyChanged=true;
	}
	else if(piControl==m_piBTSkyDecreaseOpacity)
	{
		sSky.dOpacity-=0.05;
		if(sSky.dOpacity<0){sSky.dOpacity=0;}
		bSkyChanged=true;
	}
	else if(piControl==m_piBTSkyIncreaseSpeed)
	{
		sSky.dSpeed+=0.005;
		bSkyChanged=true;
	}
	else if(piControl==m_piBTSkyDecreaseSpeed)
	{
		sSky.dSpeed-=0.005;
		bSkyChanged=true;
	}
	if(bSkyChanged)
	{
		m_WorldManagerWrapper.m_piTerrain->SetTerrainSky(&sSky);
	}

	bool bFogChanged=false;
	STerrainFog sFog;
	m_WorldManagerWrapper.m_piTerrain->GetTerrainFog(&sFog);

	if(piControl==m_piBTFogEnable)
	{
		sFog.bEnabled=!sFog.bEnabled;
		bFogChanged=true;
	}
	if(piControl==m_piBTFogFitWater)
	{
		STerrainWater sWaterConfig;
		SPlayAreaConfig sPlayAreaConfig;
		CVector vMins,vMaxs;
		m_WorldManagerWrapper.m_piTerrain->GetTerrainBBox(&vMins,&vMaxs);
		m_WorldManagerWrapper.m_piTerrain->GetTerrainWater(&sWaterConfig,NULL,NULL);
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetPlayAreaConfig(&sPlayAreaConfig);

		sFog.vMins=sWaterConfig.vMins;
		sFog.vMaxs=sWaterConfig.vMaxs;
		sFog.vMins.c[1]=vMins.c[1];
		bFogChanged=true;
	}
	if(piControl==m_piBTFogFitSky)
	{
		STerrainWater sWaterConfig;
		SPlayAreaConfig sPlayAreaConfig;
		CVector vMins,vMaxs;
		m_WorldManagerWrapper.m_piTerrain->GetTerrainBBox(&vMins,&vMaxs);
		m_WorldManagerWrapper.m_piTerrain->GetTerrainWater(&sWaterConfig,NULL,NULL);
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetPlayAreaConfig(&sPlayAreaConfig);

		sFog.vMins=sWaterConfig.vMins;
		sFog.vMaxs=sWaterConfig.vMaxs;
		sFog.vMins.c[1]=sWaterConfig.vMaxs.c[1];
		sFog.vMaxs.c[1]=sWaterConfig.vMaxs.c[1]+sPlayAreaConfig.dCameraDistance+sPlayAreaConfig.dAirPlaneHeight;
		bFogChanged=true;
	}
	else if(piControl==m_piBTFogColorSample)
	{
		m_eColorCallBack=eColorCallBackFog;
		if(SelectColorDialog("Select fog color...",this,&sFog.vColor))
		{
			bFogChanged=true;
		}
	}
	else if(piControl==m_piBTFogIncreaseStart)
	{
		sFog.vMins.c[1]+=1;
		if(sFog.vMins.c[1]+1>sFog.vMaxs.c[1]){sFog.vMins.c[1]=sFog.vMaxs.c[1]-1;}
		bFogChanged=true;
	}
	else if(piControl==m_piBTFogDecreaseStart)
	{
		sFog.vMins.c[1]-=1;
		bFogChanged=true;
	}
	else if(piControl==m_piBTFogIncreaseEnd)
	{
		sFog.vMaxs.c[1]+=1;
		bFogChanged=true;
	}
	else if(piControl==m_piBTFogDecreaseEnd)
	{
		sFog.vMaxs.c[1]-=1;
		if(sFog.vMaxs.c[1]-1<sFog.vMins.c[1]){sFog.vMaxs.c[1]=sFog.vMins.c[1]+1;}
		bFogChanged=true;
	}
	if(bFogChanged)
	{
		m_WorldManagerWrapper.m_piTerrain->SetTerrainFog(&sFog);
	}

	bool bSunChanged=false;
	STerrainSun sSun;
	CVector vAmbientColor;
	m_WorldManagerWrapper.m_piTerrain->GetTerrainSun(&sSun);
	vAmbientColor=m_WorldManagerWrapper.m_piTerrain->GetTerrainAmbientColor();

	if(piControl==m_piBTSunColorSample)
	{
		m_eColorCallBack=eColorCallBackSun;
		if(SelectColorDialog("Select sun color...",this,&sSun.vColor))
		{
			bSunChanged=true;
		}
	}
	else if(piControl==m_piBTAmbientColorSample)
	{
		m_eColorCallBack=eColorCallBackAmbient;
		if(SelectColorDialog("Select ambient color...",this,&vAmbientColor))
		{
			m_WorldManagerWrapper.m_piTerrain->SetTerrainAmbientColor(vAmbientColor);
		}
	}	
	else if(piControl==m_piBTSunIncreaseDistance)
	{
		sSun.dDistance+=0.02;
		bSunChanged=true;
	}
	else if(piControl==m_piBTSunDecreaseDistance)
	{
		sSun.dDistance-=0.02;
		if(sSun.dDistance<0){sSun.dDistance=0;}
		bSunChanged=true;
	}
	else if(piControl==m_piBTSunIncreaseElevation)
	{
		sSun.dElevation+=5.0;
		if(sSun.dElevation>90.0){sSun.dElevation=90.0;}
		bSunChanged=true;
	}
	else if(piControl==m_piBTSunDecreaseElevation)
	{
		sSun.dElevation-=5.0;
		if(sSun.dElevation<10.0){sSun.dElevation=10.0;}
		bSunChanged=true;
	}
	else if(piControl==m_piBTSunIncreaseAzimuth)
	{
		sSun.dAzimuth+=15.0;
		if(sSun.dAzimuth>=360){sSun.dAzimuth=sSun.dAzimuth-360;}
		bSunChanged=true;
	}
	else if(piControl==m_piBTSunDecreaseAzimuth)
	{
		sSun.dAzimuth-=15.0;
		if(sSun.dAzimuth<0){sSun.dAzimuth=360+sSun.dAzimuth;}
		bSunChanged=true;
	}
	if(bSunChanged)
	{
		m_WorldManagerWrapper.m_piTerrain->SetTerrainSun(&sSun);
	}
	bool bUpdateFormations=true;
	bool bPlayAreaChange=false;
	bool bPlayerChange=false;
	SPlayAreaConfig sPlayAreaConfig;
	SPlayerConfig sPlayerConfig;
	if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign)
	{
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetPlayAreaConfig(&sPlayAreaConfig);
	}
	if(m_PlayerManagerWrapper.m_piPlayerManager)
	{
		m_PlayerManagerWrapper.m_piPlayerManager->GetPlayerConfig(&sPlayerConfig);
	}

	if(piControl==m_piBTPlayAreaIncreaseAirPlane)
	{
		sPlayAreaConfig.dAirPlaneHeight+=5.0;
		bPlayAreaChange=true;
		bUpdateFormations=true;
	}
	else if(piControl==m_piBTPlayAreaDecreaseAirPlane)
	{
		sPlayAreaConfig.dAirPlaneHeight-=5.0;
		bPlayAreaChange=true;
		bUpdateFormations=true;
	}
	else if(piControl==m_piBTPlayAreaIncreaseScroll)
	{
		sPlayAreaConfig.dCameraScroll+=5.0;
		bPlayAreaChange=true;
	}
	else if(piControl==m_piBTPlayAreaDecreaseScroll)
	{
		sPlayAreaConfig.dCameraScroll-=5.0;
		bPlayAreaChange=true;
	}
	else if(piControl==m_piBTPlayAreaIncreaseCameraDistance)
	{
		sPlayAreaConfig.dCameraDistance+=5.0;
		bPlayAreaChange=true;
	}
	else if(piControl==m_piBTPlayAreaDecreaseCameraDistance)
	{
		sPlayAreaConfig.dCameraDistance-=5.0;
		bPlayAreaChange=true;
	}
	else if(piControl==m_piBTPlayAreaIncreaseCameraSpeed)
	{
		sPlayAreaConfig.dCameraSpeed+=5.0;
		bPlayAreaChange=true;
	}
	else if(piControl==m_piBTPlayAreaDecreaseCameraSpeed)
	{
		sPlayAreaConfig.dCameraSpeed-=5.0;
		bPlayAreaChange=true;
	}
	else if(piControl==m_piBTPlayAreaIncreaseCameraViewAngle)
	{
		sPlayAreaConfig.dCameraViewAngle+=1.0;
		bPlayAreaChange=true;
	}
	else if(piControl==m_piBTPlayAreaDecreaseCameraViewAngle)
	{
		sPlayAreaConfig.dCameraViewAngle-=1.0;
		bPlayAreaChange=true;
	}
	else if(piControl==m_piBTPlayAreaIncreaseCameraAspectRatio)
	{
		sPlayAreaConfig.dCameraAspectRatio+=0.05;
		bPlayAreaChange=true;
	}
	else if(piControl==m_piBTPlayAreaDecreaseCameraAspectRatio)
	{
		sPlayAreaConfig.dCameraAspectRatio-=0.05;
		bPlayAreaChange=true;
	}
	else if(piControl==m_piBTPlayAreaIncreaseDifficulty)
	{
		sPlayerConfig.dDifficulty+=1.0;
		bPlayerChange=true;
	}
	else if(piControl==m_piBTPlayAreaDecreaseDifficulty)
	{
		sPlayerConfig.dDifficulty-=1.0;
		if(sPlayerConfig.dDifficulty<0){sPlayerConfig.dDifficulty=0;}
		bPlayerChange=true;
	}
	else if(piControl==m_piBTPlayAreaAutoTakeOff)
	{
		CVector vStart,vEnd;
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&vStart,&vEnd);
		CVector vDir=vStart-vEnd; // Hacia la direccion inversa de la ruta.
		vDir.N();
		
		CMatrix m;
		m.R(AxisPosZ,DegreesToRadians(45)); // 60 grados hacia arriba.
		vDir*=m;		
		
		CVector vTerrainPos;
		CLine line;
		line.m_Points[0]=vStart;
		line.m_Points[1]=vStart+vDir*10000.0;
		GetTerrainCoordinatesFromLine(line,false,&vTerrainPos);
		vTerrainPos.c[1]+=3;
		
		sPlayerConfig.bPlayerTakeOffEnabled=true;
		sPlayerConfig.pvPlayerTakeOffPoints[0]=vTerrainPos-CVector(vStart.c[0]-vTerrainPos.c[0],0,0);
		sPlayerConfig.pvPlayerTakeOffPoints[1]=vTerrainPos;
		sPlayerConfig.pvPlayerTakeOffPoints[2]=vStart;
		sPlayerConfig.pvPlayerTakeOffPoints[3]=vStart+CVector(40,0,0);
		bPlayerChange=true;
	}
	else if(piControl==m_piBTPlayAreaAutoLanding)
	{
		CVector vStart,vEnd;
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&vStart,&vEnd);
		CVector vDir=vEnd-vStart;
		vDir.N();
		
		CMatrix m;
		m.R(AxisPosZ,DegreesToRadians(-45)); // 60 grados hacia abajo.
		vDir*=m;		
		
		CVector vTerrainPos;
		CLine line;
		line.m_Points[0]=vEnd;
		line.m_Points[1]=vEnd+vDir*10000.0;
		GetTerrainCoordinatesFromLine(line,false,&vTerrainPos);
		vTerrainPos.c[1]+=3;
		
		sPlayerConfig.bPlayerLandingEnabled=true;
		sPlayerConfig.pvPlayerLandingPoints[0]=vEnd-CVector(40,0,0);
		sPlayerConfig.pvPlayerLandingPoints[1]=vEnd;
		sPlayerConfig.pvPlayerLandingPoints[2]=vTerrainPos;
		sPlayerConfig.pvPlayerLandingPoints[3]=vTerrainPos-CVector(vEnd.c[0]-vTerrainPos.c[0],0,0);
		bPlayerChange=true;
	}
	if(m_nSelectedEntity!=-1 && m_nSelectedRoutePoint!=-1)
	{
		bool bRoutePointChanged=false;
		SRoutePoint sRoutePoint;
		m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetRoutePoint(m_nSelectedRoutePoint,&sRoutePoint);
		
		if(piControl==m_piBTRoutePointIncreasePause)
		{
			sRoutePoint.nPause+=100;
			bRoutePointChanged=true;
		}
		else if(piControl==m_piBTRoutePointDecreasePause)
		{
			sRoutePoint.nPause=sRoutePoint.nPause<=100?0:sRoutePoint.nPause-100;
			bRoutePointChanged=true;
		}
		else if(piControl==m_piBTRoutePointIncreaseSpeed)
		{
			sRoutePoint.dSpeedFactor+=0.1;
			bRoutePointChanged=true;
		}
		else if(piControl==m_piBTRoutePointDecreaseSpeed)
		{
			sRoutePoint.dSpeedFactor-=0.1;
			if(sRoutePoint.dSpeedFactor<0.1){sRoutePoint.dSpeedFactor=0.1;}
			bRoutePointChanged=true;
		}
		if(bRoutePointChanged)
		{
			m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->SetRoutePoint(m_nSelectedRoutePoint,sRoutePoint);
		}		
	}
	
	if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign && bPlayAreaChange)
	{
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->SetPlayAreaConfig(&sPlayAreaConfig);
	}
	if(m_PlayerManagerWrapper.m_piPlayerManager && bPlayerChange)
	{
		m_PlayerManagerWrapper.m_piPlayerManager->SetPlayerConfig(&sPlayerConfig);
	}
	if(bUpdateFormations)
	{
		UpdateFormationsHeight();
	}
}

void CScenarioEditorMainWindow::UpdateTexturization()
{
	if(m_WorldManagerWrapper.m_piTerrain)
	{
		m_WorldManagerWrapper.m_piTerrain->UpdateTerrain();
	}
	if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign)
	{
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->UpdateEntityLayers();
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->UpdatePlayArea();
	}
	
	if(m_PlayAreaManagerWrapper.m_piPlayAreaManager)
	{
		CVector vStart,vEnd;
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&vStart,&vEnd);
		m_vPlayMovementPosition.c[1]=vStart.c[1];
		m_vPlayMovementPosition.c[2]=vStart.c[2];
		if(m_vPlayMovementPosition.c[0]<vStart.c[0]){m_vPlayMovementPosition.c[0]=vStart.c[0];}
		if(m_vPlayMovementPosition.c[0]>vEnd.c[0]){m_vPlayMovementPosition.c[0]=vEnd.c[0];}
	}
	
}

void CScenarioEditorMainWindow::UpdateCaption()
{
	if(m_Viewport.m_piViewport)
	{
		std::string sCaption="Scenario Editor";
		if(m_sFile.length()){sCaption+=" - "+m_sFile;}
		m_Viewport.m_piViewport->SetCaption(sCaption);
	}
}

void CScenarioEditorMainWindow::UpdateLayerPanel()
{
	if(m_piGRHeightLayerPanel==NULL  ||m_piGRColorLayerPanel==NULL || m_piGREntityLayerPanel==NULL){return;}

	m_piBTShowOptionsPanel->SetBackgroundColor(CVector(1,1,1),m_bShowOptionsPanel?0.5:0.3);
	m_piBTShowEntitiesPanel->SetBackgroundColor(CVector(1,1,1),m_bShowEntitiesPanel?0.5:0.3);
	m_piBTShowFormationsPanel->SetBackgroundColor(CVector(1,1,1),m_bShowFormationsPanel?0.5:0.3);
	m_piBTShowTerrainPanel->SetBackgroundColor(CVector(1,1,1),m_bShowTerrainPanel?0.5:0.3);
	m_piBTShowFilePanel->SetBackgroundColor(CVector(1,1,1),m_bShowFilePanel?0.5:0.3);

	m_piGRGeneralPanel->Show(m_bShowGeneralPanel && m_bShowTerrainPanel);
	m_piGRWaterPanel->Show(m_bShowWaterPanel && m_bShowTerrainPanel);
	m_piGRFogPanel->Show(m_bShowFogPanel && m_bShowTerrainPanel);
	m_piGRRoutePointPanel->Show(m_nSelectedEntity!=-1 && m_nSelectedRoutePoint!=-1);
	m_piGRSunPanel->Show(m_bShowSunPanel && m_bShowTerrainPanel);
	m_piGRSkyPanel->Show(m_bShowSkyPanel && m_bShowTerrainPanel);
	m_piGRPlayAreaPanel->Show(m_bShowPlayAreaPanel && m_bShowTerrainPanel);
	m_piGREntitiesPanel->Show(m_bShowEntitiesPanel);
	m_piGRFormationsPanel->Show(m_bShowFormationsPanel);
	m_piGRTerrainPanel->Show(m_bShowTerrainPanel);
	m_piGROptionsPanel->Show(m_bShowOptionsPanel);
	m_piGRFile->Show(m_bShowFilePanel);
	m_piGRHeightLayerPanel->Show(false);
	m_piGRColorLayerPanel->Show(false);
	m_piGREntityLayerPanel->Show(false);
	m_piGREntityPanel->Show(false);
	m_piGRFormationPanel->Show(false);
	
	if(m_nSelectedHeightLayer!=-1)
	{
		STerrainHeightLayer layer;
		IGenericTexture *piTexture=NULL;
		IGenericTexture *piNormalMap=NULL;
		m_WorldManagerWrapper.m_piTerrain->GetTerrainHeightLayer(m_nSelectedHeightLayer,&layer,&piTexture,&piNormalMap);
		m_piBTHeightLayerSample->SetBackgroundTexture(piTexture);
		m_piBTHeightLayerNormalMapSample->SetBackgroundTexture(piNormalMap);
		m_piGRHeightLayerPanel->Show(m_bShowTerrainPanel);

		unsigned int nWidth=0,nHeight=0;
		if(piTexture){piTexture->GetSize(&nWidth,&nHeight);}

		char A[MAX_PATH];
		sprintf(A,"%s (%d x %d)",layer.sTextureFile.c_str(),nWidth,nHeight);
		m_piSTHeightLayerName->SetText(A);

		sprintf(A,"Min   : %.02f",layer.dMinHeight);
		m_piSTHeightLayerMinHeight->SetText(A);

		sprintf(A,"Max   : %.02f",layer.dMaxHeight);
		m_piSTHeightLayerMaxHeight->SetText(A);

		sprintf(A,"Margin: %.02f",layer.dDecayMargin);
		m_piSTHeightLayerMargin->SetText(A);

		sprintf(A,"H.Res : %.02f",layer.dHorizontalResolution);
		m_piSTHeightLayerHorzResolution->SetText(A);

		sprintf(A,"V.Res : %.02f",layer.dVerticalResolution);
		m_piSTHeightLayerVertResolution->SetText(A);

		m_piBTHeightLayerIncreaseMinHeight->Show(m_nSelectedHeightLayer!=0);
		m_piBTHeightLayerDecreaseMinHeight->Show(m_nSelectedHeightLayer!=0);
		m_piBTHeightLayerIncreaseMaxHeight->Show(m_nSelectedHeightLayer<(int)(m_vHeightLayerControls.size()-1));
		m_piBTHeightLayerDecreaseMaxHeight->Show(m_nSelectedHeightLayer<(int)(m_vHeightLayerControls.size()-1));

		REL(piTexture);
		REL(piNormalMap);
	}
	else if(m_nSelectedColorLayer!=-1)
	{
		STerrainColorLayer layer;
		IGenericTexture *piTexture=NULL;
		IGenericTexture *piNormalMap=NULL;
		m_WorldManagerWrapper.m_piTerrain->GetTerrainColorLayer(m_nSelectedColorLayer,&layer,&piTexture,&piNormalMap);
		m_piBTColorLayerSample->SetBackgroundTexture(piTexture);
		m_piBTColorLayerNormalMapSample->SetBackgroundTexture(piNormalMap);
		m_piBTColorLayerAbruptEnd->SetText(layer.bAbruptEnd?"Sharp Mix":"Soft Mix");
		m_piBTColorLayerColorSample->SetBackgroundColor(layer.vColor,1.0);

		m_piGRColorLayerPanel->Show(m_bShowTerrainPanel);

		unsigned int nWidth=0,nHeight=0;
		if(piTexture){piTexture->GetSize(&nWidth,&nHeight);}

		char A[MAX_PATH];
		sprintf(A,"%s (%d x %d)",layer.sTextureFile.c_str(),nWidth,nHeight);
		m_piSTColorLayerName->SetText(A);

		sprintf(A,"H.Res : %.02f",layer.dHorizontalResolution);
		m_piSTColorLayerHorzResolution->SetText(A);

		sprintf(A,"V.Res : %.02f",layer.dVerticalResolution);
		m_piSTColorLayerVertResolution->SetText(A);
		REL(piTexture);
		REL(piNormalMap);
	}

	if(m_nSelectedEntityLayer!=-1)
	{
		SEntityLayer layer;
		IEntityType *piEntityType=NULL;
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetEntityLayer(m_nSelectedEntityLayer,&layer,&piEntityType);
		IDesignObject *piDesignObject=QI(IDesignObject,piEntityType);

		m_piSTEntityLayerName->SetText(layer.sEntityType);
		m_piSTEntityLayerObjectLabel->SetObject(piDesignObject);
		m_piBTEntityLayerColorSample->SetBackgroundColor(layer.vColor,1.0);
		m_piGREntityLayerPanel->Show(m_bShowEntitiesPanel);

		char A[MAX_PATH];
		sprintf(A,"Spacing  : %.02f",layer.dSeparation);
		m_piSTEntityLayerSeparation->SetText(A);
		sprintf(A,"Position : %.02f",layer.dPositionJitter);
		m_piSTEntityLayerPositionJitter->SetText(A);
		sprintf(A,"Angle    : %.02f",layer.dYawJitter);
		m_piSTEntityLayerYawJitter->SetText(A);
		REL(piEntityType);
		REL(piDesignObject);
	}
	if(m_nSelectedEntity!=-1)
	{
		m_piSTEntityName->SetText(m_vEntityControls[m_nSelectedEntity]->m_piObject->GetName());
		m_piSTEntityObjectLabel->SetObject(m_vEntityControls[m_nSelectedEntity]->m_piDesignObject);
		m_piSTEntityBonusObjectLabel->SetObject(m_vEntityControls[m_nSelectedEntity]->m_piBonusDesignObject);
		m_piGREntityPanel->Show(m_bShowEntitiesPanel);

		CVector vAngles=m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetAngles();
		char A[MAX_PATH];
		sprintf(A,"Yaw   : %.02f",vAngles.c[YAW]);
		m_piSTEntityYaw->SetText(A);
		
		sprintf(A,"Count : %d",m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetCount());
		m_piSTEntityCount->SetText(A);
		
		sprintf(A,"Delay : %.02fs",((double)m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetDelay())/1000.0);
		m_piSTEntityDelay->SetText(A);

		if(m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetBonusOnChild()!=-1)
		{
			sprintf(A,"On Child: %d",m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetBonusOnChild());
		}
		else
		{
			sprintf(A,"On Child: N/A");
		}
		m_piSTEntityBonusOnChild->SetText(A);
		
		sprintf(A,"R.Del : %.02fs",((double)m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetRouteDelay())/1000.0);
		m_piSTEntityRouteDelay->SetText(A);
		
		sprintf(A,"Inter : %.02fs",((double)m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetInterval())/1000.0);
		m_piSTEntityInterval->SetText(A);
	}
	if(m_nSelectedFormation!=-1)
	{
		char A[MAX_PATH];
		unsigned int nConditionValue=0;
		m_vFormationControls[m_nSelectedFormation]->m_piPlayAreaFormation->GetCondition(NULL,&nConditionValue);
		
		if(nConditionValue)
		{sprintf(A,"Max.Weapon = %d",nConditionValue+1);}
		else
		{sprintf(A,"No condition");}

		m_piSTFormationCondition->SetText(A);
		m_piSTFormationObjectLabel->SetObject(m_vFormationControls[m_nSelectedFormation]->m_piDesignObject);
		m_piSTFormationBonusObjectLabel->SetObject(m_vFormationControls[m_nSelectedFormation]->m_piBonusDesignObject);
		m_piSTFormationAlternativeObjectLabel->SetObject(m_vFormationControls[m_nSelectedFormation]->m_piAlternativeDesignObject);
		m_piSTFormationAlternativeBonusObjectLabel->SetObject(m_vFormationControls[m_nSelectedFormation]->m_piAlternativeBonusDesignObject);
		m_piGRFormationPanel->Show(m_bShowFormationsPanel);
	}
	STerrainWater sWater;
	IGenericTexture *piWaterTexture1=NULL;
	IGenericTexture *piWaterTexture2=NULL;
	IGenericModel *piModel=NULL;
	CVector vBaseModelMins,vBaseModelMaxs;
	if(m_WorldManagerWrapper.m_piTerrain){m_WorldManagerWrapper.m_piTerrain->GetTerrainBaseModel(NULL,&piModel);}
	if(piModel){piModel->GetFrameBBox(0,0,&vBaseModelMins,&vBaseModelMaxs);}
	REL(piModel);
	
	if(m_WorldManagerWrapper.m_piTerrain)
	{
		m_WorldManagerWrapper.m_piTerrain->GetTerrainWater(&sWater,&piWaterTexture1,&piWaterTexture2);
	}

	m_piBTWaterSample->SetBackgroundTexture(piWaterTexture1);
	m_piBTWaterSecondSample->SetBackgroundTexture(piWaterTexture2);

	unsigned int nWidth=0,nHeight=0;
	unsigned int nSecondWidth=0,nSecondHeight=0;
	if(piWaterTexture1){piWaterTexture1->GetSize(&nWidth,&nHeight);}
	if(piWaterTexture2){piWaterTexture2->GetSize(&nSecondWidth,&nSecondHeight);}

	char A[MAX_PATH];
	sprintf(A,"%s (%d x %d)",sWater.sTextureFile1.c_str(),nWidth,nHeight);
	m_piSTWaterName->SetText(A);

	sprintf(A,"%s (%d x %d)",sWater.sTextureFile2.c_str(),nSecondWidth,nSecondHeight);
	m_piSTWaterSecondName->SetText(A);

	sprintf(A,"Height  : %.02f",(sWater.vMaxs.c[1]-vBaseModelMins.c[1])/(vBaseModelMaxs.c[1]-vBaseModelMins.c[1]));
	m_piSTWaterHeight->SetText(A);

	sprintf(A,"Opacity : %.f%%",sWater.dOpacity*100.0);
	m_piSTWaterOpacity->SetText(A);

	sprintf(A,"Speed   : %.f%%",sWater.dSpeed*1000.0);
	m_piSTWaterSpeed->SetText(A);

	sprintf(A,"H.Res : %.02f",sWater.dHorizontalResolution);
	m_piSTWaterHorzResolution->SetText(A);

	sprintf(A,"V.Res : %.02f",sWater.dVerticalResolution);
	m_piSTWaterVertResolution->SetText(A);

	m_piBTWaterEnable->SetText(sWater.bEnabled?"Enabled":"Disabled");
	m_piBTWaterEnable->SetBackgroundColor(sWater.bEnabled?CVector(0.8,0.8,0.8):CVector(0.6,0.6,0.6),1.0);
	m_piBTWaterAuto->Show(sWater.bEnabled);
	m_piBTWaterIncreaseHorzResolution->Show(sWater.bEnabled);
	m_piBTWaterDecreaseHorzResolution->Show(sWater.bEnabled);
	m_piBTWaterIncreaseVertResolution->Show(sWater.bEnabled);
	m_piBTWaterDecreaseVertResolution->Show(sWater.bEnabled);
	m_piBTWaterIncreaseOpacity->Show(sWater.bEnabled);
	m_piBTWaterDecreaseOpacity->Show(sWater.bEnabled);
	m_piBTWaterIncreaseSpeed->Show(sWater.bEnabled);
	m_piBTWaterDecreaseSpeed->Show(sWater.bEnabled);
	m_piBTWaterIncreaseHeight->Show(sWater.bEnabled);
	m_piBTWaterDecreaseHeight->Show(sWater.bEnabled);
	m_piBTWaterSample->Show(sWater.bEnabled);
	m_piBTWaterSecondSample->Show(sWater.bEnabled);
	REL(piWaterTexture1);
	REL(piWaterTexture2);

	// Sky

	STerrainSky sSky;
	IGenericTexture *piSkyTexture=NULL;
	if(m_WorldManagerWrapper.m_piTerrain)
	{
		m_WorldManagerWrapper.m_piTerrain->GetTerrainSky(&sSky,&piSkyTexture);
	}
	m_piBTSkySample->SetBackgroundTexture(piSkyTexture);

	nWidth=0,nHeight=0;
	if(piSkyTexture){piSkyTexture->GetSize(&nWidth,&nHeight);}

	sprintf(A,"%s (%d x %d)",sSky.sTextureFile.c_str(),nWidth,nHeight);
	m_piSTSkyName->SetText(A);

	sprintf(A,"Opacity : %.f%%",sSky.dOpacity*100.0);
	m_piSTSkyOpacity->SetText(A);

	sprintf(A,"Speed   : %.f%%",sSky.dSpeed*1000.0);
	m_piSTSkySpeed->SetText(A);

	sprintf(A,"H.Res : %.02f",sSky.dHorizontalResolution);
	m_piSTSkyHorzResolution->SetText(A);

	sprintf(A,"V.Res : %.02f",sSky.dVerticalResolution);
	m_piSTSkyVertResolution->SetText(A);

	m_piBTSkyEnable->SetText(sSky.bEnabled?"Enabled":"Disabled");
	m_piBTSkyEnable->SetBackgroundColor(sSky.bEnabled?CVector(0.8,0.8,0.8):CVector(0.6,0.6,0.6),1.0);
	m_piBTSkyIncreaseHorzResolution->Show(sSky.bEnabled);
	m_piBTSkyDecreaseHorzResolution->Show(sSky.bEnabled);
	m_piBTSkyIncreaseVertResolution->Show(sSky.bEnabled);
	m_piBTSkyDecreaseVertResolution->Show(sSky.bEnabled);
	m_piBTSkyIncreaseOpacity->Show(sSky.bEnabled);
	m_piBTSkyDecreaseOpacity->Show(sSky.bEnabled);
	m_piBTSkyIncreaseSpeed->Show(sSky.bEnabled);
	m_piBTSkyDecreaseSpeed->Show(sSky.bEnabled);
	m_piBTSkySample->Show(sSky.bEnabled);

	REL(piSkyTexture);


	CVector vAmbientColor;
	STerrainSun sSun;
	if(m_WorldManagerWrapper.m_piTerrain)
	{
		m_WorldManagerWrapper.m_piTerrain->GetTerrainSun(&sSun);
		vAmbientColor=m_WorldManagerWrapper.m_piTerrain->GetTerrainAmbientColor();
	}

	m_piBTSunColorSample->SetBackgroundColor(sSun.vColor,1);
	m_piBTAmbientColorSample->SetBackgroundColor(vAmbientColor,1);

	sprintf(A,"Distance  : %.02f",sSun.dDistance);
	m_piSTSunDistance->SetText(A);

	sprintf(A,"Elevation : %.f",sSun.dElevation);
	m_piSTSunElevation->SetText(A);

	sprintf(A,"Azimuth   : %.f",sSun.dAzimuth);
	m_piSTSunAzimuth->SetText(A);



	STerrainFog sFog;
	if(m_WorldManagerWrapper.m_piTerrain)
	{
		m_WorldManagerWrapper.m_piTerrain->GetTerrainFog(&sFog);
	}

	m_piBTFogColorSample->SetBackgroundColor(sFog.vColor,1);

	sprintf(A,"Min    : %.f",sFog.vMins.c[1]);
	m_piSTFogStart->SetText(A);

	sprintf(A,"Max    : %.f",sFog.vMaxs.c[1]);
	m_piSTFogEnd->SetText(A);

	m_piBTFogEnable->SetText(sFog.bEnabled?"Enabled":"Disabled");
	m_piBTFogEnable->SetBackgroundColor(sFog.bEnabled?CVector(0.8,0.8,0.8):CVector(0.6,0.6,0.6),1.0);
	m_piBTFogIncreaseStart->Show(sFog.bEnabled);
	m_piBTFogDecreaseStart->Show(sFog.bEnabled);
	m_piBTFogIncreaseEnd->Show(sFog.bEnabled);
	m_piBTFogDecreaseEnd->Show(sFog.bEnabled);
	m_piBTFogColorSample->Show(sFog.bEnabled);
	m_piBTFogFitWater->Show(sFog.bEnabled);
	m_piBTFogFitSky->Show(sFog.bEnabled);

	SPlayAreaConfig sPlayAreaConfig;
	SPlayerConfig sPlayerConfig;
	if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign)
	{
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetPlayAreaConfig(&sPlayAreaConfig);
	}
	if(m_PlayerManagerWrapper.m_piPlayerManager)
	{
		m_PlayerManagerWrapper.m_piPlayerManager->GetPlayerConfig(&sPlayerConfig);
	}
	sprintf(A,"Cam Dist     : %.f",sPlayAreaConfig.dCameraDistance);
	m_piSTPlayAreaCameraDistance->SetText(A);

	sprintf(A,"Cam Speed    : %.f",sPlayAreaConfig.dCameraSpeed);
	m_piSTPlayAreaCameraSpeed->SetText(A);

	sprintf(A,"View Angle   : %.02f",sPlayAreaConfig.dCameraViewAngle);
	m_piSTPlayAreaCameraViewAngle->SetText(A);

	sprintf(A,"Aspect Ratio : %.02f",sPlayAreaConfig.dCameraAspectRatio);
	m_piSTPlayAreaCameraAspectRatio->SetText(A);

	sprintf(A,"Scroll     : %.f",sPlayAreaConfig.dCameraScroll);
	m_piSTPlayAreaScroll->SetText(A);
	
	sprintf(A,"Difficulty : %.f",sPlayerConfig.dDifficulty);
	m_piSTPlayAreaDifficulty->SetText(A);
	
	sprintf(A,"Air Plane  : %.f",sPlayAreaConfig.dAirPlaneHeight);
	m_piSTPlayAreaAirPlane->SetText(A);


	SGameRect sListRect;
	if(m_piGRLayerList){m_piGRLayerList->GetRealRect(&sListRect);}

	double dCurrentY=sListRect.h;

	if(m_piBTNewColorLayer)
	{
		dCurrentY-=20+6;
		SGameRect sRowRect(3,dCurrentY,sListRect.w-6,20);
		m_piBTNewColorLayer->SetRect(&sRowRect);
	}

	for(int x=m_vColorLayerControls.size()-1;x>=0;x--)
	{
		dCurrentY-=41;
		SGameRect sRowRect(3,dCurrentY,sListRect.w-6,38);
		SColorLayerControls *pData=m_vColorLayerControls[x];
		IGenericTexture *piTexture=NULL;
		STerrainColorLayer layer;
		m_WorldManagerWrapper.m_piTerrain->GetTerrainColorLayer(x,&layer,&piTexture,NULL);

		if(pData->m_BTListRow.m_piButton)
		{
			pData->m_BTListRow.m_piButton->SetRect(&sRowRect);
			pData->m_BTListRow.m_piButton->SetBackgroundColor(CVector(1,1,1),x==m_nSelectedColorLayer?0.8:0.1);
		}
		if(pData->m_STTexture.m_piLabel)
		{
			SGameRect sRect(3,3,32,32);
			pData->m_STTexture.m_piLabel->SetRect(&sRect);
			pData->m_STTexture.m_piLabel->SetBackgroundTexture(piTexture);
		}
		if(pData->m_STDescription.m_piLabel)
		{
			SGameRect sRect(38,3,sRowRect.w-32-9,32);
			pData->m_STDescription.m_piLabel->SetRect(&sRect);
			pData->m_STDescription.m_piLabel->SetBackgroundColor(layer.vColor,1);
			pData->m_STDescription.m_piLabel->SetTextColor(CVector(0,0,0),1);

			char A[MAX_PATH];
			sprintf(A,"%.f,%.f,%.f",layer.vColor.c[0]*255.0,layer.vColor.c[1]*255.0,layer.vColor.c[2]*255.0);
			pData->m_STDescription.m_piLabel->SetText(A);
		}
		REL(piTexture);
	}

	if(m_piBTNewHeightLayer)
	{
		dCurrentY-=20+6;
		SGameRect sRowRect(3,dCurrentY,sListRect.w-6,20);
		m_piBTNewHeightLayer->SetRect(&sRowRect);
	}

	dCurrentY-=3;
	for(int x=m_vHeightLayerControls.size()-1;x>=0;x--)
	{
		dCurrentY-=38;
		IGenericTexture *piTexture=NULL;
		STerrainHeightLayer layer;
		m_WorldManagerWrapper.m_piTerrain->GetTerrainHeightLayer(x,&layer,&piTexture,NULL);

		SGameRect sRowRect(3,dCurrentY,sListRect.w-6,38);
		SHeightLayerControls *pData=m_vHeightLayerControls[x];
		if(pData->m_BTListRow.m_piButton)
		{
			pData->m_BTListRow.m_piButton->SetRect(&sRowRect);
			pData->m_BTListRow.m_piButton->SetBackgroundColor(CVector(1,1,1),x==m_nSelectedHeightLayer?0.8:0.1);
		}
		if(pData->m_STTexture.m_piLabel)
		{
			SGameRect sRect(3,3,32,32);
			pData->m_STTexture.m_piLabel->SetRect(&sRect);
			pData->m_STTexture.m_piLabel->SetBackgroundTexture(piTexture);
		}
		if(pData->m_STDescription.m_piLabel)
		{
			SGameRect sRect(38,3,sRowRect.w-32-9,32);
			pData->m_STDescription.m_piLabel->SetRect(&sRect);
			pData->m_STDescription.m_piLabel->SetTextColor(CVector(1,1,1),1);
			
			char A[MAX_PATH];
			sprintf(A,"%0.2f - %0.2f",layer.dMinHeight,layer.dMaxHeight);
			pData->m_STDescription.m_piLabel->SetText(A);
		}
		REL(piTexture);
	}


	if(m_piGREntityLayerList){m_piGREntityLayerList->GetRealRect(&sListRect);}
	dCurrentY=sListRect.h;

	if(m_piBTNewEntityLayer)
	{
		dCurrentY-=20+6;
		SGameRect sRowRect(3,dCurrentY,sListRect.w-6,20);
		m_piBTNewEntityLayer->SetRect(&sRowRect);
	}

	for(int x=m_vEntityLayerControls.size()-1;x>=0;x--)
	{
		IEntityType *piEntityType=NULL;
		SEntityLayer layer;
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetEntityLayer(x,&layer,&piEntityType);
		IDesignObject *piDesignObject=QI(IDesignObject,piEntityType);

		dCurrentY-=41;
		SGameRect sRowRect(3,dCurrentY,sListRect.w-6,38);
		SEntityLayerControls *pData=m_vEntityLayerControls[x];
		if(pData->m_BTListRow.m_piButton)
		{
			pData->m_BTListRow.m_piButton->SetRect(&sRowRect);
			pData->m_BTListRow.m_piButton->SetBackgroundColor(CVector(1,1,1),x==m_nSelectedEntityLayer?0.8:0.1);
		}
		if(pData->m_STEntity.m_piLabel)
		{
			SGameRect sRect(3,3,32,32);
			pData->m_STEntity.m_piLabel->SetRect(&sRect);
			pData->m_STEntity.m_piLabel->SetObject(piDesignObject);
		}
		if(pData->m_STDescription.m_piLabel)
		{
			SGameRect sRect(38,3,sRowRect.w-32-9,32);
			pData->m_STDescription.m_piLabel->SetRect(&sRect);
			pData->m_STDescription.m_piLabel->SetBackgroundColor(layer.vColor,1);
			pData->m_STDescription.m_piLabel->SetTextColor(CVector(0,0,0),1);

			char A[MAX_PATH];
			sprintf(A,"%.f,%.f,%.f",layer.vColor.c[0]*255.0,layer.vColor.c[1]*255.0,layer.vColor.c[2]*255.0);
			pData->m_STDescription.m_piLabel->SetText(A);
		}
		REL(piEntityType);
		REL(piDesignObject);
	}
	if(m_piBTNewEntity)
	{
		dCurrentY-=20+6;
		SGameRect sRowRect(3,dCurrentY,sListRect.w-6,20);
		m_piBTNewEntity->SetRect(&sRowRect);
	}
	if(m_piLSEntityList)
	{
		dCurrentY-=3;
		SGameRect sEntityListRect(3,3,sListRect.w-6,dCurrentY-6);
		m_piLSEntityList->SetRect(&sEntityListRect);
		dCurrentY=0;
	}
	
	if(m_piGRFormationsPanel){m_piGRFormationsPanel->GetRealRect(&sListRect);}
	dCurrentY=sListRect.h;

	if(m_piBTNewFormation)
	{
		dCurrentY-=20+6;
		SGameRect sRowRect(3,dCurrentY,sListRect.w-6,20);
		m_piBTNewFormation->SetRect(&sRowRect);
	}
	if(m_piLSFormationList)
	{
		dCurrentY-=3;
		SGameRect sFormationListRect(3,3,sListRect.w-6,dCurrentY-6);
		m_piLSFormationList->SetRect(&sFormationListRect);
		dCurrentY=0;
	}
	
	if(m_nSelectedEntity!=-1 && m_nSelectedRoutePoint!=-1)
	{		
		SRoutePoint sRoutePoint;
		m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetRoutePoint(m_nSelectedRoutePoint,&sRoutePoint);
		
		sprintf(A,"Pause : %.02fs",((double)sRoutePoint.nPause)/1000.0);
		m_piSTRoutePointPause->SetText(A);
		
		sprintf(A,"Speed : %.02fs",sRoutePoint.dSpeedFactor);
		m_piSTRoutePointSpeed->SetText(A);
	
		sprintf(A,"Route Point %d",m_nSelectedRoutePoint);
		m_piSTRoutePointCaption->SetText(A);
	}


	std::string sModelName;
	if(m_WorldManagerWrapper.m_piTerrain){m_WorldManagerWrapper.m_piTerrain->GetTerrainBaseModel(&sModelName,NULL);}
	sprintf(A,"Model    : %s",sModelName.c_str());
	m_piSTGeneralModelName->SetText(A);

	std::string sColorMap;
	if(m_WorldManagerWrapper.m_piTerrain){m_WorldManagerWrapper.m_piTerrain->GetTerrainColorMap(&sColorMap,NULL);}
	sprintf(A,"Color Map: %s",sColorMap.c_str());
	m_piSTGeneralColorMapName->SetText(A);

	std::string sMusicName;
	if(m_MusicManagerWrapper.m_piMusicDesign){m_MusicManagerWrapper.m_piMusicDesign->GetMusic(&sMusicName,NULL);}
	sprintf(A,"Music    : %s",sMusicName.c_str());
	m_piSTGeneralMusicName->SetText(A);
	
	std::string sIntroMusicName;
	if(m_MusicManagerWrapper.m_piMusicDesign){m_MusicManagerWrapper.m_piMusicDesign->GetIntroMusic(&sIntroMusicName,NULL);}
	sprintf(A,"Intro    : %s",sIntroMusicName.c_str());
	m_piSTGeneralIntroMusicName->SetText(A);
	
}

void CScenarioEditorMainWindow::CenterCamera()
{
	bool bCenter=false;
	CVector vCenter,vSize;
	if(m_nSelectedFormation!=-1 && m_vFormationControls[m_nSelectedFormation]->m_piDesignObject)
	{
		CVector vMins,vMaxs;
		m_vFormationControls[m_nSelectedFormation]->m_piDesignObject->DesignGetAABBox(Origin,Origin,&vMins,&vMaxs);
		CVector vPos=m_vFormationControls[m_nSelectedFormation]->m_piPlayAreaFormation->GetPosition();
		bCenter=true;
		vCenter=vPos+(vMaxs+vMins)*0.5;
		vSize=(vMaxs-vMins);
	}
	else if(m_nSelectedEntity!=-1 && m_vEntityControls[m_nSelectedEntity]->m_piDesignObject)
	{
		CVector vMins,vMaxs;
		m_vEntityControls[m_nSelectedEntity]->m_piDesignObject->DesignGetAABBox(Origin,Origin,&vMins,&vMaxs);
		CVector vPos=m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetPosition();
		bCenter=true;
		vCenter=vPos+(vMaxs+vMins)*0.5;
		vSize=(vMaxs-vMins);
	}
	else 
	{
		CVector vMins,vMaxs;
		m_WorldManagerWrapper.m_piTerrain->GetTerrainBBox(&vMins,&vMaxs);
		bCenter=true;
		vCenter=(vMaxs+vMins)*0.5;
		vSize=(vMaxs-vMins);
	}
	if(bCenter)
	{
		double dNearPlane=0,dFarPlane=0;
		m_Camera.m_piCamera->GetClippingPlanes(dNearPlane,dFarPlane);

		CVector vPos;
		vPos.c[0]=vCenter.c[0];
		vPos.c[2]=vCenter.c[2]+fabs(tan(m_Camera.m_piCamera->GetViewAngle()*2.0)*(vSize.c[0]+dNearPlane));
		vPos.c[1]=vCenter.c[1]+fabs(tan(m_Camera.m_piCamera->GetViewAngle()*2.0)*(vSize.c[0]+dNearPlane));
		CVector vAngles;
		m_Camera.m_piCamera->SetPosition(vPos);
		AnglesFromVector(vCenter-vPos,&vAngles);
		m_Camera.m_piCamera->SetAngles(vAngles);
	}
}

void CScenarioEditorMainWindow::OnCharacter(int nKey,bool *pbProcessed)
{
	bool bUpdatePlayerWeapon=false;
	if     (nKey=='T'|| nKey=='t'){m_bTextures=!m_bTextures;*pbProcessed=true;}
	else if(nKey=='P'|| nKey=='p'){m_bRenderPlayArea=!m_bRenderPlayArea;*pbProcessed=true;}
	else if(nKey=='G'|| nKey=='g'){m_bFog=!m_bFog;*pbProcessed=true;}
	else if(nKey=='L'|| nKey=='l'){m_bSolid=!m_bSolid;*pbProcessed=true;}
	else if(nKey=='I'|| nKey=='i'){m_bLighting=!m_bLighting;*pbProcessed=true;}
	else if(nKey=='N'|| nKey=='n'){m_bNormalMaps=!m_bNormalMaps;*pbProcessed=true;}
	else if(nKey=='Y'|| nKey=='y'){m_bSkyShadow=!m_bSkyShadow;*pbProcessed=true;}
	else if(nKey=='O'|| nKey=='o'){m_bShadows=!m_bShadows;*pbProcessed=true;}
	else if(nKey=='B'|| nKey=='b'){m_bBlend=!m_bBlend;*pbProcessed=true;}
	else if(nKey=='1'){m_nStartingWeapon=1;*pbProcessed=true;bUpdatePlayerWeapon=true;}
	else if(nKey=='2'){m_nStartingWeapon=2;*pbProcessed=true;bUpdatePlayerWeapon=true;}
	else if(nKey=='3'){m_nStartingWeapon=3;*pbProcessed=true;bUpdatePlayerWeapon=true;}
	else if(nKey=='4'){m_nStartingWeapon=4;*pbProcessed=true;bUpdatePlayerWeapon=true;}
	else if(nKey=='5'){m_nStartingWeapon=5;*pbProcessed=true;bUpdatePlayerWeapon=true;}
	else if(nKey=='6'){m_nStartingWeapon=6;*pbProcessed=true;bUpdatePlayerWeapon=true;}
	else if(nKey==' '){m_FrameManager.m_piFrameManager->SetPauseOnNextFrame(false);m_bPauseOnNextFrame=true;*pbProcessed=true;}
	else if(nKey=='M'|| nKey=='m')
	{
		if(m_eShadingModel==eShadingModel_Gouraud){m_eShadingModel=eShadingModel_Balanced;}
		else if(m_eShadingModel==eShadingModel_Balanced){m_eShadingModel=eShadingModel_Phong;}
		else if(m_eShadingModel==eShadingModel_Phong){m_eShadingModel=eShadingModel_Gouraud;}
		*pbProcessed=true;
	}
	
	if(bUpdatePlayerWeapon)
	{
		IEntity *piPlayerEntity=m_EntityManagerWrapper.m_piEntityManager->FindEntity("Player");
		IWeapon *piBulletWeapon=piPlayerEntity?piPlayerEntity->GetWeapon(0):NULL;
		if(piBulletWeapon){piBulletWeapon->SetCurrentLevel(m_nStartingWeapon-1);}
	}
}

void CScenarioEditorMainWindow::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_F1 && m_bSimulationStarted){m_bInspectionMode=!m_bInspectionMode;*pbProcessed=true;}
	else if(nKey==GK_F2){ProcessFileSave();*pbProcessed=true;}
	else if(nKey==GK_F3){ProcessFileOpen();*pbProcessed=true;}
	else if(nKey==GK_F4){m_bStats=!m_bStats;*pbProcessed=true;}
	else if(nKey==GK_F5){if(m_piGUIManager->IsKeyDown(GK_LSHIFT)){StopGameSimulation();}else{StartGameSimulation();}*pbProcessed=true;}
	else if(nKey==GK_F12){m_Render.m_piRender->ReloadShaders();*pbProcessed=true;}
	else if(nKey==GK_PAUSE){m_FrameManager.m_piFrameManager->TogglePauseOnNextFrame();*pbProcessed=true;}
	else if(nKey==GK_HOME){CenterCamera();*pbProcessed=true;}
	else if(nKey==GK_DELETE)
	{
		*pbProcessed=true;
		
		if(m_bRenderPlayArea && m_nSelectedCheckPoint!=-1)
		{
			if(ConfirmDialog("Do you really want to delete the checkpoint?","Delete checkpoint",eMessageDialogType_Warning))
			{
				m_PlayAreaManagerWrapper.m_piPlayAreaDesign->RemoveElement(m_vCheckPoints[m_nSelectedCheckPoint]->m_nPlayAreaElementId);
				UpdateCheckPoints();
				m_nSelectedCheckPoint=-1;
			}
		}
		if(m_nSelectedEntity!=-1 && m_nSelectedRoutePoint!=-1)
		{
			m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->RemoveRoutePoint(m_nSelectedRoutePoint);
			if(m_nSelectedRoutePoint>=(int)m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetRoutePoints())
			{
				m_nSelectedRoutePoint=m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetRoutePoints()-1;
			}
		}
	}
	else if(nKey==GK_INSERT)
	{
		*pbProcessed=true;
		if(m_bRenderPlayArea)
		{
			if(ConfirmDialog("Add a new checkpoint?","New checkpoint",eMessageDialogType_Warning))
			{
				unsigned long nObjectIndex=m_PlayAreaManagerWrapper.m_piPlayAreaDesign->AddElement("CPlayAreaCheckPoint");
				IPlayAreaElement *piElement=NULL;
				m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetElement(nObjectIndex,&piElement);
				IPlayAreaCheckPoint *piCheckPoint=QI(IPlayAreaCheckPoint,piElement);
				if(piCheckPoint)
				{
					CVector vPos;
					GetAirPlaneCoordinatesFromCursorPos(m_rRealRect.w/2,m_rRealRect.h/2,&vPos);
					piCheckPoint->SetCheckPointPosition(vPos);
				}
				REL(piCheckPoint);
				REL(piElement);
				UpdateCheckPoints();
				m_nSelectedCheckPoint=m_vCheckPoints.size()-1;
			}
		}
		else if(m_nSelectedEntity!=-1)
		{
			SRoutePoint sRoutePoint;
			sRoutePoint.bAbsolutePoint=true;
			if(m_nSelectedRoutePoint==-1)
			{
				CVector vForward;
				VectorsFromAngles(m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetAngles(),&vForward);
				sRoutePoint.vPosition=m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetPosition();
				sRoutePoint.vPosition+=vForward*(m_vEntityControls[m_nSelectedEntity]->m_piEntityType->DesignGetRadius()*2.0);
			}
			else
			{
				SRoutePoint sOldPoint1;
				SRoutePoint sOldPoint2;
				if(m_nSelectedRoutePoint>0)
				{
					m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetRoutePoint(m_nSelectedRoutePoint-1,&sOldPoint1);
				}
				else
				{
					sOldPoint1.vPosition=m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetPosition();
				}
				m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetRoutePoint(m_nSelectedRoutePoint,&sOldPoint2);
				CVector vDirection=sOldPoint2.vPosition-sOldPoint1.vPosition;
				vDirection.N();
				sRoutePoint.vPosition+=sOldPoint2.vPosition+vDirection*(m_vEntityControls[m_nSelectedEntity]->m_piEntityType->DesignGetRadius());
			}
			m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->AddRoutePoint(m_nSelectedRoutePoint+1,sRoutePoint);
			m_nSelectedRoutePoint=m_nSelectedRoutePoint+1;
		}
	}
	else if(nKey==GK_ESCAPE)
	{
		if(m_nSelectedEntity!=-1 || m_nSelectedEntityLayer!=-1 || 
			m_nSelectedFormation!=-1 || m_nSelectedRoutePoint!=-1)
		{
			m_nSelectedEntity=-1;
			m_nSelectedEntityLayer=-1 ;
			m_nSelectedFormation=-1;
			m_nSelectedRoutePoint=-1;
			if(m_piLSEntityList){m_piLSEntityList->SetSelectedElement(m_nSelectedEntity);}
			if(m_piLSFormationList){m_piLSFormationList->SetSelectedElement(m_nSelectedFormation);}
		}
		else
		{
			ProcessFileExit();
		}
		*pbProcessed=true;
	}
}

void CScenarioEditorMainWindow::OnMouseDoubleClick(int nButton,double dx,double dy)
{
	IGenericCamera *piCamera=NULL;
	if(!m_bSimulationStarted || m_bInspectionMode)
	{
		piCamera=ADD(m_Camera.m_piCamera);
	}
	else
	{
		piCamera=m_PlayAreaManagerWrapper.m_piPlayAreaManager?m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCamera():ADD(m_Camera.m_piCamera);
	}
	
	if(m_bRenderPlayArea)
	{
		m_Render.m_piRender->StartSelection(m_rRealRect,piCamera,dx,dy,10);
		RenderCheckPoints(true);
		int nSelection=m_Render.m_piRender->EndSelection();
		if(nSelection>=SELECT_CHECKPOINT_BASE && nSelection<(int)(SELECT_CHECKPOINT_BASE+m_vCheckPoints.size()))
		{
			m_nSelectedCheckPoint=nSelection-SELECT_CHECKPOINT_BASE;
			m_vPlayMovementPosition=m_vCheckPoints[m_nSelectedCheckPoint]->m_piPlayAreaCheckPoint->GetCheckPointPosition();
		}
	}
	REL(piCamera);
}

void CScenarioEditorMainWindow::OnMouseDown( int nButton,double dx,double dy )
{
	IGenericCamera *piCamera=NULL;
	if(!m_bSimulationStarted ||m_bInspectionMode)
	{
		piCamera=ADD(m_Camera.m_piCamera);
	}
	else
	{
		piCamera=m_PlayAreaManagerWrapper.m_piPlayAreaManager?m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCamera():ADD(m_Camera.m_piCamera);
	}
	
	if(m_bRenderPlayArea)
	{
		int nPreviouslySelectedLanding=m_nSelectedLandingRoutePoint;
		int nPreviouslySelectedTakeOff=m_nSelectedTakeOffRoutePoint;
		int nPreviouslySelectedCheckPoint=m_nSelectedCheckPoint;
		m_nSelectedLandingRoutePoint=-1;
		m_nSelectedTakeOffRoutePoint=-1;
		m_nSelectedCheckPoint=-1;
		
		m_Render.m_piRender->StartSelection(m_rRealRect,piCamera,dx,dy,10);
		RenderPlayerRoutes(true);
		RenderCheckPoints(true);
		int nSelection=m_Render.m_piRender->EndSelection();
		if(nSelection>=SELECT_LANDING_BASE && nSelection<SELECT_LANDING_BASE+4)
		{
			m_nSelectedLandingRoutePoint=nSelection-SELECT_LANDING_BASE;
			
			if(nPreviouslySelectedLanding==m_nSelectedLandingRoutePoint &&
			   DetectDrag(dx,dy))
			{
				m_bMovingPlayerRoutePoint=true;
				m_piGUIManager->SetMouseCapture(this);
				
				SPlayerConfig sPlayerConfig;
				if(m_PlayerManagerWrapper.m_piPlayerManager){m_PlayerManagerWrapper.m_piPlayerManager->GetPlayerConfig(&sPlayerConfig);}
				m_vObjectOriginalPosition=sPlayerConfig.pvPlayerLandingPoints[m_nSelectedLandingRoutePoint];
				
				if(m_nSelectedLandingRoutePoint<2)
				{
					GetAirPlaneCoordinatesFromCursorPos(dx,dy,&m_vCursorOriginalPosition);
				}
				else
				{
					GetTerrainCoordinatesFromCursorPos(dx,dy,false,&m_vCursorOriginalPosition);
				}
				
			}
			return;
		}
		else if(nSelection>=SELECT_TAKEOFF_BASE && nSelection<SELECT_TAKEOFF_BASE+4)
		{
			m_nSelectedTakeOffRoutePoint=nSelection-SELECT_TAKEOFF_BASE;
			
			if(nPreviouslySelectedTakeOff==m_nSelectedTakeOffRoutePoint&&
				DetectDrag(dx,dy))
			{
				m_bMovingPlayerRoutePoint=true;
				m_piGUIManager->SetMouseCapture(this);
				
				SPlayerConfig sPlayerConfig;
				if(m_PlayerManagerWrapper.m_piPlayerManager){m_PlayerManagerWrapper.m_piPlayerManager->GetPlayerConfig(&sPlayerConfig);}
				m_vObjectOriginalPosition=sPlayerConfig.pvPlayerTakeOffPoints[m_nSelectedTakeOffRoutePoint];
				
				if(m_nSelectedTakeOffRoutePoint>=2)
				{
					GetAirPlaneCoordinatesFromCursorPos(dx,dy,&m_vCursorOriginalPosition);
				}
				else
				{
					GetTerrainCoordinatesFromCursorPos(dx,dy,false,&m_vCursorOriginalPosition);
				}
			}
			return;
		}
		else if(nSelection>=SELECT_CHECKPOINT_BASE && nSelection<(int)(SELECT_CHECKPOINT_BASE+m_vCheckPoints.size()))
		{
			m_nSelectedCheckPoint=nSelection-SELECT_CHECKPOINT_BASE;
			
			if(nPreviouslySelectedCheckPoint==m_nSelectedCheckPoint&&
				DetectDrag(dx,dy))
			{
				m_bMovingCheckPoint=true;
				m_piGUIManager->SetMouseCapture(this);
				m_vObjectOriginalPosition=m_vCheckPoints[m_nSelectedCheckPoint]->m_piPlayAreaCheckPoint->GetCheckPointPosition();
				
				if(m_nSelectedTakeOffRoutePoint>=2)
				{
					GetAirPlaneCoordinatesFromCursorPos(dx,dy,&m_vCursorOriginalPosition);
				}
				else
				{
					GetTerrainCoordinatesFromCursorPos(dx,dy,false,&m_vCursorOriginalPosition);
				}
			}
			return;
		}
		m_Render.m_piRender->StartSelection(m_rRealRect,piCamera,dx,dy,5);
		RenderPlayArea(true);
		nSelection=m_Render.m_piRender->EndSelection();
		if(nSelection==SELECT_PLAY_AREA_INDEX)
		{
			if(DetectDrag(dx,dy))
			{
				m_nSelectedFormation=-1;
				m_nSelectedEntity=-1;
				if(m_piLSEntityList){m_piLSEntityList->SetSelectedElement(m_nSelectedEntity);}
				if(m_piLSFormationList){m_piLSFormationList->SetSelectedElement(m_nSelectedFormation);}
				
				m_bMovingCameraPosition=true;
				m_piGUIManager->SetMouseCapture(this);
				
				m_vObjectOriginalPosition=m_vPlayMovementPosition;
				GetAirPlaneCoordinatesFromCursorPos(dx,dy,&m_vCursorOriginalPosition);
			}
			return;
		}
	}
	if(m_nSelectedEntity!=-1)
	{
		if(nButton==GK_LBUTTON)
		{
			m_Render.m_piRender->StartSelection(m_rRealRect,piCamera,dx,dy,10);
			for(unsigned x=0;x<m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetRoutePoints();x++)
			{
				SRoutePoint point;
				m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetRoutePoint(x,&point);
				m_Render.m_piRender->SetSelectionId(x);
				m_Render.m_piRender->RenderPoint(point.vPosition,(int)x==m_nSelectedRoutePoint?10:5,CVector(0.8,0.8,0.8),1.0);
			}
			int nNewSelection=m_Render.m_piRender->EndSelection();
			if(m_nSelectedRoutePoint==nNewSelection)
			{
				if(nNewSelection!=-1)
				{
					if(DetectDrag(dx,dy))
					{
						m_bMovingRoutePoint=true;
						m_piGUIManager->SetMouseCapture(this);
						SRoutePoint point;
						m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetRoutePoint(nNewSelection,&point);
						m_vObjectOriginalPosition=point.vPosition;
						GetTerrainCoordinatesFromCursorPos(dx,dy,false,&m_vCursorOriginalPosition);
					}
				}
			}
			m_nSelectedRoutePoint=nNewSelection;
			if(m_nSelectedRoutePoint!=-1)
			{
				return;
			}
		}
	}

	if(m_vEntityControls.size()==0 && m_vFormationControls.size()==0){return;}

	m_Render.m_piRender->StartSelection(m_rRealRect,piCamera,dx,dy,5);

	for(unsigned int x=0;x<m_vEntityControls.size();x++)
	{
		m_Render.m_piRender->SetSelectionId(SELECT_ENTITY_BASE_INDEX+x);
		SEntityControls *pEntity=m_vEntityControls[x];
		pEntity->m_piPlayAreaEntity->DesignRender(m_Render.m_piRender,false);
	}
	for(unsigned int x=0;x<m_vFormationControls.size();x++)
	{
		m_Render.m_piRender->SetSelectionId(SELECT_FORMATION_BASE_INDEX+x);
		SFormationControls *pFormation=m_vFormationControls[x];
		pFormation->m_piPlayAreaFormation->DesignRender(m_Render.m_piRender,false);
	}
	int nNewIndex=m_Render.m_piRender->EndSelection();
	if(nNewIndex==-1)
	{
		m_nSelectedFormation=-1;
		m_nSelectedEntity=-1;
		if(m_piLSEntityList){m_piLSEntityList->SetSelectedElement(m_nSelectedEntity);}
		if(m_piLSFormationList){m_piLSFormationList->SetSelectedElement(m_nSelectedFormation);}
		
		m_bMovingCameraPosition=false;
	}
	else if(nNewIndex<SELECT_FORMATION_BASE_INDEX)
	{
		int nNewSelection=nNewIndex-SELECT_ENTITY_BASE_INDEX;
		if(m_nSelectedEntity==nNewSelection)
		{
			if(nNewSelection!=-1)
			{
				if(DetectDrag(dx,dy))
				{
					m_bMovingObject=true;
					m_piGUIManager->SetMouseCapture(this);
					m_vObjectOriginalPosition=m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetPosition();
					GetTerrainCoordinatesFromCursorPos(dx,dy,false,&m_vCursorOriginalPosition);
				}
			}

		}
		else
		{
			m_nSelectedEntity=nNewSelection;
			if(nNewSelection!=-1)
			{
				m_nSelectedEntityLayer=-1;
				m_nSelectedFormation=-1;
				m_bShowFormationPanel=false;
				m_bShowFormationsPanel=false;
				m_bShowEntityPanel=true;
				m_bShowEntitiesPanel=true;
				m_bShowTerrainPanel=false;
			}
			if(m_piLSEntityList){m_piLSEntityList->SetSelectedElement(m_nSelectedEntity);}
			if(m_piLSFormationList){m_piLSFormationList->SetSelectedElement(m_nSelectedFormation);}
		}
	}
	else
	{
		int nNewSelection=nNewIndex-SELECT_FORMATION_BASE_INDEX;
		if(m_nSelectedFormation==nNewSelection)
		{
			if(nNewSelection!=-1)
			{
				if(DetectDrag(dx,dy))
				{
					m_bMovingObject=true;
					m_piGUIManager->SetMouseCapture(this);
					m_vObjectOriginalPosition=m_vFormationControls[m_nSelectedFormation]->m_piPlayAreaFormation->GetPosition();
					GetAirPlaneCoordinatesFromCursorPos(dx,dy,&m_vCursorOriginalPosition);
				}
			}

		}
		else
		{
			m_nSelectedFormation=nNewSelection;
			if(nNewSelection!=-1)
			{
				m_nSelectedEntityLayer=-1;
				m_nSelectedEntity=-1;
				m_bShowFormationPanel=true;
				m_bShowFormationsPanel=true;
				m_bShowEntityPanel=false;
				m_bShowEntitiesPanel=false;
				m_bShowTerrainPanel=false;
			}
			if(m_piLSEntityList){m_piLSEntityList->SetSelectedElement(m_nSelectedEntity);}
			if(m_piLSFormationList){m_piLSFormationList->SetSelectedElement(m_nSelectedFormation);}
		}
	}

	REL(piCamera);
}

bool CScenarioEditorMainWindow::GetHeightAt(CVector vPoint,bool bIgnoreTerrainObjects, double *pdHeight)
{
	if(m_WorldManagerWrapper.m_piTerrain)
	{
		return m_WorldManagerWrapper.m_piTerrain->GetTerrainHeightAt(vPoint,pdHeight);
	}
	return false;
}

bool CScenarioEditorMainWindow::GetAirPlaneCoordinatesFromCursorPos(double x,double y,CVector *pAirPlanePos)
{
	IGenericCamera *piCamera=NULL;
	if(!m_bSimulationStarted ||m_bInspectionMode)
	{
		piCamera=ADD(m_Camera.m_piCamera);
	}
	else
	{
		piCamera=m_PlayAreaManagerWrapper.m_piPlayAreaManager?m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCamera():ADD(m_Camera.m_piCamera);
	}
	CLine mouseRay=GetMouseRay(x,y,10000.0,piCamera);

	double dAirPlaneHeight=GetAirPlaneAbsoluteHeight();

	CPlane airPlane(AxisPosY,CVector(0,dAirPlaneHeight,0));
	double dSide1=airPlane.GetSide(mouseRay.m_Points[0]);
	double dSide2=airPlane.GetSide(mouseRay.m_Points[1]);
	double dLength=(dSide1-dSide2);
	double dFraction=dLength?dSide1/dLength:0;
	CVector vPos=mouseRay.m_Points[0]+(mouseRay.m_Points[1]-mouseRay.m_Points[0])*dFraction;

	if(m_PlayAreaManagerWrapper.m_piPlayAreaManager)
	{
		CVector vPlayerRouteStart,vPlayerRouteEnd;
		CVector vPlayAreaMins,vPlayAreaMaxs;
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&vPlayerRouteStart,&vPlayerRouteEnd);
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayAreaPlaneAt(vPos,&vPlayAreaMins,&vPlayAreaMaxs);
		// Center the coordinate in the player route
		vPos.c[2]=vPlayerRouteStart.c[2];
	}


	if(pAirPlanePos){*pAirPlanePos=vPos;}

	REL(piCamera);
	return (dSide1*dSide2)<0;
}

bool CScenarioEditorMainWindow::GetTerrainCoordinatesFromLine(CLine line,bool bIgnoreTerrainObjects, CVector *pTerrainPos)
{
	if(m_WorldManagerWrapper.m_piTerrain)
	{
		bool bHit=m_WorldManagerWrapper.m_piTerrain->GetTerrainTrace(line.m_Points[0],line.m_Points[1],pTerrainPos);
		if(!bIgnoreTerrainObjects)
		{
			for(unsigned int x=0;x<m_vEntityControls.size();x++)
			{
				if((int)x==m_nSelectedEntity){continue;}
				CVector vPos=m_vEntityControls[x]->m_piPlayAreaEntity->GetPosition();
				CVector vAngles=m_vEntityControls[x]->m_piPlayAreaEntity->GetAngles();
				CTraceInfo info=m_vEntityControls[x]->m_piDesignObject->DesignGetTrace(vPos,vAngles,line.m_Points[0],line.m_Points[1]);
				if(info.m_bTraceHit)
				{
					if((info.m_vTracePos-line.m_Points[0])<(*pTerrainPos-line.m_Points[0]))
					{
						*pTerrainPos=info.m_vTracePos;
					}
				}
			}
		}
		pTerrainPos->c[1]+=FP_PRECISION;
		return bHit;
	}
	return false;
}

bool CScenarioEditorMainWindow::GetTerrainCoordinatesFromCursorPos(double x,double y,bool bIgnoreTerrainObjects, CVector *pTerrainPos)
{
	IGenericCamera *piCamera=NULL;
	if(!m_bSimulationStarted ||m_bInspectionMode)
	{
		piCamera=ADD(m_Camera.m_piCamera);
	}
	else
	{
		piCamera=m_PlayAreaManagerWrapper.m_piPlayAreaManager?m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCamera():ADD(m_Camera.m_piCamera);
	}
	CLine mouseRay=GetMouseRay(x,y,10000.0,piCamera);
	if(m_dMouseTraceDistance>0)
	{
		CVector vTerrainRef;
		if(GetTerrainCoordinatesFromLine(mouseRay,true,&vTerrainRef))
		{
			CVector vDir=mouseRay.m_Points[1]-mouseRay.m_Points[0];
			vDir.N();
			CVector vNewOrigin=vTerrainRef-vDir*m_dMouseTraceDistance;
			mouseRay.m_Points[0]=vNewOrigin;
		}
	}
	REL(piCamera);
	return GetTerrainCoordinatesFromLine(mouseRay,bIgnoreTerrainObjects,pTerrainPos);
}

void CScenarioEditorMainWindow::OnMouseMove( double x,double y )
{
	if(m_bMovingRoutePoint && m_nSelectedRoutePoint!=-1)
	{
		SEntityControls *pObject=m_vEntityControls[m_nSelectedEntity];
		CVector vTemp;
		if(GetTerrainCoordinatesFromCursorPos(x,y,false,&vTemp))
		{
			// Auto adjust route point
			
			CVector vAdjustRef=pObject->m_piPlayAreaEntity->GetPosition();
			for(int c=0;c<3;c++)
			{
				if(fabs(vAdjustRef.c[c]-vTemp.c[c])<2.0){vTemp.c[c]=vAdjustRef.c[c];}
			}
			
			for(unsigned int x=0;x<pObject->m_piPlayAreaEntity->GetRoutePoints();x++)
			{
				if(x==(unsigned int)m_nSelectedRoutePoint){continue;}
				
				SRoutePoint refpoint;
				pObject->m_piPlayAreaEntity->GetRoutePoint(x,&refpoint);
				for(int c=0;c<3;c++)
				{
					if(fabs(refpoint.vPosition.c[c]-vTemp.c[c])<2.0){vTemp.c[c]=refpoint.vPosition.c[c];}
				}
			}
			
			SRoutePoint point;
			pObject->m_piPlayAreaEntity->GetRoutePoint(m_nSelectedRoutePoint,&point);
			point.vPosition=vTemp;
			point.bAbsolutePoint=true;
			pObject->m_piPlayAreaEntity->SetRoutePoint(m_nSelectedRoutePoint,point);
		}
	}
	else if(m_bMovingCheckPoint && m_nSelectedCheckPoint!=-1)
	{
		CVector vTemp;
		if(GetAirPlaneCoordinatesFromCursorPos(x,y,&vTemp))
		{
			if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign)
			{
				CVector vPlayerRouteStart,vPlayerRouteEnd;
				m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&vPlayerRouteStart,&vPlayerRouteEnd);
				vTemp.c[1]=vPlayerRouteStart.c[1];
				vTemp.c[2]=vPlayerRouteStart.c[2];
			}
			m_vCheckPoints[m_nSelectedCheckPoint]->m_piPlayAreaCheckPoint->SetCheckPointPosition(vTemp);
		}
	}
	else if(m_bMovingPlayerRoutePoint && m_nSelectedLandingRoutePoint!=-1)
	{
		CVector vStart,vEnd;
		SPlayerConfig sPlayerConfig;
		if(m_PlayAreaManagerWrapper.m_piPlayAreaManager){m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&vStart,&vEnd);}
		if(m_PlayerManagerWrapper.m_piPlayerManager){m_PlayerManagerWrapper.m_piPlayerManager->GetPlayerConfig(&sPlayerConfig);}
		if(m_nSelectedLandingRoutePoint<2)
		{
			CVector vTemp;
			if(GetAirPlaneCoordinatesFromCursorPos(x,y,&vTemp))
			{
				vTemp.c[2]=vStart.c[2];
				sPlayerConfig.pvPlayerLandingPoints[m_nSelectedLandingRoutePoint]=vTemp;
			}
		}
		else
		{
			CVector vTemp;
			if(GetTerrainCoordinatesFromCursorPos(x,y,false,&vTemp))
			{
				vTemp.c[2]=vStart.c[2];
				vTemp.c[1]+=3;
				sPlayerConfig.pvPlayerLandingPoints[m_nSelectedLandingRoutePoint]=vTemp;
			}
		}
		if(m_PlayerManagerWrapper.m_piPlayerManager){m_PlayerManagerWrapper.m_piPlayerManager->SetPlayerConfig(&sPlayerConfig);}
	}
	else if(m_bMovingPlayerRoutePoint && m_nSelectedTakeOffRoutePoint!=-1)
	{
		CVector vStart,vEnd;
		SPlayerConfig sPlayerConfig;
		if(m_PlayAreaManagerWrapper.m_piPlayAreaManager){m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&vStart,&vEnd);}
		if(m_PlayerManagerWrapper.m_piPlayerManager){m_PlayerManagerWrapper.m_piPlayerManager->GetPlayerConfig(&sPlayerConfig);}
		if(m_nSelectedTakeOffRoutePoint>=2)
		{
			CVector vTemp;
			if(GetAirPlaneCoordinatesFromCursorPos(x,y,&vTemp))
			{
				vTemp.c[2]=vStart.c[2];
				sPlayerConfig.pvPlayerTakeOffPoints[m_nSelectedTakeOffRoutePoint]=vTemp;
			}
		}
		else
		{
			CVector vTemp;
			if(GetTerrainCoordinatesFromCursorPos(x,y,false,&vTemp))
			{
				vTemp.c[2]=vStart.c[2];
				vTemp.c[1]+=3;
				sPlayerConfig.pvPlayerTakeOffPoints[m_nSelectedTakeOffRoutePoint]=vTemp;
			}
		}
		if(m_PlayerManagerWrapper.m_piPlayerManager){m_PlayerManagerWrapper.m_piPlayerManager->SetPlayerConfig(&sPlayerConfig);}
	}
	else if(m_bMovingObject && m_nSelectedEntity!=-1)
	{
		SEntityControls *pObject=m_vEntityControls[m_nSelectedEntity];
		CVector vTemp;
		if(GetTerrainCoordinatesFromCursorPos(x,y,false,&vTemp))
		{
			pObject->m_piPlayAreaEntity->SetPosition(vTemp);
		}
	}
	else if(m_bMovingObject && m_nSelectedFormation!=-1)
	{
		SFormationControls *pObject=m_vFormationControls[m_nSelectedFormation];
		CVector vTemp;
		if(GetAirPlaneCoordinatesFromCursorPos(x,y,&vTemp))
		{
		  CVector vPosition=m_vObjectOriginalPosition+(vTemp-m_vCursorOriginalPosition);
		  pObject->m_piPlayAreaFormation->SetPosition(vPosition);
		}
	}
	else if(m_bMovingCameraPosition)
	{
		CVector vTemp;
		if(GetAirPlaneCoordinatesFromCursorPos(x,y,&vTemp))
		{
			CVector vStart,vEnd;
			m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&vStart,&vEnd);
			m_vPlayMovementPosition=m_vObjectOriginalPosition+(vTemp-m_vCursorOriginalPosition);
			if(m_vPlayMovementPosition.c[0]<vStart.c[0]){m_vPlayMovementPosition=vStart;}
			if(m_vPlayMovementPosition.c[0]>vEnd.c[0]){m_vPlayMovementPosition=vEnd;}
			m_PlayAreaManagerWrapper.m_piPlayAreaManager->SetPlayMovementPosition(m_vPlayMovementPosition);
		}
	}
}

void CScenarioEditorMainWindow::OnMouseUp( int nButton,double x,double y )
{
	if(m_piGUIManager->HasMouseCapture(this)){m_piGUIManager->ReleaseMouseCapture();}
	m_bMovingObject=false;
	m_bMovingRoutePoint=false;
	m_bMovingPlayerRoutePoint=false;
	m_bMovingCameraPosition=false;
	m_bMovingCheckPoint=false;
}

void CScenarioEditorMainWindow::StopGameSimulation()
{
	if(m_bSimulationStarted)
	{
		m_GameControllerWrapper.m_piGameController->Stop();
		m_bSimulationStarted=false;
	}
}

void CScenarioEditorMainWindow::StartGameSimulation()
{
	if(m_bSimulationStarted)
	{
		StopGameSimulation();
	}
	if(m_FrameManager.m_piFrameManager){m_FrameManager.m_piFrameManager->Reset();}
	if(m_PlayerManagerWrapper.m_piPlayerManager)
	{
		m_PlayerManagerWrapper.m_piPlayerManager->SetPlayerStart(m_vPlayMovementPosition);
	}
	m_GameControllerWrapper.m_piGameController->Start();
	if(m_PlayerManagerWrapper.m_piPlayerManager)
	{
		IEntity *piPlayerEntity=m_EntityManagerWrapper.m_piEntityManager->FindEntity("Player");
		IWeapon *piBombWeapon=piPlayerEntity?piPlayerEntity->GetWeapon(1):NULL;
		IWeapon *piBulletWeapon=piPlayerEntity?piPlayerEntity->GetWeapon(0):NULL;
		IPlayer *piPlayer=dynamic_cast<IPlayer*>(piPlayerEntity);
		if(piPlayer){piPlayer->SetGodMode(m_bGodMode);}
		if(piBombWeapon){piBombWeapon->SetAmmo(m_bUseAmmo?3:10000);}
		if(piBulletWeapon){piBulletWeapon->SetCurrentLevel(m_nStartingWeapon-1);}
	}

	m_bSimulationStarted=true;
}

void CScenarioEditorMainWindow::UpdateEntityLayerControls()
{
	unsigned int x;
	std::vector<SEntityLayerControls *>::iterator i;
	for(x=0;x<m_vEntityLayerControls.size();x++)
	{
		SEntityLayerControls *pControls=m_vEntityLayerControls[x];
		UNSUBSCRIBE_FROM_CAST(pControls->m_BTListRow.m_piButton,IGameGUIButtonEvents);
		pControls->m_BTListRow.m_piButton->DestroyWindow();
		pControls->m_STDescription.m_piLabel->DestroyWindow();
		pControls->m_STEntity.m_piLabel->DestroyWindow();
		delete pControls;
		pControls=NULL;
	}
	m_vEntityLayerControls.clear();
	if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign)
	{
		for(x=0;x<m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetEntityLayers();x++)
		{
			SEntityLayer sLayer;
			IEntityType *piEntityType=NULL;
			m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetEntityLayer(x,&sLayer,&piEntityType);
			IDesignObject *piDesignObject=QI(IDesignObject,piEntityType);

			SEntityLayerControls *pControls=new SEntityLayerControls;
			pControls->m_BTListRow.Create(m_piSystem,"CGameGUIButton","");
			if(pControls->m_BTListRow.m_piButton)
			{
				pControls->m_BTListRow.m_piButton->InitWindow(m_piGREntityLayerList,false);
				pControls->m_BTListRow.m_piButton->SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
				pControls->m_BTListRow.m_piButton->SetBackgroundColor(CVector(1,1,1),0.1);
				pControls->m_BTListRow.m_piButton->Show(true);
				pControls->m_BTListRow.m_piButton->Activate(true);
				SUBSCRIBE_TO_CAST(pControls->m_BTListRow.m_piButton,IGameGUIButtonEvents);

				pControls->m_STEntity.Create(m_piSystem,"CScenarioEditorObjectLabel","");
				if(pControls->m_STEntity.m_piLabel)
				{
					pControls->m_STEntity.m_piLabel->InitWindow(pControls->m_BTListRow.m_piButton,false);
					pControls->m_STEntity.m_piLabel->SetObject(piDesignObject);
					pControls->m_STEntity.m_piLabel->SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
					pControls->m_STEntity.m_piLabel->SetBackgroundColor(CVector(0,0,0),1);
					pControls->m_STEntity.m_piLabel->Show(true);
					pControls->m_STEntity.m_piLabel->Activate(false);
				}
				pControls->m_STDescription.Create(m_piSystem,"CGameGUILabel","");
				if(pControls->m_STDescription.m_piLabel)
				{
					pControls->m_STDescription.m_piLabel->InitWindow(pControls->m_BTListRow.m_piButton,false);
					pControls->m_STDescription.m_piLabel->SetTextColor(CVector(1,1,1),0.1);
					pControls->m_STDescription.m_piLabel->SetHorizontalAlignment(eTextAlignment_Center);
					pControls->m_STDescription.m_piLabel->SetVerticalAlignment(eTextAlignment_Center);
					pControls->m_STDescription.m_piLabel->SetText("Model");
					pControls->m_STDescription.m_piLabel->SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
					pControls->m_STDescription.m_piLabel->Show(true);
					pControls->m_STDescription.m_piLabel->Activate(false);
				}
			}
			REL(piEntityType);
			REL(piDesignObject);
			m_vEntityLayerControls.push_back(pControls);
		}
	}
	if(m_nSelectedEntityLayer!=-1 && m_nSelectedEntityLayer>=(int)m_vEntityLayerControls.size())
	{
		m_nSelectedEntityLayer=(int)(m_vEntityLayerControls.size()-1);
	}
}

void CScenarioEditorMainWindow::UpdateEntityControls()
{
	unsigned int x;
	std::vector<SEntityControls *>::iterator i;
	for(x=0;x<m_vEntityControls.size();x++)
	{
		SEntityControls *pControls=m_vEntityControls[x];
		delete pControls;
		pControls=NULL;
	}
	m_piLSEntityList->Clear();
	m_vEntityControls.clear();
	if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign)
	{
		for(x=0;x<m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetElements();x++)
		{
			IPlayAreaElement *piElement=NULL;
			m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetElement(x,&piElement);
			IPlayAreaEntity  *piEntity=QI(IPlayAreaEntity,piElement);
			IEntityType		 *piEntityType=NULL;
			IEntityType		 *piBonusType=NULL;
			if(piEntity){piEntity->GetEntityType(&piEntityType);}			
			if(piEntity){piEntity->GetBonusType(&piBonusType);}			
			IDesignObject *piDesignObject=QI(IDesignObject,piEntityType);
			IDesignObject *piBonusDesignObject=QI(IDesignObject,piBonusType);
			ISystemObject *piObject=QI(ISystemObject,piDesignObject);
			if(!piObject)
			{
				REL(piEntityType);
				REL(piDesignObject);
				REL(piBonusType);
				REL(piEntity);
				REL(piElement);
				REL(piBonusDesignObject);
				continue;
			}

			m_piLSEntityList->AddElement(piObject->GetName());
			SEntityControls *pControls=new SEntityControls;
			pControls->m_nPlayAreaElementId=x;
			pControls->m_piEntityType=ADD(piEntityType);
			pControls->m_piBonusDesignObject=ADD(piBonusDesignObject);
			pControls->m_piDesignObject=ADD(piDesignObject);
			pControls->m_piPlayAreaEntity=ADD(piEntity);
			pControls->m_piObject=QI(ISystemObject,piEntityType);
			REL(piEntityType);
			REL(piDesignObject);
			REL(piBonusType);
			REL(piBonusDesignObject);
			REL(piElement);
			REL(piEntity);
			REL(piObject);
			m_vEntityControls.push_back(pControls);
		}
	}
	if(m_nSelectedEntity!=-1 && m_nSelectedEntity>=(int)m_vEntityControls.size())
	{
		m_nSelectedEntity=(int)(m_vEntityControls.size()-1);
		if(m_piLSEntityList){m_piLSEntityList->SetSelectedElement(m_nSelectedEntity);}
	}
}

void CScenarioEditorMainWindow::UpdateFormationControls()
{
	unsigned int x;
	std::vector<SFormationControls *>::iterator i;
	for(x=0;x<m_vFormationControls.size();x++)
	{
		SFormationControls *pControls=m_vFormationControls[x];
		delete pControls;
		pControls=NULL;
	}
	
	m_piLSFormationList->Clear();
	m_vFormationControls.clear();
	if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign)
	{
		for(x=0;x<m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetElements();x++)
		{
			IPlayAreaElement *piElement=NULL;
			m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetElement(x,&piElement);
			IPlayAreaFormation  *piFormation=QI(IPlayAreaFormation,piElement);
			IFormationType		*piFormationType=NULL,*piAlternativeFormationType=NULL;
			IEntityType			*piBonusType=NULL,*piAlternativeBonusType=NULL;
			if(piFormation){piFormation->GetFormationType(&piFormationType);}			
			if(piFormation){piFormation->GetAlternativeFormationType(&piAlternativeFormationType);}			
			if(piFormation){piFormation->GetBonusType(&piBonusType);}			
			if(piFormation){piFormation->GetAlternativeBonusType(&piAlternativeBonusType);}			
			IDesignObject *piBonusDesignObject=QI(IDesignObject,piBonusType);
			IDesignObject *piDesignObject=QI(IDesignObject,piFormationType);
			IDesignObject *piAlternativeBonusDesignObject=QI(IDesignObject,piAlternativeBonusType);
			IDesignObject *piAlternativeDesignObject=QI(IDesignObject,piAlternativeFormationType);
			ISystemObject *piObject=QI(ISystemObject,piDesignObject);
			if(!piObject)
			{
				REL(piBonusType);
				REL(piBonusDesignObject);
				REL(piFormationType);
				REL(piFormation);
				REL(piElement);
				REL(piDesignObject);
				REL(piAlternativeBonusType);
				REL(piAlternativeBonusDesignObject);
				REL(piAlternativeFormationType);
				REL(piAlternativeDesignObject);
				continue;
			}

			m_piLSFormationList->AddElement(piObject->GetName());
			SFormationControls *pControls=new SFormationControls;
			pControls->m_nPlayAreaElementId=x;
			pControls->m_piFormationType=ADD(piFormationType);
			pControls->m_piDesignObject=ADD(piDesignObject);
			pControls->m_piAlternativeDesignObject=ADD(piAlternativeDesignObject);
			pControls->m_piPlayAreaFormation=ADD(piFormation);
			pControls->m_piObject=QI(ISystemObject,piFormationType);
			pControls->m_piBonusDesignObject=ADD(piBonusDesignObject);
			pControls->m_piAlternativeBonusDesignObject=ADD(piAlternativeBonusDesignObject);
			REL(piBonusType);
			REL(piBonusDesignObject);
			REL(piFormationType);
			REL(piDesignObject);
			REL(piElement);
			REL(piFormation);
			REL(piObject);
			REL(piAlternativeBonusType);
			REL(piAlternativeBonusDesignObject);
			REL(piAlternativeFormationType);
			REL(piAlternativeDesignObject);

			m_vFormationControls.push_back(pControls);
		}
	}
	if(m_nSelectedFormation!=-1 && m_nSelectedFormation>=(int)m_vFormationControls.size())
	{
		m_nSelectedFormation=(int)(m_vFormationControls.size()-1);
		m_nSelectedEntity=-1;
		m_nSelectedEntityLayer=-1;
		if(m_piLSEntityList){m_piLSEntityList->SetSelectedElement(m_nSelectedEntity);}
		if(m_piLSFormationList){m_piLSFormationList->SetSelectedElement(m_nSelectedFormation);}
	}
}

void CScenarioEditorMainWindow::UpdateColorLayerControls()
{
	unsigned int x;
	std::vector<SColorLayerControls *>::iterator i;
	for(x=0;x<m_vColorLayerControls.size();x++)
	{
		SColorLayerControls *pControls=m_vColorLayerControls[x];
		UNSUBSCRIBE_FROM_CAST(pControls->m_BTListRow.m_piButton,IGameGUIButtonEvents);
		pControls->m_BTListRow.m_piButton->DestroyWindow();
		pControls->m_STDescription.m_piLabel->DestroyWindow();
		pControls->m_STTexture.m_piLabel->DestroyWindow();
		delete pControls;
		pControls=NULL;
	}
	m_vColorLayerControls.clear();

	if(m_WorldManagerWrapper.m_piTerrain)
	{
		for(x=0;x<m_WorldManagerWrapper.m_piTerrain->GetTerrainColorLayers();x++)
		{
			STerrainColorLayer sLayer;
			m_WorldManagerWrapper.m_piTerrain->GetTerrainColorLayer(x,&sLayer,NULL,NULL);

			SColorLayerControls *pControls=new SColorLayerControls;
			pControls->m_BTListRow.Create(m_piSystem,"CGameGUIButton","");
			if(pControls->m_BTListRow.m_piButton)
			{
				pControls->m_BTListRow.m_piButton->InitWindow(m_piGRLayerList,false);
				pControls->m_BTListRow.m_piButton->SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
				pControls->m_BTListRow.m_piButton->SetBackgroundColor(CVector(1,1,1),0.1);
				pControls->m_BTListRow.m_piButton->Show(true);
				pControls->m_BTListRow.m_piButton->Activate(true);
				SUBSCRIBE_TO_CAST(pControls->m_BTListRow.m_piButton,IGameGUIButtonEvents);

				pControls->m_STTexture.Create(m_piSystem,"CGameGUILabel","");
				if(pControls->m_STTexture.m_piLabel)
				{
					pControls->m_STTexture.m_piLabel->InitWindow(pControls->m_BTListRow.m_piButton,false);
					pControls->m_STTexture.m_piLabel->SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
					pControls->m_STTexture.m_piLabel->SetBackgroundColor(CVector(1,1,1),1);
					pControls->m_STTexture.m_piLabel->Show(true);
					pControls->m_STTexture.m_piLabel->Activate(false);
				}
				pControls->m_STDescription.Create(m_piSystem,"CGameGUILabel","");
				if(pControls->m_STDescription.m_piLabel)
				{
					pControls->m_STDescription.m_piLabel->InitWindow(pControls->m_BTListRow.m_piButton,false);
					pControls->m_STDescription.m_piLabel->SetTextColor(CVector(1,1,1),0.1);
					pControls->m_STDescription.m_piLabel->SetHorizontalAlignment(eTextAlignment_Center);
					pControls->m_STDescription.m_piLabel->SetVerticalAlignment(eTextAlignment_Center);
					pControls->m_STDescription.m_piLabel->SetText("Color");
					pControls->m_STDescription.m_piLabel->SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
					pControls->m_STDescription.m_piLabel->Show(true);
					pControls->m_STDescription.m_piLabel->Activate(false);
				}
			}
			m_vColorLayerControls.push_back(pControls);
		}
	}
	if(m_nSelectedColorLayer!=-1 && m_nSelectedColorLayer>=(int)m_vColorLayerControls.size())
	{
		m_nSelectedColorLayer=(int)(m_vColorLayerControls.size()-1);
	}
}

void CScenarioEditorMainWindow::UpdateHeightLayerControls()
{
	unsigned int x;
	std::vector<SHeightLayerControls *>::iterator i;
	for(x=0;x<m_vHeightLayerControls.size();x++)
	{
		SHeightLayerControls *pControls=m_vHeightLayerControls[x];
		UNSUBSCRIBE_FROM_CAST(pControls->m_BTListRow.m_piButton,IGameGUIButtonEvents);
		pControls->m_BTListRow.m_piButton->DestroyWindow();
		pControls->m_STDescription.m_piLabel->DestroyWindow();
		pControls->m_STTexture.m_piLabel->DestroyWindow();
		delete pControls;
		pControls=NULL;
	}
	m_vHeightLayerControls.clear();

	if(m_WorldManagerWrapper.m_piTerrain)
	{
		for(x=0;x<m_WorldManagerWrapper.m_piTerrain->GetTerrainHeightLayers();x++)
		{
			STerrainHeightLayer sLayer;
			m_WorldManagerWrapper.m_piTerrain->GetTerrainHeightLayer(x,&sLayer,NULL,NULL);
			SHeightLayerControls *pData=new SHeightLayerControls;
			pData->m_BTListRow.Create(m_piSystem,"CGameGUIButton","");
			if(pData->m_BTListRow.m_piButton)
			{
				pData->m_BTListRow.m_piButton->InitWindow(m_piGRLayerList,false);
				pData->m_BTListRow.m_piButton->SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
				pData->m_BTListRow.m_piButton->SetBackgroundColor(CVector(1,1,1),0.1);
				pData->m_BTListRow.m_piButton->Show(true);
				pData->m_BTListRow.m_piButton->Activate(true);
				SUBSCRIBE_TO_CAST(pData->m_BTListRow.m_piButton,IGameGUIButtonEvents);

				pData->m_STTexture.Create(m_piSystem,"CGameGUILabel","");
				if(pData->m_STTexture.m_piLabel)
				{
					pData->m_STTexture.m_piLabel->InitWindow(pData->m_BTListRow.m_piButton,false);
					pData->m_STTexture.m_piLabel->SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
					pData->m_STTexture.m_piLabel->SetBackgroundColor(CVector(1,1,1),1);
					pData->m_STTexture.m_piLabel->Show(true);
					pData->m_STTexture.m_piLabel->Activate(false);
				}
				pData->m_STDescription.Create(m_piSystem,"CGameGUILabel","");
				if(pData->m_STDescription.m_piLabel)
				{
					pData->m_STDescription.m_piLabel->InitWindow(pData->m_BTListRow.m_piButton,false);
					pData->m_STDescription.m_piLabel->SetTextColor(CVector(1,1,1),0.1);
					pData->m_STDescription.m_piLabel->SetHorizontalAlignment(eTextAlignment_Center);
					pData->m_STDescription.m_piLabel->SetVerticalAlignment(eTextAlignment_Center);
					pData->m_STDescription.m_piLabel->SetText("Height");
					pData->m_STDescription.m_piLabel->SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
					pData->m_STDescription.m_piLabel->Show(true);
					pData->m_STDescription.m_piLabel->Activate(false);
				}
			}
			m_vHeightLayerControls.push_back(pData);
		}
	}
	if(m_nSelectedHeightLayer!=-1 && m_nSelectedHeightLayer>=(int)m_vHeightLayerControls.size())
	{
		m_nSelectedHeightLayer=(int)(m_vHeightLayerControls.size()-1);
	}
}

void CScenarioEditorMainWindow::UpdateCheckPoints()
{
	unsigned int x;
	std::vector<SCheckPoint *>::iterator i;
	for(x=0;x<m_vCheckPoints.size();x++)
	{
		SCheckPoint *pCheckPoint=m_vCheckPoints[x];
		delete pCheckPoint;
		pCheckPoint=NULL;
	}
	m_vCheckPoints.clear();
	if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign)
	{
		for(x=0;x<m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetElements();x++)
		{
			IPlayAreaElement *piElement=NULL;
			m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetElement(x,&piElement);
			IPlayAreaCheckPoint  *piCheckPoint=QI(IPlayAreaCheckPoint,piElement);
			if(piCheckPoint)
			{
				SCheckPoint *pCheckPoint=new SCheckPoint;
				pCheckPoint->m_nPlayAreaElementId=x;
				pCheckPoint->m_piPlayAreaCheckPoint=ADD(piCheckPoint);
				m_vCheckPoints.push_back(pCheckPoint);
			}
			REL(piElement);
			REL(piCheckPoint);
		}
	}
}

void CScenarioEditorMainWindow::UpdateFormationsHeight()
{
	double dAirPlaneHeight=GetAirPlaneAbsoluteHeight();
	for(unsigned int x=0;x<m_vFormationControls.size();x++)
	{
		CVector vPos=m_vFormationControls[x]->m_piPlayAreaFormation->GetPosition();
		vPos.c[1]=dAirPlaneHeight;
		m_vFormationControls[x]->m_piPlayAreaFormation->SetPosition(vPos);
	}

}

double CScenarioEditorMainWindow::GetAirPlaneAbsoluteHeight()
{
	double dAirPlaneHeight=0;
	CVector vMins,vMaxs;
	m_WorldManagerWrapper.m_piTerrain->GetTerrainBBox(&vMins,&vMaxs);
	dAirPlaneHeight=vMaxs.c[1];
	
	SPlayAreaConfig sPlayAreaConfig;
	if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign)
	{
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetPlayAreaConfig(&sPlayAreaConfig);
		dAirPlaneHeight+=sPlayAreaConfig.dAirPlaneHeight;
	}
	return dAirPlaneHeight;
}

void CScenarioEditorMainWindow::OpenScenario( std::string sScenario )
{
	Reset();
	if(!m_GameControllerWrapper.m_piGameController->LoadScenario(sScenario))
	{
	  std::string sMessage=(std::string)"Failed to load '"+sScenario+"'";
	  
	  Reset();
	  if(m_GameControllerWrapper.m_piGameController){m_GameControllerWrapper.m_piGameController->CreateScenario();}

	  MessageDialog(sMessage,"Error",eMessageDialogType_Error);
	  return;
	}
	
	if(m_PlayAreaManagerWrapper.m_piPlayAreaManager)
	{
		CVector vStart,vEnd;
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&vStart,&vEnd);
		m_vPlayMovementPosition=vStart;
	}
	
	m_sFile=sScenario;
	UpdateColorLayerControls();
	UpdateHeightLayerControls();
	UpdateEntityLayerControls();
	UpdateEntityControls();
	UpdateFormationControls();
	UpdateCheckPoints();
	UpdateLayerPanel();
	UpdateCaption();
	CenterCamera();
}

void CScenarioEditorMainWindow::RenderRoute( IGenericRender * piRender, int nSelectedEntity, int nSelectedRoutePoint )
{
	if(m_nSelectedEntity==-1){return;}
	SEntityControls *pEntity=m_vEntityControls[nSelectedEntity];
	piRender->PushState();
	piRender->DeactivateDepth();
	piRender->DeactivateShadowReception();

	CVector vPreviousPoint=pEntity->m_piPlayAreaEntity->GetPosition();
	for(unsigned x=0;x<pEntity->m_piPlayAreaEntity->GetRoutePoints();x++)
	{
		SRoutePoint point;
		pEntity->m_piPlayAreaEntity->GetRoutePoint(x,&point);
		piRender->RenderLine(vPreviousPoint,point.vPosition,0x8888,CVector(0.8,0.8,0.8),1);
		piRender->RenderPoint(point.vPosition,nSelectedRoutePoint==(int)x?10:5,nSelectedRoutePoint==(int)x?CVector(0,0,0.8):CVector(0.8,0.8,0.8),1.0);
		vPreviousPoint=point.vPosition;
	}
	piRender->PopState();
}
void CScenarioEditorMainWindow::OnWantFocus(bool *pbWant){*pbWant=true;}

void CScenarioEditorMainWindow::OnColorChanged(CVector vColor)
{
	if(m_WorldManagerWrapper.m_piTerrain==NULL){return;}
	
	if(m_eColorCallBack==eColorCallBackFog)
	{
		STerrainFog sFog;
		m_WorldManagerWrapper.m_piTerrain->GetTerrainFog(&sFog);
		sFog.vColor=vColor;
		m_WorldManagerWrapper.m_piTerrain->SetTerrainFog(&sFog);
	}
	else if(m_eColorCallBack==eColorCallBackSun)
	{
		STerrainSun sSun;
		m_WorldManagerWrapper.m_piTerrain->GetTerrainSun(&sSun);
		sSun.vColor=vColor;
		m_WorldManagerWrapper.m_piTerrain->SetTerrainSun(&sSun);
	}
	else if(m_eColorCallBack==eColorCallBackAmbient)
	{
		m_WorldManagerWrapper.m_piTerrain->SetTerrainAmbientColor(vColor);
	}
}
