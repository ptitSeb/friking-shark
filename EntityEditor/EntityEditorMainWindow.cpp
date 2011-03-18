#include "./stdafx.h"
#include "resource.h"
#include "GameRunTimeLib.h"
#include "GameGUILib.h"
#include "EntityEditorMainWindow.h"

#define SELECT_FORMATION_BASE_INDEX 0x800

extern CSystemModuleHelper *g_pSystemModuleHelper;

CEntityEditorMainWindow::CEntityEditorMainWindow(void)
{
	m_d3DFontSize=0;
	
	m_bShowTranslationGizmo=false;
	m_bShowRotationGizmo=false;
	
	m_bMovingGizmo=false;
	m_piAnimation=NULL;
	
	m_bShowOptionsPanel=false;
	m_bShowEntitiesPanel=false;
	m_bShowEntityPanel=false;
	m_bShowFilePanel=false;
	m_bShowPlayAreaPanel=false;
	
	m_bShowModelPanel=false;
	m_bShowSoundPanel=false;
	m_bShowEventPanel=false;
	m_bShowParticleSystemPanel=false;
	
	m_pEntity=NULL;
	
	m_bTextures=1;
	m_bSolid=1;
	
	for(unsigned int x=0;x<ePropertyPanel_Count;x++){m_ppiPropertyPanels[x]=NULL;}
	
	InitializeChildren();
	m_piGameSystem=NULL;
	m_bSimulationStarted=false;
	
	m_TranslationGizmo.SetSize(5);
	m_RotationGizmo.SetRadius(3.725);
}

CEntityEditorMainWindow::~CEntityEditorMainWindow(void)
{
}

bool CEntityEditorMainWindow::InitWindow(IGameWindow *piParent,bool bPopup)
{
	bool bOk=CGameWindowBase::InitWindow(piParent,bPopup);
	if(!bOk){return false;}
	
	m_ClassSelector.Attach("GameGUI","ClassSelector");
	m_ObjectSelector.Attach("GameGUI","ObjectSelector");
	m_NamedObjectSelector.Attach("GameGUI","NamedObjectSelector");
	m_Viewport.Attach("GameGUI","Viewport");
	m_Render.Attach("GameGUI","Render");
	m_Viewport.m_piViewport->SetCaption("Entity Editor");
	m_Camera.Create("GameGUI","Camera","");
	
	SGameRect sRect;
	sRect.x=0;
	sRect.y=0;
	sRect.w=1;
	sRect.h=1;
	m_eReferenceSystem=eGameGUIReferenceSystem_Relative;
	SetRect(&sRect);

	m_piGUIManager->SetFocus(this);

	CSystemLoaderHelper     systemLoader;
	m_piGameSystem=systemLoader.LoadSystem("Scripts/GameSystem.cfg","GameSystem");
	m_GameControllerWrapper.Attach("GameSystem","GameController");
	if(m_GameControllerWrapper.m_piGameController)
	{
	  m_GameControllerWrapper.m_piGameController->SetupGame();
	  m_GameControllerWrapper.m_piGameController->CreateScenario();
	}
	m_pEntity=new CFakeEntity();
	m_pEntity->GetPhysicInfo()->dMaxVelocity=10;

	m_FrameManager.Attach("GameSystem","FrameManager");
	m_PhysicManagerWrapper.Attach("GameSystem","PhysicManager");
	m_SoundManagerWrapper.Attach("GameSystem","SoundManager");
	UpdateVisiblePanels();
	UpdateStateList();
	return bOk;
}

void CEntityEditorMainWindow::DestroyWindow()
{
	StopGameSimulation();
	Reset();
	UpdateStateList();
	if(m_GameControllerWrapper.m_piGameController){m_GameControllerWrapper.m_piGameController->EndGame();}
	if(m_GameControllerWrapper.m_piGameController){m_GameControllerWrapper.m_piGameController->CloseScenario();}
	m_PhysicManagerWrapper.Detach();
	m_SoundManagerWrapper.Detach();
	m_GameControllerWrapper.Detach();
	
	m_FrameManager.Detach();
	if(m_piGameSystem){m_piGameSystem->Destroy();}
	REL(m_piGameSystem);
	m_pEntity=NULL; // Se borra por el Entitymanager
	
	m_ClassSelector.Detach();
	m_ObjectSelector.Detach();
	m_NamedObjectSelector.Detach();
	m_Viewport.Detach();
	m_Viewport.Detach();

	UNSUBSCRIBE_FROM_ALL();
	CGameWindowBase::DestroyWindow();
}

void CEntityEditorMainWindow::Reset()
{
	delete m_piAnimation;
	m_piAnimation=NULL;
	
	StopGameSimulation();
	m_EntityType.Detach();

	m_bShowFilePanel=false;
	m_bShowEntitiesPanel=false;
	m_bShowEntityPanel=false;
	m_bShowPlayAreaPanel=false;
	m_bShowOptionsPanel=false;
	m_bSimulationStarted=false;
	m_bInspectionMode=false;
	m_sEntityName="";

	UpdateCaption();
	UpdateStateList();
}

void CEntityEditorMainWindow::ProcessInput(double dTimeFraction,double dRealTimeFraction)
{
	if(!m_piGUIManager->HasFocus(this)){return;}
	
	if(!m_bSimulationStarted || m_bInspectionMode)
	{
		if(m_piGUIManager->IsKeyDown(GK_UP) || m_piGUIManager->IsKeyDown('W')){ProcessKey(KEY_FORWARD,dTimeFraction,dRealTimeFraction);}
		if(m_piGUIManager->IsKeyDown(GK_DOWN) || m_piGUIManager->IsKeyDown('S')){ProcessKey(KEY_BACK,dTimeFraction,dRealTimeFraction);}
		if(m_piGUIManager->IsKeyDown(GK_LEFT) || m_piGUIManager->IsKeyDown('A')){ProcessKey(KEY_LEFT,dTimeFraction,dRealTimeFraction);}
		if(m_piGUIManager->IsKeyDown(GK_RIGHT) || m_piGUIManager->IsKeyDown('D')){ProcessKey(KEY_RIGHT,dTimeFraction,dRealTimeFraction);}
		if(m_piGUIManager->IsKeyDown('R')){ProcessKey(KEY_UP,dTimeFraction,dRealTimeFraction);}
		if(m_piGUIManager->IsKeyDown('F')){ProcessKey(KEY_DOWN,dTimeFraction,dRealTimeFraction);}
	}
}

