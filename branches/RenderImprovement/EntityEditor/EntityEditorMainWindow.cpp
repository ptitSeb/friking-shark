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
#include "EntityEditorMainWindow.h"
#include "../GameRunTime/GameRunTime.h"

CEntityEditorMainWindow::CEntityEditorMainWindow(void)
{
	m_d3DFontSize=0;
	m_eMode=eEntityEditorMode_EntityProperties;
	m_eBBoxGroup=eEntityEditorBBoxGroup_Collisions;
	
	m_bShowTranslationGizmo=false;
	m_bShowRotationGizmo=false;
	m_bShowBBoxGizmo=false;
	
	m_bMovingGizmo=false;
	m_piAnimation=NULL;
	m_piStaticStructureTypeDesign=NULL;
	
	m_bShowOptionsPanel=false;
	m_bShowFilePanel=false;
	
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
	m_nAnimationActivationTime=0;

	UpdateInteractiveElementsSpeedsAndSizes();

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
//	m_pEntity->GetPhysicInfo()->dMaxVelocity=10;

	m_FrameManager.Attach("GameSystem","FrameManager");
	m_PhysicManagerWrapper.Attach("GameSystem","PhysicManager");
	m_SoundManagerWrapper.Attach("GameGUI","SoundManager");
	UpdateVisiblePanels();
	UpdateStateList();
	UpdateBBoxList();
	return bOk;
}

