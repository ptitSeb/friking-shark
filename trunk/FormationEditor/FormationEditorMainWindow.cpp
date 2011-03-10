#include "./stdafx.h"
#include "resource.h"
#include "GameRunTimeLib.h"
#include "GameGUILib.h"
#include "FormationEditorMainWindow.h"

#define SELECT_FORMATION_BASE_INDEX 0x800

extern CSystemModuleHelper *g_pSystemModuleHelper;

CFormationEditorMainWindow::CFormationEditorMainWindow(void)
{
	m_nFormationId=0;
	m_bMovingRoutePoint=false;
	m_bRenderPlayArea=false;
	
	m_bInspectionMode=false;
	m_bShowOptionsPanel=false;
	m_bShowEntitiesPanel=false;
	m_bShowEntityPanel=false;
	m_bShowFilePanel=false;
	m_bShowPlayAreaPanel=false;
	m_nSelectedRoutePoint=-1;

	m_dwNexControlKey=0;
	m_bTextures=1;
	m_bColors=1;
	m_bSolid=1;
	m_bBlend=1;
	m_nSelectedEntity=-1;

	InitializeChildren();
	m_piGameSystem=NULL;
	m_bSimulationStarted=false;
}

CFormationEditorMainWindow::~CFormationEditorMainWindow(void)
{
}

bool CFormationEditorMainWindow::InitWindow(IGameWindow *piParent,bool bPopup)
{
	bool bOk=CGameWindowBase::InitWindow(piParent,bPopup);
	if(bOk)
	{
		m_ObjectSelector.Attach("GameGUI","ObjectSelector");
		m_Viewport.Attach("GameGUI","Viewport");
		m_Render.Attach("GameGUI","Render");
		m_Viewport.m_piViewport->SetCaption("Formation Editor");
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

	m_piSTEntityObjectLabel->Activate(false);

	UpdateLayerPanel();

	CSystemLoaderHelper     systemLoader;
	m_piGameSystem=systemLoader.LoadSystem("Scripts/GameSystem.cfg","GameSystem");
	m_GameControllerWrapper.Attach("GameSystem","GameController");
	if(m_GameControllerWrapper.m_piGameController)
	{
	  m_GameControllerWrapper.m_piGameController->SetupGame();
	  m_GameControllerWrapper.m_piGameController->CreateScenario();
	  m_FormationType.Create("FormationTypes","CFormationType","__unnamed__");
	}

	m_WorldManagerWrapper.Attach("GameSystem","WorldManager");
	m_GameRenderWrapper.Attach("GameSystem","GameRender");
	m_PlayAreaManagerWrapper.Attach("GameSystem","PlayAreaManager");
	m_FrameManager.Attach("GameSystem","FrameManager");
	m_SoundManagerWrapper.Attach("GameSystem","SoundManager");
	if(m_WorldManagerWrapper.m_piTerrain){m_WorldManagerWrapper.m_piTerrain->SetTerrainBaseModel("terreno.ASE");m_WorldManagerWrapper.m_piTerrain->UpdateTerrain();}
	if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign){m_PlayAreaManagerWrapper.m_piPlayAreaDesign->UpdatePlayArea();}
	CenterCamera();
	return bOk;
}

void CFormationEditorMainWindow::DestroyWindow()
{
	StopGameSimulation();
	Reset();
	if(m_GameControllerWrapper.m_piGameController){m_GameControllerWrapper.m_piGameController->EndGame();}
	if(m_GameControllerWrapper.m_piGameController){m_GameControllerWrapper.m_piGameController->CloseScenario();}
	m_SoundManagerWrapper.Detach();
	m_PlayAreaManagerWrapper.Detach();
	m_GameControllerWrapper.Detach();
	m_GameRenderWrapper.Detach();

	m_FrameManager.Detach();
	if(m_piGameSystem){m_piGameSystem->Destroy();}
	REL(m_piGameSystem);

	m_ObjectSelector.Detach();
	m_Viewport.Detach();
	m_Viewport.Detach();

	UNSUBSCRIBE_FROM_ALL();
	CGameWindowBase::DestroyWindow();
}

void CFormationEditorMainWindow::Reset()
{
	StopGameSimulation();
	if(m_piSTEntityObjectLabel){m_piSTEntityObjectLabel->SetObject(NULL);}
	m_FormationType.Detach();
	UpdateEntityControls();

	m_nSelectedEntity=-1;
	m_bShowFilePanel=false;
	m_bShowEntitiesPanel=false;
	m_bShowEntityPanel=false;
	m_bShowPlayAreaPanel=false;
	m_bShowOptionsPanel=false;
	m_bSimulationStarted=false;
	m_bInspectionMode=false;
	m_bMovingRoutePoint=false;
	m_nSelectedRoutePoint=-1;

	UpdateCaption();
}

void CFormationEditorMainWindow::ProcessInput(double dTimeFraction,double dRealTimeFraction)
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

void CFormationEditorMainWindow::ProcessKey(unsigned short nKey,double dTimeFraction,double dRealTimeFraction)
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
void CFormationEditorMainWindow::SetupRenderOptions(IGenericRender *piRender,IGenericCamera *piCamera)
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