void CEntityEditorMainWindow::ProcessKey(unsigned short nKey,double dTimeFraction,double dRealTimeFraction)
{
	double dTranslationInspectionSpeed=100.0;
	double dForwardSpeed=dTranslationInspectionSpeed*dTimeFraction;
	double dCameraForwardSpeed=dTranslationInspectionSpeed*dRealTimeFraction*3.0;
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
void CEntityEditorMainWindow::SetupRenderOptions(IGenericRender *piRender,IGenericCamera *piCamera)
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

void CEntityEditorMainWindow::OnDraw(IGenericRender *piRender)
{
	if(!m_FrameManager.m_piFrameManager)
	{
		return;
	}
	/*
	m_Render.m_piRender->SetAmbientLight(CVector(0.3,0.3,0.3));
	if(m_pEntity)
	{
		CVector vSunPosition=m_pEntity->GetPhysicInfo()->vPosition;
		vSunPosition=CVector(50,50,50);
		CVector vSunDirection;
		vSunDirection=m_pEntity->GetPhysicInfo()->vPosition-vSunPosition;
		m_Render.m_piRender->SetSunLight(vSunPosition,vSunDirection,CVector(0.3,0.3,0.3));
	}
	*/
	m_Camera.m_piCamera->SetAspectRatio(m_rRealRect.w/m_rRealRect.h);

	m_FrameManager.m_piFrameManager->ProcessFrame();
	
	ProcessInput(m_FrameManager.m_piFrameManager->GetTimeFraction(),m_FrameManager.m_piFrameManager->GetRealTimeFraction());
	
	if(m_piAnimation)
	{
		m_piAnimation->ProcessFrame(m_PhysicManagerWrapper.m_piPhysicManager,m_FrameManager.m_piFrameManager->GetCurrentRealTime(),m_FrameManager.m_piFrameManager->GetRealTimeFraction());
		if(m_piAnimation->HasFinished()){m_piAnimation->Activate(m_FrameManager.m_piFrameManager->GetCurrentRealTime());}
		
		CVector vUpdate=Origin;
		if(m_pEntity)
		{
			vUpdate+=AxisPosX*m_pEntity->GetPhysicInfo()->dMaxVelocity*m_FrameManager.m_piFrameManager->GetTimeFraction();
			m_pEntity->GetPhysicInfo()->vPosition+=vUpdate;
		}
		CVector vNewCameraPos=m_Camera.m_piCamera->GetPosition()+vUpdate;
		m_Camera.m_piCamera->SetPosition(vNewCameraPos);
	}
	
	m_Render.m_piRender->ActivateDepth();
	m_Render.m_piRender->PushOptions();
	m_Render.m_piRender->PushState();

	m_bTextures?m_Render.m_piRender->EnableTextures():m_Render.m_piRender->DisableTextures();
	m_bSolid?m_Render.m_piRender->EnableSolid():m_Render.m_piRender->DisableSolid();
	m_Render.m_piRender->EnableBlending();
	m_Render.m_piRender->EnableShadows();
	m_Render.m_piRender->DisableLighting();
	m_Render.m_piRender->DisableHeightFog();
	m_Render.m_piRender->DisableShaders();
	
	SetupRenderOptions(piRender,m_Camera.m_piCamera);
	piRender->StartStagedRendering();
	if(m_piAnimation){m_piAnimation->CustomRender(m_Render.m_piRender,m_Camera.m_piCamera);}
	piRender->EndStagedRendering();

	if(!m_bMovingGizmo && m_pEntity && m_PositionWrapper.m_piDesign)
	{
		m_TranslationGizmo.SetPosition(m_pEntity->GetPhysicInfo()->vPosition+m_PositionWrapper.m_piDesign->GetPosition());
	}
		
	m_RotationGizmo.SetPosition(m_TranslationGizmo.GetPosition());
	if(m_bShowTranslationGizmo){m_TranslationGizmo.Render(m_Render.m_piRender,m_Camera.m_piCamera);}
	if(m_bShowRotationGizmo){m_RotationGizmo.Render(m_Render.m_piRender,m_Camera.m_piCamera);}

	m_Render.m_piRender->PopOptions();
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
}
void CEntityEditorMainWindow::ProcessFileNew()
{
	if(m_ClassSelector.m_piClassSelector==NULL){return;}
	
	if(m_EntityType.m_piEntityType)
	{
		if(!ConfirmDialog("Reset all and start a new project?","New project",eMessageDialogType_Warning)){return;}
	}
	std::string sClass;
	if(m_ClassSelector.m_piClassSelector->SelectClass(this,"Select Entity Class...","EntityTypes",&sClass))
	{
		Reset();
		m_EntityType.Create("EntityTypes",sClass,"");
		m_sEntityName="";
		UpdateCaption();
		UpdateStateList();
	}	
}

void CEntityEditorMainWindow::ProcessFileOpen()
{
	if(m_ObjectSelector.m_piObjectSelector==NULL){return;}
	
	unsigned long nSelectedEntityType=0;
	std::vector<IDesignObject *> vEntityTypes;
	GetSystemObjects("EntityTypes",&vEntityTypes);

	if(m_ObjectSelector.m_piObjectSelector->SelectObject("Open Entity...",this,&vEntityTypes,&nSelectedEntityType,96.0,96.0))
	{
		Reset();
		CConfigFile cfg;
		CEntityTypeWrapper existingWrapper;
		bool bOk=existingWrapper.Attach(vEntityTypes[nSelectedEntityType]);
		if(bOk){bOk=existingWrapper.m_piSerializable->Serialize(cfg.GetRoot());}
		if(bOk){bOk=m_EntityType.Create("EntityTypes",existingWrapper.m_piObject->GetClass(),"");}
		if(bOk){bOk=m_EntityType.m_piSerializable->Unserialize(cfg.GetRoot());}
		if(bOk)
		{
			m_sEntityName=existingWrapper.m_piObject->GetName();
			UpdateCaption();
			UpdateStateList();
			CenterCamera();
		}
	}
	for(unsigned long x=0;x<vEntityTypes.size();x++){IDesignObject *piEntityType=vEntityTypes[x];REL(piEntityType);}	
}


void CEntityEditorMainWindow::ProcessFileSave()
{
	if(m_EntityType.m_piEntityType==NULL)
	{
		MessageDialog("No entity to save","Entity Editor",eMessageDialogType_Error);
		return;
	}
	
	std::string sName=m_sEntityName;
	bool bSave=true;
	while(bSave && sName.length()==0)
	{
		bSave=InputDialog(&sName,"Save entity as...");
		if(bSave)
		{
			if(sName.length()==0)
			{
				MessageDialog("Please provide a valid entity name","Entity Editor",eMessageDialogType_Error);
			}
			else
			{
				CEntityTypeWrapper existingWrapper;
				if(existingWrapper.Attach("EntityTypes",sName) && sName!=m_sEntityName)
				{
					std::string sText="Entity '";
					sText+=sName;
					sText+="' already exists. Overwrite?";
					if(!ConfirmDialog(sText,"Entity Editor",eMessageDialogType_Warning))
					{
						bSave=false;
					}
				}
			}
		}
	}
	if(bSave)
	{
		CConfigFile cfg;
		CSystemSaverHelper helper;
		CEntityTypeWrapper existingWrapper;
		bool bOk=true;
		bOk=existingWrapper.Create("EntityTypes",m_EntityType.m_piObject->GetClass(),sName);
		if(bOk){bOk=m_EntityType.m_piSerializable->Serialize(cfg.GetRoot());}
		if(bOk){bOk=existingWrapper.m_piSerializable->Unserialize(cfg.GetRoot());}
		// Dump all formations to the formation config file
		if(bOk){bOk=helper.SaveSystemNamedObjects("Scripts/EntityTypes.cfg","EntityTypes");}
		if(bOk){m_sEntityName=sName;}
		existingWrapper.Detach(false);
		UpdateCaption();
	}
}


void CEntityEditorMainWindow::ProcessFileRemove()
{
	unsigned long nSelectedEntityType=0;
	std::vector<IDesignObject *> vEntityTypes;
	GetSystemObjects("EntityTypes",&vEntityTypes);
	
	if(m_ObjectSelector.m_piObjectSelector->SelectObject("Remove Entity...",this,&vEntityTypes,&nSelectedEntityType,96.0,96.0))
	{
		CEntityTypeWrapper existingWrapper;
		bool bOk=existingWrapper.Attach(vEntityTypes[nSelectedEntityType]);
		if(bOk)
		{
			std::string sText="Remove entity '";
			sText+=existingWrapper.m_piObject->GetName();
			sText+="' ?";
			if(ConfirmDialog(sText,"Entity Editor",eMessageDialogType_Error))
			{
				existingWrapper.Destroy();
				CSystemSaverHelper helper;
				helper.SaveSystemNamedObjects("Scripts/EntityTypes.cfg","EntityTypes");
			}
		}
	}
	for(unsigned long x=0;x<vEntityTypes.size();x++){IDesignObject *piEntityType=vEntityTypes[x];REL(piEntityType);}
}

void CEntityEditorMainWindow::ProcessFileSaveAs()
{
	if(m_EntityType.m_piEntityType==NULL)
	{
		MessageDialog("No entity to save","Entity Editor",eMessageDialogType_Error);
		return;
	}
	
	std::string sName=m_sEntityName;
	bool bSave=true;
	do
	{
		bSave=InputDialog(&sName,"Save entity as...");
		if(bSave)
		{
			if(sName.length()==0)
			{
				MessageDialog("Please provide a valid entity name","Entity Editor",eMessageDialogType_Error);
			}
			else
			{
				CEntityTypeWrapper existingWrapper;
				if(existingWrapper.Attach("EntityTypes",sName) && sName!=m_sEntityName)
				{
					std::string sText="Entity '";
					sText+=sName;
					sText+="' already exists. Overwrite?";
					if(!ConfirmDialog(sText,"Entity Editor",eMessageDialogType_Warning))
					{
						bSave=false;
					}
				}
			}
		}
	}
	while(bSave && sName.length()==0);

	if(bSave)
	{
		CConfigFile cfg;
		CSystemSaverHelper helper;
		CEntityTypeWrapper existingWrapper;
		bool bOk=true;
		bOk=existingWrapper.Create("EntityTypes",m_EntityType.m_piObject->GetClass(),sName);
		if(bOk){bOk=m_EntityType.m_piSerializable->Serialize(cfg.GetRoot());}
		if(bOk){bOk=existingWrapper.m_piSerializable->Unserialize(cfg.GetRoot());}
		if(bOk){m_sEntityName=sName;}
		// Dump all formations to the formation config file
		if(bOk){bOk=helper.SaveSystemNamedObjects("Scripts/EntityTypes.cfg","EntityTypes");}
		existingWrapper.Detach(false);
		UpdateCaption();
	}
}

void CEntityEditorMainWindow::ProcessFileExit()
{
	if(ConfirmDialog("Do you really want to exit?","Entity Editor",eMessageDialogType_Question))
	{
		m_piGUIManager->ExitGUILoop();
	}
}

void CEntityEditorMainWindow::OnButtonClicked(IGameGUIButton *piControl)
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

	if(m_piBTEntityNew==piControl)
	{
		ProcessFileNew();
		m_bShowFilePanel=false;
	}
	if(m_piBTEntityOpen==piControl)
	{
		ProcessFileOpen();
		m_bShowFilePanel=false;
	}
	if(m_piBTEntitySave==piControl)
	{
		ProcessFileSave();
		m_bShowFilePanel=false;
	}
	if(m_piBTEntitySaveAs==piControl)
	{
		ProcessFileSaveAs();
		m_bShowFilePanel=false;
	}
	if(m_piBTEntityRemove==piControl)
	{
		ProcessFileRemove();
		m_bShowFilePanel=false;
	}	
	if(m_piBTEntityExit==piControl)
	{
		ProcessFileExit();
		m_bShowFilePanel=false;
	}
	if(m_piBTShowOptionsPanel==piControl){m_bShowOptionsPanel=!m_bShowOptionsPanel;}
	if(m_piBTShowFilePanel==piControl){m_bShowFilePanel=!m_bShowFilePanel;}
	if(m_piBTOptionsTextures==piControl){m_bTextures=!m_bTextures;}
	if(m_piBTOptionsSolid==piControl){m_bSolid=!m_bSolid;}
	if(m_piBTNewModel==piControl){ProcessNewModel();}
	if(m_piBTNewSound==piControl){ProcessNewSound();}
	if(m_piBTNewEvent==piControl){ProcessNewEvent();}
	if(m_piBTNewParticleSystem==piControl){ProcessNewParticleSystem();}
	if(m_piBTNewAnimation==piControl){ProcessNewAnimation();}
	
	
	UpdateVisiblePanels();
}