void CEntityEditorMainWindow::DestroyWindow()
{
	StopGameSimulation();
	Reset();
	UpdateStateList();
	UpdateBBoxList();
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
	REL(m_piStaticStructureTypeDesign);
	
	StopGameSimulation();
	m_EntityType.Detach();

	m_bShowFilePanel=false;
	m_bShowOptionsPanel=false;
	m_bSimulationStarted=false;
	m_bInspectionMode=false;
	m_sEntityName="";
	m_eBBoxGroup=eEntityEditorBBoxGroup_Collisions;
	

	UpdateCaption();
	UpdateStateList();
	UpdateBBoxList();
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
	double dCameraRotationSpeed=90.0;
	double dCameraForwardSpeed=m_dCameraSpeed*dRealTimeFraction*3.0;
	if(m_piGUIManager->IsKeyDown(GK_LSHIFT)){dCameraForwardSpeed*=3.0;dCameraRotationSpeed*=2.0;}
	if(m_piGUIManager->IsKeyDown(GK_LCONTROL))
	{
		CVector vChange;
		if(nKey==KEY_FORWARD)	{vChange.c[PITCH]-=dRealTimeFraction*dCameraRotationSpeed;}
		else if(nKey==KEY_BACK)	{vChange.c[PITCH]+=dRealTimeFraction*dCameraRotationSpeed;}
		else if(nKey==KEY_LEFT)	{vChange.c[YAW]+=dRealTimeFraction*dCameraRotationSpeed;}
		else if(nKey==KEY_RIGHT){vChange.c[YAW]-=dRealTimeFraction*dCameraRotationSpeed;}
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
	
	m_Camera.m_piCamera->SetAspectRatio(m_rRealRect.w/m_rRealRect.h);

	m_FrameManager.m_piFrameManager->ProcessFrame();
	
	ProcessInput(m_FrameManager.m_piFrameManager->GetTimeFraction(),m_FrameManager.m_piFrameManager->GetRealTimeFraction());
	
	if(m_piAnimation)
	{
		m_piAnimation->ProcessFrame(m_PhysicManagerWrapper.m_piPhysicManager,m_FrameManager.m_piFrameManager->GetCurrentRealTime(),m_FrameManager.m_piFrameManager->GetRealTimeFraction());
		if(m_piAnimation->HasFinished())
		{
			m_nAnimationActivationTime=m_FrameManager.m_piFrameManager->GetCurrentRealTime();
			m_piAnimation->Activate(m_nAnimationActivationTime);		
		}
		
		CVector vUpdate=Origin;
		if(m_pEntity)
		{
			vUpdate+=AxisPosX*m_pEntity->GetPhysicInfo()->dMaxVelocity*m_FrameManager.m_piFrameManager->GetTimeFraction();
			m_pEntity->GetPhysicInfo()->vPosition+=vUpdate;
		}
		CVector vNewCameraPos=m_Camera.m_piCamera->GetPosition()+vUpdate;
		m_Camera.m_piCamera->SetPosition(vNewCameraPos);
	}
	
	m_Render.m_piRender->PushOptions();
	m_Render.m_piRender->PushState();
	m_Render.m_piRender->ActivateDepth();
	
	m_bTextures?m_Render.m_piRender->EnableTextures():m_Render.m_piRender->DisableTextures();
	m_bSolid?m_Render.m_piRender->EnableSolid():m_Render.m_piRender->DisableSolid();
	m_Render.m_piRender->EnableBlending();
	m_Render.m_piRender->EnableShadows();
	m_Render.m_piRender->EnableLighting();
	m_Render.m_piRender->EnableAutoShadowVolume();
	m_Render.m_piRender->DisableHeightFog();
	
	
	STerrainSun sSun;
	sSun.dDistance=1.5;
	sSun.dAzimuth=30;
	sSun.dElevation=60;
	sSun.vColor=CVector(0.5,0.5,0.5);
	
	CVector vSunAngles(sSun.dAzimuth,sSun.dElevation,0);
	CVector vForward;
	CVector vBlack;
	VectorsFromAngles(vSunAngles,&vForward);
	double dTerrainMaxDimension=1000.0;
	CVector vSunPos=vForward*(dTerrainMaxDimension*sSun.dDistance);
	piRender->SetSunLight(vSunPos,vForward,vBlack,sSun.vColor,sSun.vColor);
	piRender->SetAmbientLight(CVector(0.4,0.4,0.4));

	SetupRenderOptions(piRender,m_Camera.m_piCamera);
	
	piRender->StartStagedRendering();
	piRender->ClearDepth();
	piRender->Clear(m_vBackgroundColor);
	
	if(m_piAnimation){m_piAnimation->CustomRender(m_Render.m_piRender,m_Camera.m_piCamera);}
	// Pintado de las entidades hijas
	if(m_pEntity && m_EntityType.m_piEntityTypeDesign)
	{
		for(unsigned int x=0;x<m_EntityType.m_piEntityTypeDesign->GetChildren();x++)
		{
			CVector vPosition,vAngles;
			IEntityType *piType=NULL;
			m_EntityType.m_piEntityTypeDesign->GetChild(x,&piType);
			m_EntityType.m_piEntityTypeDesign->GetChildLocation(x,vPosition,vAngles);
			vPosition+=m_pEntity->GetPhysicInfo()->vPosition;
			if(piType){piType->DesignRender(piRender,vPosition,vAngles,false);}
			if(m_piLSChildren && m_piLSChildren->GetSelectedElement()==(int)x){piType->DesignRender(piRender,vPosition,vAngles,true);}
			REL(piType);
		}
	}
	// Pintado de las entidades generadas. Es necesario hacer un pintado ad-hoc porque no
	// tenemos entity manager ni playareamanager, y por lo tanto no se generan entidades.
	
	int nAnimation=m_piLSAnimations->GetSelectedElement();
	if(m_pEntity && nAnimation!=-1)
	{
		IAnimationTypeDesign *piAnimation=m_vAnimations[nAnimation].m_piAnimationTypeDesign;
		for(unsigned int x=0;x<piAnimation->GetObjectCount();x++)
		{
			IAnimationObjectType *piAnimationObject=NULL;
			piAnimation->GetObject(x,&piAnimationObject);
			
			CAnimationEntityTypeWrapper wrapper;
			wrapper.Attach(piAnimationObject);
			if(wrapper.m_piDesign)
			{
				SEntityAnimationObjectTypeConfig config;
				wrapper.m_piDesign->GetConfig(&config);
				
				if((m_nAnimationActivationTime+config.nTime)<m_FrameManager.m_piFrameManager->GetCurrentRealTime())
				{
					wrapper.m_piDesign->DesignRender(piRender,m_pEntity->GetPhysicInfo()->vPosition,m_pEntity->GetPhysicInfo()->vAngles,false);
				}
			}
			REL(piAnimationObject);
		}
	}
	piRender->EndStagedRendering();

	if(!m_bMovingGizmo && m_pEntity)
	{
		if(m_PositionWrapper.m_piDesign)
		{
			m_TranslationGizmo.SetPosition(m_pEntity->GetPhysicInfo()->vPosition+m_PositionWrapper.m_piDesign->GetPosition());
		}
		else
		{
			if(m_piLSChildren && m_EntityType.m_piEntityTypeDesign)
			{
				int nSelectedEntity=m_piLSChildren->GetSelectedElement();
				if(nSelectedEntity!=-1)
				{
					CVector vPosition,vAngles;
					m_EntityType.m_piEntityTypeDesign->GetChildLocation(nSelectedEntity,vPosition,vAngles);
					m_TranslationGizmo.SetPosition(m_pEntity->GetPhysicInfo()->vPosition+vPosition);
				}
			}
		}
	}
	if(!m_bMovingGizmo && m_pEntity)
	{
		m_BBoxGizmo.SetPosition(m_pEntity->GetPhysicInfo()->vPosition);
		m_BBoxGizmo.SetArrowSize((m_Camera.m_piCamera->GetPosition()-m_pEntity->GetPhysicInfo()->vPosition)/15.0);
	}
		
	m_TranslationGizmo.SetSize((m_Camera.m_piCamera->GetPosition()-m_TranslationGizmo.GetPosition())/7.5);
	m_RotationGizmo.SetRadius((m_Camera.m_piCamera->GetPosition()-m_TranslationGizmo.GetPosition())/10.0);
	m_RotationGizmo.SetPosition(m_TranslationGizmo.GetPosition());
	if(m_bShowTranslationGizmo){m_TranslationGizmo.Render(m_Render.m_piRender,m_Camera.m_piCamera);}
	if(m_bShowRotationGizmo){m_RotationGizmo.Render(m_Render.m_piRender,m_Camera.m_piCamera);}
	if(m_bShowBBoxGizmo)
	{
		m_BBoxGizmo.Render(m_Render.m_piRender,m_Camera.m_piCamera);
		
		std::vector<SBBox> vBBoxes;
		GetBBoxGroup(&vBBoxes);
		
		for(unsigned int b=0;b<vBBoxes.size();b++)
		{
			piRender->RenderBBox(m_pEntity->GetPhysicInfo()->vPosition,Origin,vBBoxes[b].vMins,vBBoxes[b].vMaxs,0x8888,CVector(1,1,1),1.0);
		}
	}
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
	m_piGREntityProperties->Show(m_eMode==eEntityEditorMode_EntityProperties);
	m_piGRGraphicProperties->Show(m_eMode==eEntityEditorMode_GraphicProperties);
	m_piBTShowEntityProperties->SetBackgroundColor(CVector(1,1,1),m_eMode==eEntityEditorMode_EntityProperties?0.5:0.3);
	m_piBTShowGraphicProperties->SetBackgroundColor(CVector(1,1,1),m_eMode==eEntityEditorMode_GraphicProperties?0.5:0.3);
	
	m_piBTBBoxGroupCollisions->SetBackgroundColor(CVector(1,1,1),m_eBBoxGroup==eEntityEditorBBoxGroup_Collisions?0.5:0.3);
	
	m_piBTBBoxGroupVulnerable->Activate(m_piStaticStructureTypeDesign!=NULL);
	m_piBTBBoxGroupProtectiveNormal->Activate(m_piStaticStructureTypeDesign!=NULL);
	m_piBTBBoxGroupProtectiveDestroyed->Activate(m_piStaticStructureTypeDesign!=NULL);
	
	if(m_piStaticStructureTypeDesign)
	{
		m_piBTBBoxGroupVulnerable->SetBackgroundColor(CVector(1,1,1),m_eBBoxGroup==eEntityEditorBBoxGroup_Vulnerable?0.5:0.3);
		m_piBTBBoxGroupProtectiveNormal->SetBackgroundColor(CVector(1,1,1),m_eBBoxGroup==eEntityEditorBBoxGroup_ProtectiveNormal?0.5:0.3);
		m_piBTBBoxGroupProtectiveDestroyed->SetBackgroundColor(CVector(1,1,1),m_eBBoxGroup==eEntityEditorBBoxGroup_ProtectiveDestroyed?0.5:0.3);
	}
	else
	{
		m_piBTBBoxGroupVulnerable->SetBackgroundColor(CVector(1,1,1),0.1);
		m_piBTBBoxGroupProtectiveNormal->SetBackgroundColor(CVector(1,1,1),0.1);
		m_piBTBBoxGroupProtectiveDestroyed->SetBackgroundColor(CVector(1,1,1),0.1);
	}
	m_piGUIManager->RestoreViewport();
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
		
		if(m_EntityType.m_piEntityTypeDesign)
		{
			SEntityTypeConfig config;
			m_piStaticStructureTypeDesign=QI(IStaticStructureTypeDesign,m_EntityType.m_piEntityTypeDesign);
			m_EntityType.m_piEntityTypeDesign->GetEntityTypeConfig(&config);
			m_BBoxGizmo.SetBounds(Origin,Origin);
		}
		
		UpdateCaption();
		UpdateStateList();
		UpdateBBoxList();
	}	
}