void CFormationEditorMainWindow::OnDraw(IGenericRender *piRender)
{
	if(!m_FrameManager.m_piFrameManager)
	{
		return;
	}
	m_Camera.m_piCamera->SetAspectRatio(m_rRealRect.w/m_rRealRect.h);

	m_FrameManager.m_piFrameManager->ProcessFrame();
	ProcessInput(m_FrameManager.m_piFrameManager->GetTimeFraction(),m_FrameManager.m_piFrameManager->GetRealTimeFraction());
	
	m_Render.m_piRender->ActivateDepth();
	m_Render.m_piRender->PushOptions();
	m_Render.m_piRender->PushState();

	m_bTextures?m_Render.m_piRender->EnableTextures():m_Render.m_piRender->DisableTextures();
	m_bSolid?m_Render.m_piRender->EnableSolid():m_Render.m_piRender->DisableSolid();
	m_bBlend?m_Render.m_piRender->EnableBlending():m_Render.m_piRender->DisableBlending();
	m_Render.m_piRender->EnableShadows();
	m_Render.m_piRender->DisableLighting();
	m_Render.m_piRender->DisableHeightFog();
	m_Render.m_piRender->DisableShaders();
	
	if(m_bSimulationStarted)
	{
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->ProcessInput(m_piGUIManager);
		m_GameControllerWrapper.m_piGameController->ProcessFrame(m_FrameManager.m_piFrameManager->GetCurrentTime(),m_FrameManager.m_piFrameManager->GetTimeFraction());
		IGenericCamera *piPlayCamera=m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCamera();
		IGenericCamera *piCamera=NULL;
		piCamera=ADD(m_bInspectionMode?m_Camera.m_piCamera:piPlayCamera);
		if(piCamera)
		{
			SetupRenderOptions(piRender,piCamera);
			m_GameRenderWrapper.m_piRender->Render(piRender,piCamera);
		}
		REL(piCamera);
		REL(piPlayCamera);
	}
	else
	{
		SetupRenderOptions(piRender,m_Camera.m_piCamera);
		piRender->StartStagedRendering();
		m_WorldManagerWrapper.m_piTerrain->DesignRender(piRender);
		for(unsigned int x=0;x<m_vEntityControls.size();x++)
		{
			SRoutePoint point,point2;
			m_FormationType.m_piFormationTypeDesign->GetElementRoutePoint(x,0,&point);
			if(m_FormationType.m_piFormationTypeDesign->GetElementRoutePoints(x)>1)
			{
				m_FormationType.m_piFormationTypeDesign->GetElementRoutePoint(x,1,&point2);
			}
			else
			{
				point2.vPosition=point.vPosition+AxisPosY*0.1;
			}
			CVector vWorld=FormationToWorld(point.vPosition);
			CVector vWorld2=FormationToWorld(point2.vPosition);
			CVector vAngles=AnglesFromVector(vWorld2-vWorld);
			m_vEntityControls[x]->m_piDesignObject->DesignRender(piRender,vWorld,vAngles,false);
			if(m_nSelectedEntity==(int)x)
			{
				m_vEntityControls[x]->m_piDesignObject->DesignRender(piRender,vWorld,vAngles,true);
			}
			RenderRoute(piRender,x,m_nSelectedEntity,m_nSelectedRoutePoint);
		}
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->DesignRender(piRender);

		piRender->EndStagedRendering();
	}

	m_Render.m_piRender->PopOptions();
	m_Render.m_piRender->PopState();


	//Play Area
	m_Render.m_piRender->PushState();
	m_Render.m_piRender->ActivateBlending();
	CVector vPlayAreaMins,vPlayAreaMaxs;
	SPlayAreaConfig vPlayAreaConfig;
	m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetPlayAreaConfig(&vPlayAreaConfig);
	m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetAirPlayPlane(&vPlayAreaMins,&vPlayAreaMaxs);
	CVector vCenter=(vPlayAreaMaxs+vPlayAreaMins)*0.5;
	CVector vSize=(vPlayAreaMaxs-vPlayAreaMins);

	piRender->ActivateSolid();

	piRender->SetColor(CVector(1,1,1),0.1);
	piRender->RenderRect(vCenter,AxisPosX,AxisPosZ,vSize.c[0],vSize.c[2]);
	//Left scroll
	piRender->SetColor(CVector(1,1,1),0.05);
	piRender->RenderRect(vCenter-CVector(0,0,vSize.c[2]*0.5+vPlayAreaConfig.dCameraScroll*0.5),AxisPosX,AxisPosZ,vSize.c[0],vPlayAreaConfig.dCameraScroll);
	//Right scroll
	piRender->SetColor(CVector(1,1,1),0.05);
	piRender->RenderRect(vCenter+CVector(0,0,vSize.c[2]*0.5+vPlayAreaConfig.dCameraScroll*0.5),AxisPosX,AxisPosZ,vSize.c[0],vPlayAreaConfig.dCameraScroll);

	piRender->ActivateDepth();
	piRender->DeactivateSolid();

	piRender->SetColor(CVector(1,1,1),1.0);
	piRender->RenderRect(vCenter,AxisPosX,AxisPosZ,vSize.c[0],vSize.c[2]);
	piRender->RenderRect(vCenter-CVector(0,0,vSize.c[2]*0.5+vPlayAreaConfig.dCameraScroll*0.5),AxisPosX,AxisPosZ,vSize.c[0],vPlayAreaConfig.dCameraScroll);
	piRender->RenderRect(vCenter+CVector(0,0,vSize.c[2]*0.5+vPlayAreaConfig.dCameraScroll*0.5),AxisPosX,AxisPosZ,vSize.c[0],vPlayAreaConfig.dCameraScroll);

	m_Render.m_piRender->PopState();

	
	if(m_piSTFps)
	{
		char A[200];
		sprintf(A,"Fps: %.02f",m_FrameManager.m_piFrameManager->GetCurrentFps());
		m_piSTFps->SetText(A);
	}
	if(m_piSTVolume && m_SoundManagerWrapper.m_piSoundManager)
	{
		char A[200];
		sprintf(A,"Vol: %d%%",m_SoundManagerWrapper.m_piSoundManager->GetMasterVolume());
		m_piSTVolume->SetText(A);
	}

	UpdateLayerPanel();
}
void CFormationEditorMainWindow::ProcessFileNew()
{
	if(ConfirmDialog("Reset all and start a new project?","New project",eMessageDialogType_Warning))
	{
		m_FormationType.Detach();
		m_FormationType.Create("FormationTypes","CFormationType","__unnamed__");
		
		
		Reset();
		UpdateEntityControls();
		UpdateLayerPanel();
		UpdateCaption();
		CenterCamera();
		m_bShowFilePanel=false;
	}
}