void CEntityEditorMainWindow::UpdateCaption()
{
	if(m_Viewport.m_piViewport)
	{
		std::string sCaption="Entity Editor";
		if(m_sEntityName!=""){sCaption+=" - "+m_sEntityName;}
		if(m_EntityType.m_piEntityType){sCaption+=" ("+m_EntityType.m_piObject->GetClass()+")";}
		m_Viewport.m_piViewport->SetCaption(sCaption);
	}
}

void CEntityEditorMainWindow::OnCharacter( int nKey,bool *pbProcessed )
{
	if     (nKey=='T' || nKey=='t'){m_bTextures=!m_bTextures;*pbProcessed=true;}
	else if(nKey=='L' || nKey=='l'){m_bSolid=!m_bSolid;*pbProcessed=true;}
}

void CEntityEditorMainWindow::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_F2){ProcessFileSave();UpdateVisiblePanels();*pbProcessed=true;}
	else if(nKey==GK_F3){ProcessFileOpen();UpdateVisiblePanels();*pbProcessed=true;}
	else if(nKey==GK_PAUSE){m_FrameManager.m_piFrameManager->TogglePauseOnNextFrame();*pbProcessed=true;}
	else if(nKey==GK_HOME){CenterCamera();*pbProcessed=true;}
	else if(nKey==GK_NUMPAD5){CenterCamera(eEntityEditorView_Top);*pbProcessed=true;}
	else if(nKey==GK_NUMPAD4){CenterCamera(eEntityEditorView_Left);*pbProcessed=true;}
	else if(nKey==GK_NUMPAD8){CenterCamera(eEntityEditorView_Front);*pbProcessed=true;}
	else if(nKey==GK_NUMPAD2){CenterCamera(eEntityEditorView_Back);*pbProcessed=true;}
	else if(nKey==GK_NUMPAD6){CenterCamera(eEntityEditorView_Right);*pbProcessed=true;}
	else if(nKey==GK_DELETE)
	{
		*pbProcessed=true;
	}
	else if(nKey==GK_INSERT)
	{
		*pbProcessed=true;
	}
	else if(nKey==GK_ESCAPE)
	{
		ProcessFileExit();
		*pbProcessed=true;
	}
}