void CEntityEditorMainWindow::ProcessFileOpen()
{
	if(m_ObjectSelector.m_piObjectListSelector==NULL){return;}
	
	unsigned long nSelectedEntityType=0;
	std::vector<IDesignObject *> vEntityTypes;
	GetSystemObjects("EntityTypes",&vEntityTypes);

	if(m_ObjectSelector.m_piObjectListSelector->SelectObject("Open Entity...",this,&vEntityTypes,&nSelectedEntityType))
	{
		Reset();
		CConfigFile cfg;
		CEntityTypeWrapper existingWrapper;
		bool bOk=existingWrapper.Attach(vEntityTypes[nSelectedEntityType]);
		if(bOk){bOk=existingWrapper.m_piSerializable->Serialize(cfg.GetRoot());}
		if(bOk){bOk=m_EntityType.Create("EntityTypes",existingWrapper.m_piObject->GetClass(),"");}
		if(bOk){bOk=m_EntityType.m_piSerializable->Unserialize(cfg.GetRoot());}
		if(bOk){m_piStaticStructureTypeDesign=QI(IStaticStructureTypeDesign,m_EntityType.m_piEntityTypeDesign);}
		if(bOk)
		{
			SEntityTypeConfig config;
			m_EntityType.m_piEntityTypeDesign->GetEntityTypeConfig(&config);
			if(config.vBBoxes.size())
			{
				m_BBoxGizmo.SetBounds(config.vBBoxes[0].vMins,config.vBBoxes[0].vMaxs);
			}
			m_sEntityName=existingWrapper.m_piObject->GetName();
			UpdateCaption();
			UpdateStateList();
			UpdateBBoxList();
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
				if(existingWrapper.Attach("EntityTypes",sName,false) && sName!=m_sEntityName)
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
	
	if(m_ObjectSelector.m_piObjectListSelector->SelectObject("Remove Entity...",this,&vEntityTypes,&nSelectedEntityType))
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
				if(existingWrapper.Attach("EntityTypes",sName,false) && sName!=m_sEntityName)
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
	if(m_piBTNewChild==piControl){ProcessNewChild();}
	if(m_piBTRemoveChild==piControl){ProcessRemoveChild();}
	if(m_piBTNewBBox==piControl){ProcessNewBBox();}
	if(m_piBTRemoveBBox==piControl){ProcessRemoveBBox();}
	if(m_piBTNewEntity==piControl){ProcessNewEntity();}
	if(m_piBTShowEntityProperties==piControl){m_eMode=eEntityEditorMode_EntityProperties;UpdateSelectedObject();}
	if(m_piBTShowGraphicProperties==piControl){m_eMode=eEntityEditorMode_GraphicProperties;UpdateSelectedObject();}
	
	if(m_piBTBBoxGroupCollisions==piControl){m_eBBoxGroup=eEntityEditorBBoxGroup_Collisions;UpdateBBoxList();}
	if(m_piBTBBoxGroupVulnerable==piControl){m_eBBoxGroup=eEntityEditorBBoxGroup_Vulnerable;UpdateBBoxList();}
	if(m_piBTBBoxGroupProtectiveNormal==piControl){m_eBBoxGroup=eEntityEditorBBoxGroup_ProtectiveNormal;UpdateBBoxList();}
	if(m_piBTBBoxGroupProtectiveDestroyed==piControl){m_eBBoxGroup=eEntityEditorBBoxGroup_ProtectiveDestroyed;UpdateBBoxList();}
	
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
	/*	if(nKey=='I' && m_pEntity){m_pEntity->GetPhysicInfo()->vAngles.c[PITCH]+=5;if(m_pEntity->GetPhysicInfo()->vAngles.c[PITCH]>360){m_pEntity->GetPhysicInfo()->vAngles.c[PITCH]=90;}}
	 i f*(nKey=='K' && m_pEntity){m_pEntity->GetPhysicInfo()->vAngles.c[PITCH]-=5;if(m_pEntity->GetPhysicInfo()->vAngles.c[PITCH]<-0){m_pEntity->GetPhysicInfo()->vAngles.c[PITCH]=-0;}}
	 if(nKey=='J' && m_pEntity){m_pEntity->GetPhysicInfo()->vAngles.c[YAW]+=5;if(m_pEntity->GetPhysicInfo()->vAngles.c[YAW]>360){m_pEntity->GetPhysicInfo()->vAngles.c[YAW]-=360;}}
	 if(nKey=='L' && m_pEntity){m_pEntity->GetPhysicInfo()->vAngles.c[YAW]-=5;if(m_pEntity->GetPhysicInfo()->vAngles.c[YAW]<0){m_pEntity->GetPhysicInfo()->vAngles.c[YAW]+=360;}}
	 if(nKey=='N' && m_pEntity){m_pEntity->GetPhysicInfo()->vAngles.c[ROLL]+=5;if(m_pEntity->GetPhysicInfo()->vAngles.c[ROLL]>360){m_pEntity->GetPhysicInfo()->vAngles.c[ROLL]-=360;}}
	 if(nKey=='M' && m_pEntity){m_pEntity->GetPhysicInfo()->vAngles.c[ROLL]-=5;if(m_pEntity->GetPhysicInfo()->vAngles.c[ROLL]<0){m_pEntity->GetPhysicInfo()->vAngles.c[ROLL]+=360;}}
	 */	
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
	else if(nKey=='\t'){m_eMode=(m_eMode==eEntityEditorMode_EntityProperties)?eEntityEditorMode_GraphicProperties:eEntityEditorMode_EntityProperties;UpdateSelectedObject();*pbProcessed=true;}
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
	if(m_bShowTranslationGizmo){nCurrentId=m_TranslationGizmo.SelectionRender(nCurrentId,m_Render.m_piRender,m_Camera.m_piCamera);}
	if(m_bShowRotationGizmo)   {nCurrentId=m_RotationGizmo.SelectionRender(nCurrentId,m_Render.m_piRender,m_Camera.m_piCamera);}
	if(m_bShowBBoxGizmo)       {nCurrentId=m_BBoxGizmo.SelectionRender(nCurrentId,m_Render.m_piRender,m_Camera.m_piCamera);}
	int nSelectionId=m_Render.m_piRender->EndSelection();
	if(m_bShowTranslationGizmo){m_TranslationGizmo.Select(nSelectionId);}
	if(m_bShowRotationGizmo)   {m_RotationGizmo.Select(nSelectionId);}
	if(m_bShowBBoxGizmo)       {m_BBoxGizmo.Select(nSelectionId);}
	
	if(nSelectionId!=-1)
	{
		if(DetectDrag(dx,dy))
		{
			CLine mouseRay=GetMouseRay(dx,dy,10000.0,m_Camera.m_piCamera);
			if( (m_bShowTranslationGizmo && m_TranslationGizmo.BeginTranslation(&mouseRay,m_Camera.m_piCamera)) ||
				(m_bShowRotationGizmo && m_RotationGizmo.BeginRotation(&mouseRay,m_Camera.m_piCamera)) ||
				(m_bShowBBoxGizmo && m_BBoxGizmo.BeginBBox(&mouseRay,m_Camera.m_piCamera)))
			{
				//m_pEntity->GetPhysicInfo()->dMaxVelocity=0;
				
				m_bMovingGizmo=true;
				m_piGUIManager->SetMouseCapture(this);
				
				UpdateInteractiveElementsSpeedsAndSizes();
			}
		}
	}
	else
	{
		int nSelectedAnimation=m_piLSAnimations?m_piLSAnimations->GetSelectedElement():-1;
		if(!m_bMovingGizmo && m_pEntity && nSelectedAnimation!=-1)
		{
			unsigned long nTime=m_FrameManager.m_piFrameManager->GetCurrentRealTime()-m_nAnimationActivationTime;
			m_Render.m_piRender->PushOptions();
			m_Render.m_piRender->PushState();
			
			m_bTextures?m_Render.m_piRender->EnableTextures():m_Render.m_piRender->DisableTextures();
			m_bSolid?m_Render.m_piRender->EnableSolid():m_Render.m_piRender->DisableSolid();
			
			m_Render.m_piRender->StartSelection(m_rRealRect,m_Camera.m_piCamera,dx,dy,5);
			unsigned int nObjects=m_vAnimations[nSelectedAnimation].m_piAnimationTypeDesign->GetObjectCount();
			for(unsigned int x=0;x<nObjects;x++)
			{
				IAnimationObjectType *piObject=NULL;
				m_vAnimations[nSelectedAnimation].m_piAnimationTypeDesign->GetObject(x,&piObject);
				
				CAnimationModelTypeWrapper modelWrapper;
				CAnimationEntityTypeWrapper entityWrapper;
				
				bool bInTimeFrame=true;
				modelWrapper.Attach(piObject);
				entityWrapper.Attach(piObject);
				if(modelWrapper.m_piDesign)
				{
					SModelAnimationObjectTypeConfig config;
					modelWrapper.m_piDesign->GetConfig(&config);
					bInTimeFrame=(nTime>=config.nStartTime && (config.nEndTime==0 || nTime<config.nEndTime));
				}
				if(entityWrapper.m_piDesign)
				{
					SEntityAnimationObjectTypeConfig config;
					entityWrapper.m_piDesign->GetConfig(&config);
					bInTimeFrame=(nTime>=config.nTime);
				}
				if(bInTimeFrame &&piObject)
				{
					m_Render.m_piRender->SetSelectionId(x);
					piObject->DesignRender(m_Render.m_piRender,m_pEntity->GetPhysicInfo()->vPosition,m_pEntity->GetPhysicInfo()->vAngles,false);
				}
				REL(piObject);
			}
			
			if(m_EntityType.m_piEntityTypeDesign)
			{
				for(unsigned int x=0;x<m_EntityType.m_piEntityTypeDesign->GetChildren();x++)
				{
					m_Render.m_piRender->SetSelectionId(nObjects+x);
					CVector vPosition,vAngles;
					IEntityType *piType=NULL;
					m_EntityType.m_piEntityTypeDesign->GetChild(x,&piType);
					m_EntityType.m_piEntityTypeDesign->GetChildLocation(x,vPosition,vAngles);
					vPosition+=m_pEntity->GetPhysicInfo()->vPosition;
					if(piType){piType->DesignRender(m_Render.m_piRender,vPosition,vAngles,false);}
					REL(piType);
				}
			}
			
			int nSelectionId=m_Render.m_piRender->EndSelection();
			if(nSelectionId==-1)
			{
				if(m_piLSObjects){m_piLSObjects->SetSelectedElement(-1);}
				if(m_piLSChildren){m_piLSChildren->SetSelectedElement(-1);}
			}
			else if(nSelectionId<(int)nObjects)
			{
				if(m_piLSObjects){m_piLSObjects->SetSelectedElement(nSelectionId);}
				if(m_piLSChildren){m_piLSChildren->SetSelectedElement(-1);}
				m_eMode=eEntityEditorMode_GraphicProperties;
			}
			else
			{
				if(m_piLSObjects){m_piLSObjects->SetSelectedElement(-1);}
				if(m_piLSChildren){m_piLSChildren->SetSelectedElement(nSelectionId-nObjects);}
				m_eMode=eEntityEditorMode_EntityProperties;
			}
			m_Render.m_piRender->PopOptions();
			m_Render.m_piRender->PopState();
			
			UpdateSelectedObject();
		}
	}
	
}

void CEntityEditorMainWindow::OnMouseMove( double x,double y )
{
	if(m_bMovingGizmo)
	{
		CLine mouseRay=GetMouseRay(x,y,10000.0,m_Camera.m_piCamera);
		if(m_bShowTranslationGizmo){m_TranslationGizmo.ProcessTranslation(&mouseRay,m_Camera.m_piCamera);}
		if(m_bShowRotationGizmo)   {m_RotationGizmo.ProcessRotation(&mouseRay,m_Camera.m_piCamera);}
		if(m_bShowBBoxGizmo)       {m_BBoxGizmo.ProcessBBox(&mouseRay,m_Camera.m_piCamera);}
		
		if(m_PositionWrapper.m_piDesign && m_pEntity)
		{
			m_PositionWrapper.m_piDesign->SetPosition(m_TranslationGizmo.GetPosition()-m_pEntity->GetPhysicInfo()->vPosition);
		}
		if(m_OrientationWrapper.m_piDesign && m_pEntity)
		{
			m_OrientationWrapper.m_piDesign->SetAngles(m_RotationGizmo.GetAngles());
		}
		if(m_piLSChildren && m_EntityType.m_piEntityTypeDesign)
		{
			int nSelectedEntity=m_piLSChildren->GetSelectedElement();
			if(nSelectedEntity!=-1)
			{
				m_EntityType.m_piEntityTypeDesign->SetChildLocation(nSelectedEntity,m_TranslationGizmo.GetPosition()-m_pEntity->GetPhysicInfo()->vPosition,m_RotationGizmo.GetAngles());
			}
		}
		
		
		if(m_piLSBBoxes && m_piLSBBoxes->GetSelectedElement()!=-1)
		{
			SBBox sBBox;
			m_BBoxGizmo.GetBounds(&sBBox.vMins,&sBBox.vMaxs);
			
			std::vector<SBBox> vBBoxes;
			GetBBoxGroup(&vBBoxes);
			vBBoxes[m_piLSBBoxes->GetSelectedElement()]=sBBox;
			SetBBoxGroup(&vBBoxes);			
		}
		UpdateInteractiveElementsSpeedsAndSizes();
	}
	else
	{
		int nCurrentId=100;
		m_Render.m_piRender->StartSelection(m_rRealRect,m_Camera.m_piCamera,x,y,5);
		nCurrentId=m_TranslationGizmo.SelectionRender(nCurrentId,m_Render.m_piRender,m_Camera.m_piCamera);
		nCurrentId=m_RotationGizmo.SelectionRender(nCurrentId,m_Render.m_piRender,m_Camera.m_piCamera);
		nCurrentId=m_BBoxGizmo.SelectionRender(nCurrentId,m_Render.m_piRender,m_Camera.m_piCamera);
		int nSelectionId=m_Render.m_piRender->EndSelection();
		m_TranslationGizmo.Select(nSelectionId);
		m_RotationGizmo.Select(nSelectionId);
		m_BBoxGizmo.Select(nSelectionId);
	}
}

void CEntityEditorMainWindow::OnMouseUp( int nButton,double x,double y )
{
	if(m_piGUIManager->HasMouseCapture(this))
	{
		m_piGUIManager->ReleaseMouseCapture();
		m_TranslationGizmo.EndTranslation();
		m_RotationGizmo.EndRotation();	
		m_BBoxGizmo.EndBBox();	
		//m_pEntity->GetPhysicInfo()->dMaxVelocity=10;
	}
	m_bMovingGizmo=false;
	UpdateInteractiveElementsSpeedsAndSizes();
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
	UpdateChildrenList();
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

void CEntityEditorMainWindow::UpdateChildrenList()
{
	if(m_piLSChildren==NULL){return;}
	m_piLSChildren->Clear();
	
	if(m_EntityType.m_piEntityTypeDesign)
	{
		for(unsigned int x=0;x<m_EntityType.m_piEntityTypeDesign->GetChildren();x++)
		{
			IEntityType *piType=NULL;
			CEntityTypeWrapper wrapper;
			m_EntityType.m_piEntityTypeDesign->GetChild(x,&piType);
			wrapper.Attach(piType);
			if(wrapper.m_piObject)
			{
				m_piLSChildren->AddElement(wrapper.m_piObject->GetName());
			}
			REL(piType);
		}
	}
}


void CEntityEditorMainWindow::UpdateBBoxList()
{
	if(m_piLSBBoxes==NULL){return;}
	m_piLSBBoxes->Clear();	
	
	std::vector<SBBox> vBBoxes;
	GetBBoxGroup(&vBBoxes);
	
	for(unsigned int b=0;b<vBBoxes.size();b++)
	{
		char sName[100];
		sprintf(sName,"%d",b+1);
		m_piLSBBoxes->AddElement(sName);
	}
	
	if(m_piLSBBoxes->GetElementCount()){m_piLSBBoxes->SetSelectedElement(0);}
	UpdateSelectedBBox();
}

void CEntityEditorMainWindow::UpdateSelectedBBox()
{
	SBBox sBBox;
	
	if(m_piLSBBoxes==NULL){return;}
	if(m_EntityType.m_piEntityTypeDesign==NULL){return;}
	if(m_piLSBBoxes->GetSelectedElement()!=-1)
	{
		std::vector<SBBox> vBBoxes;
		GetBBoxGroup(&vBBoxes);
		sBBox=vBBoxes[m_piLSBBoxes->GetSelectedElement()];
	}
	m_BBoxGizmo.SetBounds(sBBox.vMins,sBBox.vMaxs);
}

void CEntityEditorMainWindow::ShowPropertiesOf(ISystemObject *piObject)
{
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
	UpdateGizmos();
}

void CEntityEditorMainWindow::UpdateGizmos()
{
	m_bShowTranslationGizmo=false;
	m_bShowRotationGizmo=false;

	if(m_PositionWrapper.m_piDesign){m_bShowTranslationGizmo=true;m_TranslationGizmo.SetPosition(m_PositionWrapper.m_piDesign->GetPosition());}
	if(m_OrientationWrapper.m_piDesign){m_bShowRotationGizmo=true;m_RotationGizmo.SetAngles(m_OrientationWrapper.m_piDesign->GetAngles());}

	if(!m_bShowTranslationGizmo && !m_bShowRotationGizmo && m_piLSChildren && m_EntityType.m_piEntityTypeDesign)
	{
		int nSelectedEntity=m_piLSChildren->GetSelectedElement();
		if(nSelectedEntity!=-1)
		{
			m_bShowRotationGizmo=true;
			m_bShowTranslationGizmo=true;
			
			CVector vPosition,vAngles;
			m_EntityType.m_piEntityTypeDesign->GetChildLocation(nSelectedEntity,vPosition,vAngles);

			m_TranslationGizmo.SetPosition(vPosition);
			m_RotationGizmo.SetAngles(vAngles);
		}
	}

	m_bShowBBoxGizmo=(m_eMode==eEntityEditorMode_EntityProperties);
}

void CEntityEditorMainWindow::UpdateSelectedObject()
{
	if(m_piLSAnimations==NULL){return;}
	if(m_piLSObjects==NULL){return;}

	m_PositionWrapper.Detach();
	m_OrientationWrapper.Detach();

	ISystemObject *piObject=NULL;
	
	if(m_eMode==eEntityEditorMode_EntityProperties)
	{
		piObject=ADD(m_EntityType.m_piObject);
	}
	else
	{
		int nSelectedAnimation=(m_piLSAnimations->GetSelectedElement());
		int nSelectedObject=(m_piLSObjects->GetSelectedElement());
		
		IAnimationObjectType *piAnimationObject=NULL;
		
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
	}
		
	ShowPropertiesOf(piObject);
	
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
	if(m_piAnimation)
	{
		m_nAnimationActivationTime=m_FrameManager.m_piFrameManager->GetCurrentRealTime();
		m_piAnimation->Activate(m_nAnimationActivationTime);		
	}
	
	UpdateInteractiveElementsSpeedsAndSizes();
}

void CEntityEditorMainWindow::UpdateInteractiveElementsSpeedsAndSizes()
{
	if(m_EntityType.m_piEntityType)
	{
		CVector vMins,vMaxs;
		m_EntityType.m_piEntityType->DesignGetAABBox(Origin,Origin,&vMins,&vMaxs);
		
		CVector vSize=vMaxs-vMins;
		double dMaxDimension=std::max(std::max(vSize.c[0],vSize.c[1]),vSize.c[2]);
		m_dCameraSpeed=dMaxDimension*0.5;

		if(m_pEntity && m_EntityType.m_piEntityTypeDesign)
		{
			SEntityTypeConfig sConfig;
			m_EntityType.m_piEntityTypeDesign->GetEntityTypeConfig(&sConfig);
			m_pEntity->GetPhysicInfo()->dMaxVelocity=m_bMovingGizmo?0:sConfig.dMaxVelocity;
		}
	}
	else
	{
		m_BBoxGizmo.SetArrowSize(5);
		m_TranslationGizmo.SetSize(5);
		m_RotationGizmo.SetRadius(3.725);
		m_dCameraSpeed=100.0;
	}
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
		// Deselect entity
		if(nElement!=-1 &&m_piLSChildren){m_piLSChildren->SetSelectedElement(-1);}
		UpdateSelectedObject();
	}
	if(piControl==m_piLSChildren)
	{
		// Deselect animation object
		if(nElement!=-1 && m_piLSObjects){m_piLSObjects->SetSelectedElement(-1);UpdateSelectedObject();}
	}
	if(piControl==m_piLSBBoxes)
	{
		UpdateSelectedBBox();
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
	m_vAnimations[nSelected].m_piAnimationType->DesignGetAABBox(Origin,Origin,&vMins,&vMaxs);
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
	
	if(OpenFileDialog("Select Model...",".ase;.ASE;.obj",&sBaseModel))
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
	
	if(OpenFileDialog("Select Sound...",".wav;.WAV;.ogg;.OGG",&sBaseSound))
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
					if(model.m_piSound->Load(sBaseSound,"SoundFX"))
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

void CEntityEditorMainWindow::ProcessNewChild()
{
	if(m_piLSChildren==NULL){return;}
	if(m_piLSObjects==NULL){return;}
	if(m_EntityType.m_piEntityType==NULL)
	{
		MessageDialog("No entity being edited.","Entity Editor",eMessageDialogType_Error);
		return;
	}

	unsigned long nSelectedEntityType=0;
	std::vector<IDesignObject *> vEntityTypes;
	GetSystemObjects("EntityTypes",&vEntityTypes);

	if(m_ObjectSelector.m_piObjectListSelector->SelectObject("New Child...",this,&vEntityTypes,&nSelectedEntityType))
	{
		ISystemObject *piObject=QI(ISystemObject,vEntityTypes[nSelectedEntityType]);
		if(piObject)
		{
			if(piObject->GetName()==m_sEntityName)
			{
				MessageDialog("Entity recursion is not allowed","Entity Editor",eMessageDialogType_Error);
			}
			else
			{
				unsigned int nElement=m_EntityType.m_piEntityTypeDesign->AddChild(piObject->GetName());
				UpdateChildrenList();
				m_piLSChildren->SetSelectedElement(nElement);
				if(m_piLSObjects){m_piLSObjects->SetSelectedElement(-1);UpdateSelectedObject();}
			}
		}
		REL(piObject);
	}
	for(unsigned long x=0;x<vEntityTypes.size();x++){IDesignObject *piEntityType=vEntityTypes[x];REL(piEntityType);}	
}

void CEntityEditorMainWindow::ProcessRemoveChild()
{
	if(m_EntityType.m_piEntityTypeDesign==NULL){return;}
	
	if(m_piLSChildren==NULL || m_piLSChildren->GetSelectedElement()==-1)
	{
		MessageDialog("No child selected","Entity Editor",eMessageDialogType_Error);
		return;
	}
	
	IEntityType   *piEntityType=NULL;
	ISystemObject *piObject=NULL;
	m_EntityType.m_piEntityTypeDesign->GetChild(m_piLSChildren->GetSelectedElement(),&piEntityType);
	piObject=QI(ISystemObject,piEntityType);
	if(piObject==NULL)
	{
		REL(piEntityType);
		REL(piObject);
		return;
	}

	std::string sText="Remove child '";
	sText+=piObject->GetName();
	sText+="' ?";
	REL(piEntityType);
	REL(piObject);
	
	if(!ConfirmDialog(sText,"New project",eMessageDialogType_Warning)){return;}
	
	m_EntityType.m_piEntityTypeDesign->RemoveChild(m_piLSChildren->GetSelectedElement());
	UpdateChildrenList();
}

void CEntityEditorMainWindow::GetBBoxGroup(	std::vector<SBBox> *pvBBoxes)
{
	pvBBoxes->clear();
	
	if(m_EntityType.m_piEntityType && m_eBBoxGroup==eEntityEditorBBoxGroup_Collisions)
	{
		SEntityTypeConfig sConfig;
		m_EntityType.m_piEntityTypeDesign->GetEntityTypeConfig(&sConfig);
		*pvBBoxes=sConfig.vBBoxes;
	}
	else if(m_piStaticStructureTypeDesign && m_eBBoxGroup==eEntityEditorBBoxGroup_Vulnerable)
	{
		m_piStaticStructureTypeDesign->GetVulnerableRegions(pvBBoxes);
	}
	else if(m_piStaticStructureTypeDesign && m_eBBoxGroup==eEntityEditorBBoxGroup_ProtectiveNormal)
	{
		m_piStaticStructureTypeDesign->GetProtectiveRegions(pvBBoxes);
	}
	else if(m_piStaticStructureTypeDesign && m_eBBoxGroup==eEntityEditorBBoxGroup_ProtectiveDestroyed)
	{
		m_piStaticStructureTypeDesign->GetProtectiveDestroyedRegions(pvBBoxes);
	}
}

void CEntityEditorMainWindow::SetBBoxGroup(	std::vector<SBBox> *pvBBoxes)
{
	if(m_EntityType.m_piEntityType && m_eBBoxGroup==eEntityEditorBBoxGroup_Collisions)
	{
		SEntityTypeConfig sConfig;
		m_EntityType.m_piEntityTypeDesign->GetEntityTypeConfig(&sConfig);
		sConfig.vBBoxes=*pvBBoxes;
		m_EntityType.m_piEntityTypeDesign->SetEntityTypeConfig(&sConfig);
	}
	else if(m_piStaticStructureTypeDesign && m_eBBoxGroup==eEntityEditorBBoxGroup_Vulnerable)
	{
		m_piStaticStructureTypeDesign->SetVulnerableRegions(pvBBoxes);
	}
	else if(m_piStaticStructureTypeDesign && m_eBBoxGroup==eEntityEditorBBoxGroup_ProtectiveNormal)
	{
		m_piStaticStructureTypeDesign->SetProtectiveRegions(pvBBoxes);
	}
	else if(m_piStaticStructureTypeDesign && m_eBBoxGroup==eEntityEditorBBoxGroup_ProtectiveDestroyed)
	{
		m_piStaticStructureTypeDesign->SetProtectiveDestroyedRegions(pvBBoxes);
	}
}

void CEntityEditorMainWindow::ProcessNewBBox()
{
	if(!m_EntityType.m_piEntityTypeDesign){return;}
	
	SBBox sBBox;
	std::vector<SBBox> vBBoxes;
	GetBBoxGroup(&vBBoxes);
	vBBoxes.push_back(sBBox);
	SetBBoxGroup(&vBBoxes);
	
	UpdateBBoxList();
	m_piLSBBoxes->SetSelectedElement(((int)vBBoxes.size())-1);
	UpdateSelectedBBox();
}

void CEntityEditorMainWindow::ProcessRemoveBBox()
{
	if(m_piLSBBoxes==NULL || m_piLSBBoxes->GetSelectedElement()==-1)
	{
		MessageDialog("No BBox selected.","Entity Editor",eMessageDialogType_Error);
		return;
	}
	int nSelected=m_piLSBBoxes->GetSelectedElement();

	vector<SBBox> vOldBBoxes,vBBoxes;
	GetBBoxGroup(&vOldBBoxes);
	for(int x=0;x<(int)vOldBBoxes.size();x++)
	{
		if(x!=nSelected){vBBoxes.push_back(vOldBBoxes[x]);}
	}
	SetBBoxGroup(&vBBoxes);
	
	
	UpdateBBoxList();
	m_piLSBBoxes->SetSelectedElement(nSelected>=(int)vBBoxes.size()?((int)vBBoxes.size())-1:nSelected);
	UpdateSelectedBBox();
}

void CEntityEditorMainWindow::ProcessNewEntity()
{
	if(m_NamedObjectSelector.m_piObjectSelector==NULL){return;}
	if(m_piLSAnimations==NULL){return;}
	int nSelected=(m_piLSAnimations->GetSelectedElement());
	if(nSelected==-1)
	{
		MessageDialog("No animation selected.","Entity Editor",eMessageDialogType_Error);
		return;
	}
	
	unsigned long nSelectedEntityType=0;
	std::vector<IDesignObject *> vEntityTypes;
	GetSystemObjects("EntityTypes",&vEntityTypes);
	
	if(m_ObjectSelector.m_piObjectListSelector->SelectObject("New Entity...",this,&vEntityTypes,&nSelectedEntityType))
	{
		CEntityTypeWrapper entityTypeWrapper;
		entityTypeWrapper.Attach(vEntityTypes[nSelectedEntityType]);
		
		if(entityTypeWrapper.m_piEntityType)
		{
			IAnimationObjectType *piAnimationObject=NULL;
			CAnimationEntityTypeWrapper animationObject;
			unsigned int nIndex=m_vAnimations[nSelected].m_piAnimationTypeDesign->AddObject("EntityAnimationObjectType");
			m_vAnimations[nSelected].m_piAnimationTypeDesign->GetObject(nIndex,&piAnimationObject);
			if(animationObject.Attach(piAnimationObject))
			{
				SEntityAnimationObjectTypeConfig config;
				animationObject.m_piDesign->SetConfig(&config);
				animationObject.m_piDesign->SetEntityType(entityTypeWrapper.m_piEntityType);
			}
			REL(piAnimationObject);
		}
			
		UpdateSelectedAnimation();
	}
	for(unsigned long x=0;x<vEntityTypes.size();x++){IDesignObject *piEntityType=vEntityTypes[x];REL(piEntityType);}	
}


void CEntityEditorMainWindow::OnObjectChanged(IEntityEditorPropertyPanel *piPanel,ISystemObject *piObject)
{
	if(m_piLSAnimations==NULL){return;}
	if(m_piLSObjects==NULL){return;}
	
	int nSelectedAnimation=(m_piLSAnimations->GetSelectedElement());
	
	if(nSelectedAnimation!=-1)
	{
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