void CFormationEditorMainWindow::ProcessFileOpen()
{
	if(m_ObjectSelector.m_piObjectSelector==NULL){return;}
	if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign==NULL){return;}
	
	std::string sScenario="./";
	unsigned long nSelectedFormationType=0;
	std::vector<IDesignObject *> vFormationTypes;
	GetSystemObjects("FormationTypes",&vFormationTypes);
	SPlayAreaConfig sPlayAreaConfig;
	m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetPlayAreaConfig(&sPlayAreaConfig);
	if(m_ObjectSelector.m_piObjectSelector->SelectObject(this,&vFormationTypes,&nSelectedFormationType,96.0,(96.0)/sPlayAreaConfig.dCameraAspectRatio))
	{
		Reset();
		if(m_FormationType.Attach(vFormationTypes[nSelectedFormationType]))
		{
			UpdateEntityControls();
			UpdateLayerPanel();
			UpdateCaption();
			CenterCamera();
		}
	}
	for(unsigned long x=0;x<vFormationTypes.size();x++){IDesignObject *piFormationType=vFormationTypes[x];REL(piFormationType);}
}

void CFormationEditorMainWindow::ProcessFileSave()
{
	/*std::string sScenario=m_sFile;
	bool bSave=true;
	if(m_sFile.length()==0)
	{
		bSave=SaveFileDialog("Save scenario...","Scenario files (*.ges)\0*.ges\0\0",&sScenario,true);
	}
	if(bSave)
	{
	}*/
}

void CFormationEditorMainWindow::ProcessFileSaveAs()
{
/*	std::string sScenario=m_sFile;
	if(SaveFileDialog("Save scenario...","Scenario files (*.ges)\0*.ges\0\0",&sScenario,true))
	{
		m_GameControllerWrapper.m_piGameController->SaveScenario(sScenario);	
		m_sFile=sScenario;
		UpdateCaption();
	}*/
}
void CFormationEditorMainWindow::ProcessFileExit()
{
	if(ConfirmDialog("Do you really want to exit?","Scenario Editor",eMessageDialogType_Question))
	{
		m_piGUIManager->ExitGUILoop();
	}
}