void CEntityEditorMainWindow::OnMouseDown( int nButton,double dx,double dy )
{
	int nCurrentId=100;
	m_Render.m_piRender->StartSelection(m_rRealRect,m_Camera.m_piCamera,dx,dy,5);
	nCurrentId=m_TranslationGizmo.SelectionRender(nCurrentId,m_Render.m_piRender,m_Camera.m_piCamera);
	nCurrentId=m_RotationGizmo.SelectionRender(nCurrentId,m_Render.m_piRender,m_Camera.m_piCamera);
	int nSelectionId=m_Render.m_piRender->EndSelection();
	m_TranslationGizmo.Select(nSelectionId);
	m_RotationGizmo.Select(nSelectionId);
	
	if(DetectDrag(dx,dy))
	{
		CLine mouseRay=GetMouseRay(dx,dy,10000.0,m_Camera.m_piCamera);
		if( (m_bShowTranslationGizmo && m_TranslationGizmo.BeginTranslation(&mouseRay,m_Camera.m_piCamera)) ||
			(m_bShowRotationGizmo && m_RotationGizmo.BeginRotation(&mouseRay,m_Camera.m_piCamera)) )
		{
			m_pEntity->GetPhysicInfo()->dMaxVelocity=0;
			
			m_bMovingGizmo=true;
			m_piGUIManager->SetMouseCapture(this);
		}
	}
}