void CFormationEditorMainWindow::OnButtonClicked(IGameGUIButton *piControl)
{
	if(m_piBTIncreaseVolume==piControl && m_SoundManagerWrapper.m_piSoundManager)
	{
	  int volume=m_SoundManagerWrapper.m_piSoundManager->GetMasterVolume();
	  volume+=5;
	  m_SoundManagerWrapper.m_piSoundManager->SetMasterVolume(volume);
	}

	if(m_piBTDecreaseVolume==piControl && m_SoundManagerWrapper.m_piSoundManager)
	{
	  int volume=m_SoundManagerWrapper.m_piSoundManager->GetMasterVolume();
	  volume-=5;
	  if(volume<0){volume=0;}
	  m_SoundManagerWrapper.m_piSoundManager->SetMasterVolume(volume);
	}

	if(m_piBTFormationNew==piControl)
	{
		ProcessFileNew();
		m_bShowFilePanel=false;
	}
	if(m_piBTFormationOpen==piControl)
	{
		ProcessFileOpen();
		m_bShowFilePanel=false;
	}
	if(m_piBTFormationSave==piControl)
	{
		ProcessFileSave();
		m_bShowFilePanel=false;
	}
	if(m_piBTFormationSaveAs==piControl)
	{
		ProcessFileSaveAs();
		m_bShowFilePanel=false;
	}
	if(m_piBTFormationExit==piControl)
	{
		ProcessFileExit();
		m_bShowFilePanel=false;
	}
	if(m_piBTShowOptionsPanel==piControl){m_bShowOptionsPanel=!m_bShowOptionsPanel;}
	if(m_piBTShowEntitiesPanel==piControl){m_bShowEntitiesPanel=!m_bShowEntitiesPanel;}
	if(m_piBTShowFilePanel==piControl){m_bShowFilePanel=!m_bShowFilePanel;}
	if(m_piBTShowPlayAreaProperties==piControl){m_bShowPlayAreaPanel=true;m_nSelectedEntity=-1;}
	if(m_piBTOptionsBlend==piControl){m_bBlend=!m_bBlend;}
	if(m_piBTOptionsTextures==piControl){m_bTextures=!m_bTextures;}
	if(m_piBTOptionsSolid==piControl){m_bSolid=!m_bSolid;}
	if(m_piBTNewEntity==piControl)
	{
		unsigned long nSelectedEntityType=0;
		std::vector<IDesignObject *> vEntityTypes;
		GetSystemObjects("EntityTypes",&vEntityTypes);
		if(m_ObjectSelector.m_piObjectSelector->SelectObject(this,&vEntityTypes,&nSelectedEntityType))
		{
			ISystemObject *piObject=QI(ISystemObject,vEntityTypes[nSelectedEntityType]);
			IEntityType *piEntityType=QI(IEntityType,vEntityTypes[nSelectedEntityType]);
			std::string sEntityType=piObject?piObject->GetName():"";

			if(piEntityType)
			{
				SRoutePoint point;
				point.bAbsolutePoint=false;
				unsigned long nObjectIndex=m_FormationType.m_piFormationTypeDesign->AddElement();
				GetAirPlaneCoordinatesFromCursorPos(m_rRealRect.w/2,m_rRealRect.h/2,&point.vPosition);
				m_FormationType.m_piFormationTypeDesign->AddElementRoutePoint(nObjectIndex,0,point);
				m_FormationType.m_piFormationTypeDesign->SetElementEntityCount(nObjectIndex,1);
				m_FormationType.m_piFormationTypeDesign->SetElementEntityInterval(nObjectIndex,1000);
				m_FormationType.m_piFormationTypeDesign->SetElementEntityType(nObjectIndex,piEntityType);
			}
			REL(piObject);
			REL(piEntityType);
			UpdateEntityControls();
			m_nSelectedEntity=m_vEntityControls.size()-1;
		}
		for(unsigned long x=0;x<vEntityTypes.size();x++){IDesignObject *piEntityType=vEntityTypes[x];REL(piEntityType);}
	}
	
	for(unsigned int x=0;x<m_vEntityControls.size();x++)
	{
		if(m_vEntityControls[x]->m_BTListRow.m_piButton==piControl)
		{
			m_nSelectedEntity=x;
		}
	}
	SEntityControls		 *pEntity=NULL;
	if(m_nSelectedEntity!=-1){pEntity=m_vEntityControls[m_nSelectedEntity];}
	if(pEntity)
	{
		if(piControl==m_piBTEntityRemove)
		{
			m_FormationType.m_piFormationTypeDesign->RemoveElement(m_nSelectedEntity);
			UpdateEntityControls();
		}
		else if(piControl==m_piBTEntitySample)
		{
			unsigned long nSelectedEntityType=0;
			std::vector<IDesignObject *> vEntityTypes;
			GetSystemObjects("EntityTypes",&vEntityTypes);
			if(m_ObjectSelector.m_piObjectSelector->SelectObject(this,&vEntityTypes,&nSelectedEntityType))
			{
				IEntityType *piEntityType=QI(IEntityType,vEntityTypes[nSelectedEntityType]);
				m_FormationType.m_piFormationTypeDesign->SetElementEntityType(m_nSelectedEntity,piEntityType);
				REL(piEntityType);

				UpdateEntityControls();
			}
			for(unsigned long x=0;x<vEntityTypes.size();x++){IDesignObject *piEntityType=vEntityTypes[x];REL(piEntityType);}
		}
		else if(m_piBTEntityClearRoute==piControl)
		{
			m_FormationType.m_piFormationTypeDesign->ClearElementRoute(m_nSelectedEntity);
			m_nSelectedRoutePoint=-1;
		}
	}
	
	bool bPlayAreaChange=false;
	SPlayAreaConfig sPlayAreaConfig;
	if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign)
	{
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetPlayAreaConfig(&sPlayAreaConfig);
	}

	if(piControl==m_piBTPlayAreaIncreaseAirPlane)
	{
		sPlayAreaConfig.dAirPlaneHeight+=5.0;
		bPlayAreaChange=true;
	}
	else if(piControl==m_piBTPlayAreaDecreaseAirPlane)
	{
		sPlayAreaConfig.dAirPlaneHeight-=5.0;
		bPlayAreaChange=true;
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
	if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign && bPlayAreaChange)
	{
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->SetPlayAreaConfig(&sPlayAreaConfig);
	}
	
	if(m_FormationType.m_piFormationType!=NULL && m_nSelectedEntity!=-1)
	{
		if(m_piBTEntityIncreaseCount==piControl)
		{
			int nCount=m_FormationType.m_piFormationTypeDesign->GetElementEntityCount(m_nSelectedEntity);
			m_FormationType.m_piFormationTypeDesign->SetElementEntityCount(m_nSelectedEntity,nCount+1);
		}

		if(m_piBTEntityDecreaseCount==piControl)
		{
			int nCount=m_FormationType.m_piFormationTypeDesign->GetElementEntityCount(m_nSelectedEntity);
			if(nCount){m_FormationType.m_piFormationTypeDesign->SetElementEntityCount(m_nSelectedEntity,nCount-1);}
		}
		if(m_piBTEntityIncreaseDelay==piControl)
		{
			int nDelay=m_FormationType.m_piFormationTypeDesign->GetElementEntityDelay(m_nSelectedEntity);
			m_FormationType.m_piFormationTypeDesign->SetElementEntityDelay(m_nSelectedEntity,nDelay+100);
		}

		if(m_piBTEntityDecreaseDelay==piControl)
		{
			int nDelay=m_FormationType.m_piFormationTypeDesign->GetElementEntityDelay(m_nSelectedEntity);
			if(nDelay>=100){m_FormationType.m_piFormationTypeDesign->SetElementEntityDelay(m_nSelectedEntity,nDelay-100);}
		}
		if(m_piBTEntityIncreaseInterval==piControl)
		{
			int nInterval=m_FormationType.m_piFormationTypeDesign->GetElementEntityInterval(m_nSelectedEntity);
			m_FormationType.m_piFormationTypeDesign->SetElementEntityInterval(m_nSelectedEntity,nInterval+100);
		}

		if(m_piBTEntityDecreaseInterval==piControl)
		{
			int nInterval=m_FormationType.m_piFormationTypeDesign->GetElementEntityInterval(m_nSelectedEntity);
			if(nInterval>=100){m_FormationType.m_piFormationTypeDesign->SetElementEntityInterval(m_nSelectedEntity,nInterval-100);}
		}
	}
}

void CFormationEditorMainWindow::UpdateCaption()
{
	if(m_Viewport.m_piViewport)
	{
		std::string sCaption="Formation Editor";
		if(m_FormationType.m_piObject){sCaption+=" - "+m_FormationType.m_piObject->GetName();}
		m_Viewport.m_piViewport->SetCaption(sCaption);
	}
}