void CEntityEditorMainWindow::OnMouseMove( double x,double y )
{
	if(m_bMovingGizmo)
	{
		CLine mouseRay=GetMouseRay(x,y,10000.0,m_Camera.m_piCamera);
		m_TranslationGizmo.ProcessTranslation(&mouseRay,m_Camera.m_piCamera);
		m_RotationGizmo.ProcessRotation(&mouseRay,m_Camera.m_piCamera);
		
		if(m_PositionWrapper.m_piDesign && m_pEntity)
		{
			m_PositionWrapper.m_piDesign->SetPosition(m_TranslationGizmo.GetPosition()-m_pEntity->GetPhysicInfo()->vPosition);
		}
		if(m_OrientationWrapper.m_piDesign && m_pEntity)
		{
			m_OrientationWrapper.m_piDesign->SetAngles(m_RotationGizmo.GetAngles());
		}
	}
	else
	{
		// Hover desactivado, en algunas tarjetas se hace por software y lo hace tan lento que es inmanejable
		
		//RTTRACE("CEntityEditorMainWindow::OnMouseMove -> %d %d",(int)x,(int)y);
		//int nCurrentId=100;
		//m_Render.m_piRender->StartSelection(m_rRealRect,m_Camera.m_piCamera,x,y,5);
		//nCurrentId=m_TranslationGizmo.SelectionRender(nCurrentId,m_Render.m_piRender,m_Camera.m_piCamera);
		//nCurrentId=m_RotationGizmo.SelectionRender(nCurrentId,m_Render.m_piRender,m_Camera.m_piCamera);
		//int nSelectionId=m_Render.m_piRender->EndSelection();
		//m_TranslationGizmo.Select(nSelectionId);
		//m_RotationGizmo.Select(nSelectionId);
	}
}

void CEntityEditorMainWindow::OnMouseUp( int nButton,double x,double y )
{
	if(m_piGUIManager->HasMouseCapture(this))
	{
		m_piGUIManager->ReleaseMouseCapture();
		m_TranslationGizmo.EndTranslation();
		m_RotationGizmo.EndRotation();	
		m_pEntity->GetPhysicInfo()->dMaxVelocity=10;
	}
	m_bMovingGizmo=false;
}

void CEntityEditorMainWindow::StopGameSimulation()
{
	if(m_bSimulationStarted)
	{
		m_GameControllerWrapper.m_piGameController->Stop();
		m_bSimulationStarted=false;
	}
}

void CEntityEditorMainWindow::StartGameSimulation()
{
	if(m_bSimulationStarted)
	{
		StopGameSimulation();
	}
	m_GameControllerWrapper.m_piGameController->Start();
	m_bSimulationStarted=true;
}

void CEntityEditorMainWindow::UpdateVisiblePanels()
{
	if(m_piGROptionsPanel){m_piGROptionsPanel->Show(m_bShowOptionsPanel);}
	if(m_piGREntity){m_piGREntity->Show(m_bShowFilePanel);}
}

void CEntityEditorMainWindow::UpdateStateList()
{
	if(m_piLSStates==NULL){return;}

	m_piLSStates->Clear();
	if(m_EntityType.m_piEntityType)
	{
		for(unsigned int x=0;x<m_EntityType.m_piEntityTypeDesign->GetStateCount();x++)
		{
			std::string sName;
			m_EntityType.m_piEntityTypeDesign->GetStateName(x,&sName);
			m_piLSStates->AddElement(sName);
		}
	}
	if(m_piLSStates->GetElementCount())
	{
		m_piLSStates->SetSelectedElement(0);
	}
	UpdateAnimationList();
}

void CEntityEditorMainWindow::UpdateAnimationList()
{
	if(m_piLSStates==NULL){return;}
	if(m_piLSAnimations==NULL){return;}
	m_piLSAnimations->Clear();
	m_vAnimations.clear();
	
	int nState=m_piLSStates->GetSelectedElement();
	
	if(nState>=0 && m_EntityType.m_piEntityType)
	{
		for(unsigned int x=0;x<m_EntityType.m_piEntityTypeDesign->GetStateAnimationCount(nState);x++)
		{
			IAnimationType *piAnimation=NULL;
			m_EntityType.m_piEntityTypeDesign->GetStateAnimation(nState,x,&piAnimation);
			
			CAnimationDesignTypeWrapper wrapper;
			if(piAnimation && wrapper.Attach(piAnimation))
			{
				m_vAnimations.push_back(wrapper);
				char sName[128];
				sprintf(sName,"Animation %d",x);
				m_piLSAnimations->AddElement(sName);
			}
			REL(piAnimation);
		}
	}
	if(m_piLSAnimations->GetElementCount())
	{
		m_piLSAnimations->SetSelectedElement(0);
	}
	UpdateSelectedAnimation();
}

void CEntityEditorMainWindow::UpdateSelectedAnimation()
{
	UpdateObjectList();
	UpdateRunningAnimation();	
}


void CEntityEditorMainWindow::UpdateObjectList()
{
	if(m_piLSObjects==NULL){return;}
	m_piLSObjects->Clear();
	
	int nAnimation=m_piLSAnimations->GetSelectedElement();
	if(nAnimation!=-1)
	{
		IAnimationTypeDesign *piAnimation=m_vAnimations[nAnimation].m_piAnimationTypeDesign;
		
		for(unsigned int x=0;x<piAnimation->GetObjectCount();x++)
		{
			IAnimationObjectType *piAnimationObject=NULL;
			piAnimation->GetObject(x,&piAnimationObject);
			m_piLSObjects->AddElement(piAnimationObject->GetAnimationObjectDescription());
			REL(piAnimationObject);
		}
	}

	UpdateSelectedObject();
}

void CEntityEditorMainWindow::UpdateSelectedObject()
{
	if(m_piLSAnimations==NULL){return;}
	if(m_piLSObjects==NULL){return;}

	m_PositionWrapper.Detach();
	m_OrientationWrapper.Detach();

	int nSelectedAnimation=(m_piLSAnimations->GetSelectedElement());
	int nSelectedObject=(m_piLSObjects->GetSelectedElement());
	
	IAnimationObjectType *piAnimationObject=NULL;
	ISystemObject *piObject=NULL;

	if(nSelectedAnimation!=-1)
	{
		if(nSelectedObject!=-1)
		{
			m_vAnimations[nSelectedAnimation].m_piAnimationTypeDesign->GetObject(nSelectedObject,&piAnimationObject);
			piObject=QI(ISystemObject,piAnimationObject);
		}
		else
		{
			piObject=ADD(m_vAnimations[nSelectedAnimation].m_piObject);
		}
	}
		
	for(unsigned int x=0;x<ePropertyPanel_Count;x++)
	{
		if(m_ppiPropertyPanels[x]==NULL){continue;}
		if(!m_ppiPropertyPanels[x]->SetObject(piObject))
		{
			m_ppiPropertyPanels[x]->Show(false);
		}
		else
		{
			m_ppiPropertyPanels[x]->Show(piObject!=NULL);
		}
	}
	m_PositionWrapper.Attach(piObject);
	m_OrientationWrapper.Attach(piObject);
	
	m_bShowTranslationGizmo=false;
	m_bShowRotationGizmo=false;
	
	if(m_PositionWrapper.m_piDesign){m_bShowTranslationGizmo=true;m_TranslationGizmo.SetPosition(m_PositionWrapper.m_piDesign->GetPosition());}
	if(m_OrientationWrapper.m_piDesign){m_bShowRotationGizmo=true;m_RotationGizmo.SetAngles(m_OrientationWrapper.m_piDesign->GetAngles());}
	
	REL(piObject);
}