void CFormationEditorMainWindow::UpdateLayerPanel()
{
	m_piBTShowOptionsPanel->SetBackgroundColor(CVector(1,1,1),m_bShowOptionsPanel?0.5:0.3);
	m_piBTShowEntitiesPanel->SetBackgroundColor(CVector(1,1,1),m_bShowEntitiesPanel?0.5:0.3);
	m_piBTShowFilePanel->SetBackgroundColor(CVector(1,1,1),m_bShowFilePanel?0.5:0.3);

	m_piGRPlayAreaPanel->Show(m_bShowPlayAreaPanel && m_nSelectedEntity==-1);
	m_piGREntitiesPanel->Show(m_bShowEntitiesPanel);
	m_piGROptionsPanel->Show(m_bShowOptionsPanel);
	m_piGRFormation->Show(m_bShowFilePanel);
	m_piGREntityPanel->Show(false);
	
	SGameRect sListRect;
	if(m_piGREntityLayerList){m_piGREntityLayerList->GetRealRect(&sListRect);}
	
	double dCurrentY=sListRect.h;
	
	if(m_nSelectedEntity!=-1)
	{
		m_piSTEntityName->SetText(m_vEntityControls[m_nSelectedEntity]->m_piObject->GetName());
		m_piSTEntityObjectLabel->SetObject(m_vEntityControls[m_nSelectedEntity]->m_piDesignObject);
		m_piGREntityPanel->Show(m_bShowEntitiesPanel);
	}
	if(m_piBTNewEntity)
	{
		dCurrentY-=20+6;
		SGameRect sRowRect(3,dCurrentY,sListRect.w-6,20);
		m_piBTNewEntity->SetRect(&sRowRect);
	}
	double dCurrentX=3;
	if(m_vEntityControls.size()){dCurrentY-=41;}
	for(int x=m_vEntityControls.size()-1;x>=0;x--)
	{	
		if(dCurrentX+38>sListRect.w){dCurrentX=3;dCurrentY-=41;}

		SGameRect sRowRect(dCurrentX,dCurrentY,38,38);
		SEntityControls *pData=m_vEntityControls[x];
		if(pData->m_BTListRow.m_piButton)
		{
			pData->m_BTListRow.m_piButton->SetRect(&sRowRect);
			pData->m_BTListRow.m_piButton->SetBackgroundColor(CVector(1,1,1),x==m_nSelectedEntity?0.8:0.1);
		}
		if(pData->m_STEntity.m_piLabel)
		{
			SGameRect sRect(3,3,32,32);
			pData->m_STEntity.m_piLabel->SetRect(&sRect);
			pData->m_STEntity.m_piLabel->SetObject(m_vEntityControls[x]->m_piDesignObject);
		}
		dCurrentX+=41;
	}
	
	
	char A[MAX_PATH];
	
	SPlayAreaConfig sPlayAreaConfig;
	if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign)
	{
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetPlayAreaConfig(&sPlayAreaConfig);
	}
	
	sprintf(A,"Cam Dist     : %.f",sPlayAreaConfig.dCameraDistance);
	m_piSTPlayAreaCameraDistance->SetText(A);
	
	sprintf(A,"Cam Speed    : %.f",sPlayAreaConfig.dCameraSpeed);
	m_piSTPlayAreaCameraSpeed->SetText(A);
	
	sprintf(A,"View Angle   : %.02f",sPlayAreaConfig.dCameraViewAngle);
	m_piSTPlayAreaCameraViewAngle->SetText(A);
	
	sprintf(A,"Aspect Ratio : %.02f",sPlayAreaConfig.dCameraAspectRatio);
	m_piSTPlayAreaCameraAspectRatio->SetText(A);
	
	sprintf(A,"Scroll    : %.f",sPlayAreaConfig.dCameraScroll);
	m_piSTPlayAreaScroll->SetText(A);
	
	sprintf(A,"Air Plane : %.f",sPlayAreaConfig.dAirPlaneHeight);
	m_piSTPlayAreaAirPlane->SetText(A);
	
	if(m_FormationType.m_piFormationTypeDesign && m_nSelectedEntity!=-1)
	{
		sprintf(A,"Count : %d",m_FormationType.m_piFormationTypeDesign->GetElementEntityCount(m_nSelectedEntity));
		m_piSTEntityCount->SetText(A);
		
		sprintf(A,"Delay : %.02fs",((double)m_FormationType.m_piFormationTypeDesign->GetElementEntityDelay(m_nSelectedEntity))/1000.0);
		m_piSTEntityDelay->SetText(A);

		sprintf(A,"Inter : %.02fs",((double)m_FormationType.m_piFormationTypeDesign->GetElementEntityInterval(m_nSelectedEntity))/1000.0);
		m_piSTEntityInterval->SetText(A);
	}
}

void CFormationEditorMainWindow::CenterCamera()
{
	if(m_PlayAreaManagerWrapper.m_piPlayAreaManager==NULL){return;}
	bool bCenter=false;
	CVector vCenter,vSize;
	if(m_nSelectedEntity!=-1 && m_vEntityControls[m_nSelectedEntity]->m_piDesignObject)
	{
		CVector vMins,vMaxs;
		SRoutePoint point;
		m_vEntityControls[m_nSelectedEntity]->m_piDesignObject->DesignGetBBox(&vMins,&vMaxs);		
		m_FormationType.m_piFormationTypeDesign->GetElementRoutePoint(m_nSelectedEntity,0,&point);
		bCenter=true;
		vCenter=FormationToWorld(point.vPosition)+(vMaxs+vMins)*0.5;
		vSize=(vMaxs-vMins);
	}
	else
	{
		bCenter=true;
		CVector vMins,vMaxs;
		CVector vPlayerRouteStart,vPlayerRouteEnd;
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayerRoute(&vPlayerRouteStart,&vPlayerRouteEnd);
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayAreaPlaneAt(vPlayerRouteStart,&vMins,&vMaxs);
		vCenter=vPlayerRouteStart+(vMaxs+vMins)*0.5;
		vSize=(vMaxs-vMins)*2.0;
	}
	if(bCenter)
	{
		double dNearPlane=0,dFarPlane=0;
		m_Camera.m_piCamera->GetClippingPlanes(dNearPlane,dFarPlane);
	
		CVector vPos;
		vPos.c[0]=vCenter.c[0];
		vPos.c[2]=vCenter.c[2];
		vPos.c[1]=vCenter.c[1]+fabs(tan(m_Camera.m_piCamera->GetViewAngle()*2.0)*(vSize.c[0]+dNearPlane));
		CVector vAngles;
		m_Camera.m_piCamera->SetPosition(vPos);
		AnglesFromVector(vCenter-vPos,&vAngles);
		m_Camera.m_piCamera->SetAngles(vAngles);
	}
}

void CFormationEditorMainWindow::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_F1 && m_bSimulationStarted){m_bInspectionMode=!m_bInspectionMode;*pbProcessed=true;}
	else if(nKey==GK_F2){ProcessFileSave();*pbProcessed=true;}
	else if(nKey==GK_F3){ProcessFileOpen();*pbProcessed=true;}
	else if(nKey==GK_F5){if(m_piGUIManager->IsKeyDown(GK_LSHIFT)){StopGameSimulation();}else{StartGameSimulation();}*pbProcessed=true;}
	else if(nKey==GK_PAUSE){m_FrameManager.m_piFrameManager->TogglePauseOnNextFrame();*pbProcessed=true;}
	else if(nKey=='T'){m_bTextures=!m_bTextures;*pbProcessed=true;}
	else if(nKey=='P'){m_bRenderPlayArea=!m_bRenderPlayArea;*pbProcessed=true;}
	else if(nKey=='C'){m_bColors=!m_bColors;*pbProcessed=true;}
	else if(nKey=='L'){m_bSolid=!m_bSolid;*pbProcessed=true;}
	else if(nKey=='B'){m_bBlend=!m_bBlend;*pbProcessed=true;}
	else if(nKey==GK_HOME){CenterCamera();*pbProcessed=true;}
	else if(nKey==GK_DELETE)
	{
		*pbProcessed=true;
		if(m_nSelectedEntity!=-1 && m_nSelectedRoutePoint>0)
		{
			m_FormationType.m_piFormationTypeDesign->RemoveElementRoutePoint(m_nSelectedEntity,m_nSelectedRoutePoint);
			if(m_nSelectedRoutePoint>=(int)m_FormationType.m_piFormationTypeDesign->GetElementRoutePoints(m_nSelectedEntity))
			{
				m_nSelectedRoutePoint=m_FormationType.m_piFormationTypeDesign->GetElementRoutePoints(m_nSelectedEntity)-1;
			}
		}
	}
	else if(nKey==GK_INSERT)
	{
		*pbProcessed=true;
		if(m_nSelectedEntity!=-1)
		{
			SRoutePoint firstPoint;
			firstPoint.bAbsolutePoint=false;
			m_FormationType.m_piFormationTypeDesign->GetElementRoutePoint(m_nSelectedEntity,0,&firstPoint);
			
			SRoutePoint sRoutePoint;
			sRoutePoint.bAbsolutePoint=false;
			if(m_nSelectedRoutePoint==-1 || m_nSelectedRoutePoint==0)
			{
				CVector vForward=AxisPosX;
				sRoutePoint.vPosition=firstPoint.vPosition;
				sRoutePoint.vPosition+=vForward*(0.1);
			}
			else
			{
				SRoutePoint sOldPoint1;
				SRoutePoint sOldPoint2;
				sOldPoint1.bAbsolutePoint=false;
				sOldPoint2.bAbsolutePoint=false;
				if(m_nSelectedRoutePoint>0)
				{
					m_FormationType.m_piFormationTypeDesign->GetElementRoutePoint(m_nSelectedEntity,m_nSelectedRoutePoint-1,&sOldPoint1);
				}
				else
				{
					sOldPoint1.vPosition=firstPoint.vPosition;
				}
				m_FormationType.m_piFormationTypeDesign->GetElementRoutePoint(m_nSelectedEntity,m_nSelectedRoutePoint,&sOldPoint2);
				CVector vDirection=sOldPoint2.vPosition-sOldPoint1.vPosition;
				vDirection.N();
				sRoutePoint.vPosition+=sOldPoint2.vPosition+vDirection*(0.1);
			}
			m_FormationType.m_piFormationTypeDesign->AddElementRoutePoint(m_nSelectedEntity,m_nSelectedRoutePoint+1,sRoutePoint);
			m_nSelectedRoutePoint=m_nSelectedRoutePoint+1;
		}
	}
	else if(nKey==GK_ESCAPE)
	{
		if(m_nSelectedEntity!=-1 || m_nSelectedRoutePoint!=-1)
		{
			m_nSelectedEntity=-1;
			m_nSelectedRoutePoint=-1;
		}
		else
		{
			ProcessFileExit();
		}
		*pbProcessed=true;
	}
}

void CFormationEditorMainWindow::OnMouseDown( int nButton,double dx,double dy )
{
	if(nButton==GK_LBUTTON)
	{	
		int nSelectedEntity=-1;
		int nSelectedRoutePoint=0;
		
		for(unsigned int x=0;x<m_vEntityControls.size();x++)
		{
			SRoutePoint point,point2;
			m_FormationType.m_piFormationTypeDesign->GetElementRoutePoint(x,0,&point);
			if(m_FormationType.m_piFormationTypeDesign->GetElementRoutePoints(x)>1)
			{
				m_FormationType.m_piFormationTypeDesign->GetElementRoutePoint(x,1,&point2);
			}
			else
			{
				point2.vPosition=point.vPosition+AxisPosY*0.1;
			}
	
			m_Render.m_piRender->StartSelection(m_rRealRect,m_Camera.m_piCamera,dx,dy,10);
			for(unsigned y=1;y<m_FormationType.m_piFormationTypeDesign->GetElementRoutePoints(x);y++)
			{
				SRoutePoint point;
				m_FormationType.m_piFormationTypeDesign->GetElementRoutePoint(x,y,&point);
				m_Render.m_piRender->SetSelectionId(y);
				CVector vWorld=FormationToWorld(point.vPosition);
				m_Render.m_piRender->RenderPoint(vWorld,(int)x==m_nSelectedRoutePoint?15:8,CVector(0.8,0.8,0.8),1.0);
			}
			int nNewSelection=m_Render.m_piRender->EndSelection();

			if(nNewSelection==-1)
			{
				m_Render.m_piRender->StartSelection(m_rRealRect,m_Camera.m_piCamera,dx,dy,10);
				m_Render.m_piRender->SetSelectionId(0);
				CVector vWorld=FormationToWorld(point.vPosition);
				CVector vWorld2=FormationToWorld(point2.vPosition);
				CVector vAngles=AnglesFromVector(vWorld2-vWorld);
				m_vEntityControls[x]->m_piEntityType->DesignRender(m_Render.m_piRender,vWorld,vAngles,false);
				nNewSelection=m_Render.m_piRender->EndSelection();
			}

			if(nNewSelection!=-1 && m_nSelectedEntity==(int)x && m_nSelectedRoutePoint==nNewSelection)
			{
				if(DetectDrag(dx,dy))
				{
					m_bMovingRoutePoint=true;
					m_piGUIManager->SetMouseCapture(this);
					SRoutePoint point;
					m_FormationType.m_piFormationTypeDesign->GetElementRoutePoint(x,nNewSelection,&point);
					m_vObjectOriginalPosition=point.vPosition;
					GetAirPlaneCoordinatesFromCursorPos(dx,dy,&m_vCursorOriginalPosition);
				}
				m_bShowEntityPanel=true;
				m_bShowEntitiesPanel=true;
			
			}
			if(nNewSelection!=-1)
			{
				m_bShowEntitiesPanel=true;
				m_bShowPlayAreaPanel=false;
				nSelectedEntity=x;
				nSelectedRoutePoint=nNewSelection;
			}
		}
		m_nSelectedEntity=nSelectedEntity;
		m_nSelectedRoutePoint=nSelectedRoutePoint;
	}
}