void CEntityEditorMainWindow::UpdateRunningAnimation()
{
	delete m_piAnimation;
	m_piAnimation=NULL;
	if(m_pEntity)
	{
		CVector vNewCameraPos=m_Camera.m_piCamera->GetPosition()-m_pEntity->GetPhysicInfo()->vPosition;
		m_Camera.m_piCamera->SetPosition(vNewCameraPos);
		m_pEntity->GetPhysicInfo()->vPosition=Origin;
	}
	if(m_piLSAnimations==NULL){return;}
	int nSelected=(m_piLSAnimations->GetSelectedElement());
	if(nSelected==-1){return;}	
	
	m_piAnimation=m_vAnimations[nSelected].m_piAnimationType->CreateInstance(m_pEntity,m_FrameManager.m_piFrameManager->GetCurrentRealTime());
	if(m_piAnimation){m_piAnimation->Activate(m_FrameManager.m_piFrameManager->GetCurrentRealTime());}
}

void CEntityEditorMainWindow::OnSelectionChanged(IGameGUIList *piControl,int nElement,std::string sElement)
{
	if(piControl==m_piLSStates)
	{
		UpdateAnimationList();
	}
	if(piControl==m_piLSAnimations)
	{
		UpdateSelectedAnimation();
	}
	if(piControl==m_piLSObjects)
	{
		UpdateSelectedObject();
	}
}
void CEntityEditorMainWindow::OnSelectionDoubleCliked(IGameGUIList *piControl,int nElement,std::string sElement){}
void CEntityEditorMainWindow::OnWantFocus(bool *pbWant){*pbWant=true;}

void CEntityEditorMainWindow::CenterCamera(EEntityEditorView eView)
{
	if(m_piLSAnimations==NULL){return;}
	int nSelected=(m_piLSAnimations->GetSelectedElement());
	if(nSelected==-1){return;}
	
	
	bool bCenter=false;
	CVector vCenter,vSize;
	CVector vMins,vMaxs;
	m_vAnimations[nSelected].m_piAnimationType->DesignGetBBox(&vMins,&vMaxs);
	bCenter=true;
	vCenter+=(vMaxs+vMins)*0.5;
	vSize=(vMaxs-vMins);

	double dNearPlane=0,dFarPlane=0;
	m_Camera.m_piCamera->GetClippingPlanes(dNearPlane,dFarPlane);
	double dAspect=m_Camera.m_piCamera->GetAspectRatio();
	
	CVector vPos;
	if(eView==eEntityEditorView_Perspective)
	{
		vPos.c[0]=vCenter.c[0];
		vPos.c[1]=vCenter.c[1]+fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5))*(vSize.c[0]*0.5))+dNearPlane;
		vPos.c[2]=vCenter.c[2]+fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5))*(vSize.c[0]*0.5))*m_Camera.m_piCamera->GetAspectRatio()+dNearPlane;
	}
	else if(eView==eEntityEditorView_Left)
	{
		double dDistance=vSize.c[0]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))*dAspect);
		dDistance=std::max(dDistance,vSize.c[1]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))));
		dDistance=std::max(dDistance,dNearPlane);
		vPos.c[0]=vCenter.c[0];
		vPos.c[1]=vCenter.c[1];
		vPos.c[2]=vMins.c[2]-dDistance;
	}
	else if(eView==eEntityEditorView_Top)
	{
		double dDistance=vSize.c[2]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))*dAspect);
		dDistance=std::max(dDistance,vSize.c[0]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))));
		dDistance=std::max(dDistance,dNearPlane);
		vPos.c[0]=vCenter.c[0];
		vPos.c[1]=vMaxs.c[1]+dDistance;
		vPos.c[2]=vCenter.c[2];
	}
	else if(eView==eEntityEditorView_Bottom)
	{
		double dDistance=vSize.c[2]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))*dAspect);
		dDistance=std::max(dDistance,vSize.c[0]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))));
		dDistance=std::max(dDistance,dNearPlane);
		vPos.c[0]=vCenter.c[0];
		vPos.c[1]=vMins.c[1]-dDistance;
		vPos.c[2]=vCenter.c[2];
	}
	else if(eView==eEntityEditorView_Right)
	{
		double dDistance=vSize.c[0]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))*dAspect);
		dDistance=std::max(dDistance,vSize.c[1]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))));
		dDistance=std::max(dDistance,dNearPlane);
		vPos.c[0]=vCenter.c[0];
		vPos.c[1]=vCenter.c[1];
		vPos.c[2]=vMaxs.c[2]+dDistance;
	}
	else if(eView==eEntityEditorView_Front)
	{
		double dDistance=vSize.c[2]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))*dAspect);
		dDistance=std::max(dDistance,vSize.c[1]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))));
		dDistance=std::max(dDistance,dNearPlane);
		vPos.c[0]=vMaxs.c[0]+dDistance;
		vPos.c[1]=vCenter.c[1];
		vPos.c[2]=vCenter.c[2];
	}
	else if(eView==eEntityEditorView_Back)
	{
		double dDistance=vSize.c[2]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))*dAspect);
		dDistance=std::max(dDistance,vSize.c[1]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))));
		dDistance=std::max(dDistance,dNearPlane);
		vPos.c[0]=vMins.c[0]-dDistance;
		vPos.c[1]=vCenter.c[1];
		vPos.c[2]=vCenter.c[2];
	}
	CVector vAngles;
	AnglesFromVector(vCenter-vPos,&vAngles);
	if(m_pEntity){vPos+=m_pEntity->GetPhysicInfo()->vPosition;}
	m_Camera.m_piCamera->SetPosition(vPos);
	m_Camera.m_piCamera->SetAngles(vAngles);
}