bool CFormationEditorMainWindow::GetAirPlaneCoordinatesFromCursorPos(double x,double y,CVector *pAirPlanePos)
{
	CLine mouseRay=GetMouseRay(x,y,10000.0,m_Camera.m_piCamera);

	CVector vPlayAreaMins,vPlayAreaMaxs;
	m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetVisibleAirPlayPlane(&vPlayAreaMins,&vPlayAreaMaxs);

	CPlane airPlane(AxisPosY,vPlayAreaMins);
	double dSide1=airPlane.GetSide(mouseRay.m_Points[0]);
	double dSide2=airPlane.GetSide(mouseRay.m_Points[1]);
	double dLength=(dSide1-dSide2);
	double dFraction=dLength?dSide1/dLength:0;
	CVector vPos=mouseRay.m_Points[0]+(mouseRay.m_Points[1]-mouseRay.m_Points[0])*dFraction;

	if(pAirPlanePos){*pAirPlanePos=WorldToFormation(vPos);}

	return (dSide1*dSide2)<0;
}


void CFormationEditorMainWindow::OnMouseMove( double x,double y )
{
	if(m_bMovingRoutePoint && m_nSelectedRoutePoint!=-1)
	{
		CVector vTemp;
		if(GetAirPlaneCoordinatesFromCursorPos(x,y,&vTemp))
		{
			SRoutePoint point;
			point.vPosition=vTemp;
			point.bAbsolutePoint=false;
			m_FormationType.m_piFormationTypeDesign->SetElementRoutePoint(m_nSelectedEntity,m_nSelectedRoutePoint,point);
		}
	}
}

void CFormationEditorMainWindow::OnMouseUp( int nButton,double x,double y )
{
	if(m_piGUIManager->HasMouseCapture(this)){m_piGUIManager->ReleaseMouseCapture();}
	m_bMovingRoutePoint=false;
}

void CFormationEditorMainWindow::StopGameSimulation()
{
	if(m_bSimulationStarted)
	{
		m_Formation.Detach();
		m_GameControllerWrapper.m_piGameController->Stop();

		if(m_PlayAreaManagerWrapper.m_piPlayAreaManager)
		{
			m_PlayAreaManagerWrapper.m_piPlayAreaDesign->RemoveElement(m_nFormationId);
			m_nFormationId=0;
		}
		m_bSimulationStarted=false;
	}
}

void CFormationEditorMainWindow::StartGameSimulation()
{
	if(m_bSimulationStarted)
	{
		StopGameSimulation();
	}
	if(m_PlayAreaManagerWrapper.m_piPlayAreaManager)
	{
		CVector vPlayerStart,vPlayerEnd;
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayerRoute(&vPlayerStart,&vPlayerEnd);

		IPlayAreaElement *piElement;
		m_nFormationId=m_PlayAreaManagerWrapper.m_piPlayAreaDesign->AddElement("CPlayAreaFormation");
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetElement(m_nFormationId,&piElement);
		m_Formation.Attach(piElement);
		if(m_Formation.m_piPlayAreaFormation)
		{
			m_Formation.m_piPlayAreaFormation->SetPosition(vPlayerStart);
			m_Formation.m_piPlayAreaFormation->SetFormationType(m_FormationType.m_piFormationType);
		}
		REL(piElement);
	}
	m_GameControllerWrapper.m_piGameController->Start();
	m_bSimulationStarted=true;
}

void CFormationEditorMainWindow::UpdateEntityControls()
{
	unsigned int x;
	std::vector<SEntityControls *>::iterator i;
	for(x=0;x<m_vEntityControls.size();x++)
	{
		SEntityControls *pControls=m_vEntityControls[x];
		UNSUBSCRIBE_FROM_CAST(pControls->m_BTListRow.m_piButton,IGameGUIButtonEvents);
		pControls->m_BTListRow.m_piButton->DestroyWindow();
		pControls->m_STEntity.m_piLabel->DestroyWindow();
		delete pControls;
		pControls=NULL;
	}
	m_vEntityControls.clear();
	if(m_FormationType.m_piFormationTypeDesign)
	{
		for(x=0;x<m_FormationType.m_piFormationTypeDesign->GetElements();x++)
		{
			IEntityType		 *piEntityType=NULL;
			m_FormationType.m_piFormationTypeDesign->GetElementEntityType(x,&piEntityType);
			IDesignObject *piDesignObject=QI(IDesignObject,piEntityType);
			ISystemObject *piObject=QI(ISystemObject,piDesignObject);
			if(!piObject)
			{
				REL(piEntityType);
				REL(piDesignObject);
				continue;
			}

			SEntityControls *pControls=new SEntityControls;
			pControls->m_nElementId=x;
			pControls->m_BTListRow.Create(m_piSystem,"CGameGUIButton","");
			pControls->m_piEntityType=ADD(piEntityType);
			pControls->m_piDesignObject=ADD(piDesignObject);
			pControls->m_piObject=QI(ISystemObject,piEntityType);
			if(pControls->m_BTListRow.m_piButton)
			{
				pControls->m_BTListRow.m_piButton->InitWindow(m_piGREntityLayerList,false);
				pControls->m_BTListRow.m_piButton->SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
				pControls->m_BTListRow.m_piButton->SetBackgroundColor(CVector(1,1,1),0.1);
				pControls->m_BTListRow.m_piButton->Show(true);
				pControls->m_BTListRow.m_piButton->Activate(true);
				SUBSCRIBE_TO_CAST(pControls->m_BTListRow.m_piButton,IGameGUIButtonEvents);

				pControls->m_STEntity.Create(m_piSystem,"CFormationEditorObjectLabel","");
				if(pControls->m_STEntity.m_piLabel)
				{
					pControls->m_STEntity.m_piLabel->InitWindow(pControls->m_BTListRow.m_piButton,false);
					pControls->m_STEntity.m_piLabel->SetObject(piDesignObject);
					pControls->m_STEntity.m_piLabel->SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
					pControls->m_STEntity.m_piLabel->SetBackgroundColor(CVector(0,0,0),1);
					pControls->m_STEntity.m_piLabel->Show(true);
					pControls->m_STEntity.m_piLabel->Activate(false);
				}
			}
			REL(piEntityType);
			REL(piDesignObject);
			REL(piObject);
			m_vEntityControls.push_back(pControls);
		}
	}
	if(m_nSelectedEntity!=-1 && m_nSelectedEntity>=(int)m_vEntityControls.size())
	{
		m_nSelectedEntity=(int)(m_vEntityControls.size()-1);
	}
}

CVector CFormationEditorMainWindow::WorldToFormation(CVector vWorldPoint)
{
	if(m_PlayAreaManagerWrapper.m_piPlayAreaManager==NULL){return Origin;}
	CVector vVisiblePlayAreaMins,vVisiblePlayAreaMaxs;
	m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetVisibleAirPlayPlane(&vVisiblePlayAreaMins,&vVisiblePlayAreaMaxs);
	CVector vPlayAreaSize=(vVisiblePlayAreaMaxs-vVisiblePlayAreaMins)*0.5;
	CVector vPlayAreaCenter=vVisiblePlayAreaMins+(vVisiblePlayAreaMaxs-vVisiblePlayAreaMins)*0.5;
	
	CVector vFormationPoint;
	CVector vAbsPoint=vWorldPoint-vPlayAreaCenter;
	vFormationPoint.c[1]=vAbsPoint.c[0]/vPlayAreaSize.c[0];
	vFormationPoint.c[0]=vAbsPoint.c[2]/vPlayAreaSize.c[2];
	return vFormationPoint;
}

CVector CFormationEditorMainWindow::FormationToWorld(CVector vFormationPoint)
{
	if(m_PlayAreaManagerWrapper.m_piPlayAreaManager==NULL){return Origin;}
	CVector vVisiblePlayAreaMins,vVisiblePlayAreaMaxs;
	m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetVisibleAirPlayPlane(&vVisiblePlayAreaMins,&vVisiblePlayAreaMaxs);
	CVector vPlayAreaSize=(vVisiblePlayAreaMaxs-vVisiblePlayAreaMins)*0.5;
	CVector vPlayAreaCenter=vVisiblePlayAreaMins+(vVisiblePlayAreaMaxs-vVisiblePlayAreaMins)*0.5;
	
	CVector vWoldPoint;
	CVector vAbsPoint;
	vWoldPoint.c[0]=vFormationPoint.c[1]*vPlayAreaSize.c[0];
	vWoldPoint.c[2]=vFormationPoint.c[0]*vPlayAreaSize.c[2];
	vWoldPoint+=vPlayAreaCenter;
	return vWoldPoint;	
}

void CFormationEditorMainWindow::RenderRoute( IGenericRender * piRender, int nEntity,int nSelectedEntity, int nSelectedRoutePoint )
{
	piRender->PushState();
	piRender->DeactivateDepth();

	SRoutePoint point;
	m_FormationType.m_piFormationTypeDesign->GetElementRoutePoint(nEntity,0,&point);
	CVector vPreviousPoint=FormationToWorld(point.vPosition);
	for(unsigned x=1;x<m_FormationType.m_piFormationTypeDesign->GetElementRoutePoints(nEntity);x++)
	{
		double dPointSize=8;
		CVector vPointColor=CVector(0.4,0.4,0.4);
		CVector vLineColor=CVector(0.4,0.4,0.4);
		if(nSelectedEntity==nEntity)
		{
			vPointColor=CVector(0.8,0.8,0.8);
			vLineColor=CVector(0.8,0.8,0.8);
			if(nSelectedRoutePoint==(int)x)
			{
				dPointSize=15;
				vPointColor=CVector(0,0,0.8);
			}
		}

		m_FormationType.m_piFormationTypeDesign->GetElementRoutePoint(nEntity,x,&point);
		CVector vWorld=FormationToWorld(point.vPosition);
		piRender->RenderLine(vPreviousPoint,vWorld,vLineColor);
		piRender->RenderPoint(vWorld,dPointSize,vPointColor,1.0);
		vPreviousPoint=vWorld;
	}
	piRender->PopState();
}
void CFormationEditorMainWindow::OnWantFocus(bool *pbWant){*pbWant=true;}