void CEntityEditorMainWindow::ProcessNewAnimation()
{
	if(m_piLSStates==NULL){return;}
	if(m_piLSAnimations==NULL){return;}
	
	int nSelected=(m_piLSStates->GetSelectedElement());
	if(nSelected==-1)
	{
		MessageDialog("No state selected.","Entity Editor",eMessageDialogType_Error);
		return;
	}
	
	if(ConfirmDialog("Add new animation for state '"+m_piLSStates->GetElement(nSelected)+"'?","Entity Editor",eMessageDialogType_Warning))
	{
		unsigned int nElement=m_EntityType.m_piEntityTypeDesign->AddStateAnimation(nSelected);
		
		UpdateAnimationList();
		m_piLSAnimations->SetSelectedElement(nElement);
		UpdateSelectedAnimation();
	}
}

void CEntityEditorMainWindow::ProcessNewModel()
{
	if(m_piLSAnimations==NULL){return;}
	int nSelected=(m_piLSAnimations->GetSelectedElement());
	if(nSelected==-1)
	{
		MessageDialog("No animation selected.","Entity Editor",eMessageDialogType_Error);
		return;
	}
	
	bool bOk=false;
	std::string sBaseModel="./Models/";
	
	CModelWrapper model;
	
	if(OpenFileDialog("Select Model...",".ase;.ASE",&sBaseModel))
	{
		std::vector<IGenericModel*> vModels;
		GetSystemObjects("GameResources",&vModels);
		
		bool bNew=true;
		for(unsigned int x=0;x<vModels.size();x++)
		{
			IGenericModel *piModel=vModels[x];
			if(piModel->GetFileName()==sBaseModel)
			{
				bNew=false;
				model.Attach(piModel);
				break;
			}
		}
		if(bNew)
		{
			if(ConfirmDialog("Add new model " +sBaseModel+" to the resource repository?","Entity Editor",eMessageDialogType_Warning))
			{
				if(model.Create("GameResources","Model",sBaseModel))
				{
					if(model.m_piModel->Load(sBaseModel))
					{
						CSystemSaverHelper helper;
						bOk=helper.SaveSystemNamedObjects("Scripts/GameResources.cfg","GameResources");
					}
					else
					{
						model.Destroy();
						MessageDialog("Failed to load model","Entity Editor",eMessageDialogType_Error);
					}
				}
				else
				{
					model.Destroy();
					MessageDialog("Failed to create model instance","Entity Editor",eMessageDialogType_Error);
				}
			}
		}
		
		if(model.m_piModel)
		{
			IAnimationObjectType *piAnimationObject=NULL;
			CAnimationModelTypeWrapper animationObject;
			unsigned int nIndex=m_vAnimations[nSelected].m_piAnimationTypeDesign->AddObject("ModelAnimationObjectType");
			m_vAnimations[nSelected].m_piAnimationTypeDesign->GetObject(nIndex,&piAnimationObject);
			if(animationObject.Attach(piAnimationObject))
			{
				animationObject.m_piDesign->SetModel(model.m_piModel);
			}
			REL(piAnimationObject);
		
			UpdateSelectedAnimation();
		}
			
		
		for(unsigned int x=0;x<vModels.size();x++)
		{
			IGenericModel *piModel=vModels[x];
			REL(piModel);
		}
	}
}

void CEntityEditorMainWindow::ProcessNewSound()
{
	if(m_piLSAnimations==NULL){return;}
	int nSelected=(m_piLSAnimations->GetSelectedElement());
	if(nSelected==-1)
	{
		MessageDialog("No animation selected.","Entity Editor",eMessageDialogType_Error);
		return;
	}
	
	bool bOk=false;
	std::string sBaseSound="./Sounds/";
	
	CSoundWrapper model;
	
	if(OpenFileDialog("Select Sound...",".wav;.WAV",&sBaseSound))
	{
		std::vector<ISoundType*> vSounds;
		GetSystemObjects("GameResources",&vSounds);
		
		bool bNew=true;
		for(unsigned int x=0;x<vSounds.size();x++)
		{
			ISoundType *piSound=vSounds[x];
			if(piSound->GetFileName()==sBaseSound)
			{
				bNew=false;
				model.Attach(piSound);
				break;
			}
		}
		if(bNew)
		{
			if(ConfirmDialog("Add new model " +sBaseSound+" to the resource repository?","Entity Editor",eMessageDialogType_Warning))
			{
				if(model.Create("GameResources","SoundType",sBaseSound))
				{
					if(model.m_piSound->Load(sBaseSound))
					{
						CSystemSaverHelper helper;
						bOk=helper.SaveSystemNamedObjects("Scripts/GameResources.cfg","GameResources");
					}
					else
					{
						model.Destroy();
						MessageDialog("Failed to load model","Entity Editor",eMessageDialogType_Error);
					}
				}
				else
				{
					model.Destroy();
					MessageDialog("Failed to create model instance","Entity Editor",eMessageDialogType_Error);
				}
			}
		}
		
		if(model.m_piSound)
		{
			IAnimationObjectType *piAnimationObject=NULL;
			CAnimationSoundTypeWrapper animationObject;
			unsigned int nIndex=m_vAnimations[nSelected].m_piAnimationTypeDesign->AddObject("SoundAnimationObjectType");
			m_vAnimations[nSelected].m_piAnimationTypeDesign->GetObject(nIndex,&piAnimationObject);
			if(animationObject.Attach(piAnimationObject))
			{
				animationObject.m_piDesign->SetSound(model.m_piSound);
			}
			REL(piAnimationObject);
			
			UpdateSelectedAnimation();
		}
		
		
		for(unsigned int x=0;x<vSounds.size();x++)
		{
			ISoundType *piSound=vSounds[x];
			REL(piSound);
		}
	}
}
void CEntityEditorMainWindow::ProcessNewEvent()
{
	if(m_piLSAnimations==NULL){return;}
	int nSelected=(m_piLSAnimations->GetSelectedElement());
	if(nSelected==-1)
	{
		MessageDialog("No animation selected.","Entity Editor",eMessageDialogType_Error);
		return;
	}

	std::string sEventName;
	while(sEventName.length()==0)
	{
		if(!InputDialog(&sEventName,"New Event")){return;}
		if(sEventName.length()==0)
		{
			MessageDialog("Cannot create an unnamed event","Entity Editor",eMessageDialogType_Error);
		}
	}


	IAnimationObjectType *piAnimationObject=NULL;
	CAnimationEventTypeWrapper animationObject;
	unsigned int nIndex=m_vAnimations[nSelected].m_piAnimationTypeDesign->AddObject("EventAnimationObjectType");
	m_vAnimations[nSelected].m_piAnimationTypeDesign->GetObject(nIndex,&piAnimationObject);
	if(animationObject.Attach(piAnimationObject))
	{
		SEventAnimationObjectTypeConfig sConfig;
		sConfig.sName=sEventName;
		animationObject.m_piDesign->SetConfig(&sConfig);
	}
	REL(piAnimationObject);

	UpdateSelectedAnimation();
}

void CEntityEditorMainWindow::ProcessNewParticleSystem()
{
	if(m_NamedObjectSelector.m_piObjectSelector==NULL){return;}
	if(m_piLSAnimations==NULL){return;}
	int nSelected=(m_piLSAnimations->GetSelectedElement());
	if(nSelected==-1)
	{
		MessageDialog("No animation selected.","Entity Editor",eMessageDialogType_Error);
		return;
	}
	std::string sObject;
	if(m_NamedObjectSelector.m_piObjectSelector->SelectObject(this,"Select Particle System...","ParticleSystems",&sObject))
	{
		CParticleSystemWrapper particleSystemWrapper;
		particleSystemWrapper.Attach("ParticleSystems",sObject);
		
		if(particleSystemWrapper.m_piParticleSystem)
		{
			IAnimationObjectType *piAnimationObject=NULL;
			CAnimationParticleSystemTypeWrapper animationObject;
			unsigned int nIndex=m_vAnimations[nSelected].m_piAnimationTypeDesign->AddObject("ParticleSystemAnimationObjectType");
			m_vAnimations[nSelected].m_piAnimationTypeDesign->GetObject(nIndex,&piAnimationObject);
			if(animationObject.Attach(piAnimationObject))
			{
				SParticleSystemAnimationObjectTypeConfig config;
				config.bTrackEntity=true;
				animationObject.m_piDesign->SetConfig(&config);
				animationObject.m_piDesign->SetParticleSystemType(particleSystemWrapper.m_piParticleSystem);
			}
			REL(piAnimationObject);
			
			UpdateSelectedAnimation();
		}
	}
}

void CEntityEditorMainWindow::OnObjectChanged(IEntityEditorPropertyPanel *piPanel,ISystemObject *piObject)
{
	if(m_piLSAnimations==NULL){return;}
	if(m_piLSObjects==NULL){return;}
	
	int nSelectedAnimation=(m_piLSAnimations->GetSelectedElement());
	
	for(unsigned int x=0;x<m_vAnimations[nSelectedAnimation].m_piAnimationTypeDesign->GetObjectCount();x++)
	{
		IAnimationObjectType *piAnimationObject=NULL;
		m_vAnimations[nSelectedAnimation].m_piAnimationTypeDesign->GetObject(x,&piAnimationObject);
		if((ISystemUnknown*)piObject==(ISystemUnknown*)piAnimationObject)
		{
			m_piLSObjects->SetElement(x,piAnimationObject->GetAnimationObjectDescription());
			REL(piAnimationObject);
			break;
		}
		REL(piAnimationObject);
	}
	
	UpdateRunningAnimation();
}

void CEntityEditorMainWindow::OnObjectRemoved(IEntityEditorPropertyPanel *piPanel,ISystemObject *piObject)
{
	if(m_piLSStates==NULL){return;}
	if(m_piLSAnimations==NULL){return;}
	if(m_piLSObjects==NULL){return;}

	int nSelectedState=(m_piLSStates->GetSelectedElement());
	int nSelectedAnimation=(m_piLSAnimations->GetSelectedElement());
	int nSelectedObject=(m_piLSObjects->GetSelectedElement());

	if(nSelectedAnimation!=-1 && (ISystemUnknown*)piObject==(ISystemUnknown*)m_vAnimations[nSelectedAnimation].m_piObject)
	{
		m_EntityType.m_piEntityTypeDesign->RemoveStateAnimation(nSelectedState,nSelectedAnimation);
		UpdateAnimationList();
	}
	if(nSelectedObject!=-1)
	{
		for(unsigned int x=0;x<m_vAnimations[nSelectedAnimation].m_piAnimationTypeDesign->GetObjectCount();x++)
		{
			IAnimationObjectType *piAnimationObject=NULL;
			m_vAnimations[nSelectedAnimation].m_piAnimationTypeDesign->GetObject(x,&piAnimationObject);
			if((ISystemUnknown*)piObject==(ISystemUnknown*)piAnimationObject)
			{
				m_vAnimations[nSelectedAnimation].m_piAnimationTypeDesign->RemoveObject(x);
				REL(piAnimationObject);
				break;
			}
			REL(piAnimationObject);
		}
		UpdateSelectedAnimation();
	}
}
