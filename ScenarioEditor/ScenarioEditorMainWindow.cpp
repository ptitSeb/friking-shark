#include "StdAfx.h"
#include "glprocs.h"
#include "resource.h"
#include "GameRuntimeLib.h"
#include "GameLib.h"
#include "GameGUILib.h"
#include "ScenarioEditorMainWindow.h"

#define SELECT_FORMATION_BASE_INDEX 0x800

#pragma warning ( disable : 4244 )

DECLARE_CUSTOM_WRAPPER1(CGameGUIManagerWrapper,IGameGUIManager,m_piInterface);

extern CSystemModuleHelper *g_pSystemModuleHelper;

CScenarioEditorMainWindow::CScenarioEditorMainWindow(void)
{
	m_bMovingObject=false;
	m_bAutoGenerateBSP=true;
	m_bAutoUpdateBSP=false;
	
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
	m_dwNexControlKey=0;
	m_bTextures=1;
	m_bFog=1;
	m_bShaders=1;
	m_bColors=1;
	m_bShadows=1;
	m_bSolid=1;
	m_bLighting=1;
	m_bBlend=1;
	m_nSelectedHeightLayer=-1;
	m_nSelectedColorLayer=-1;
	m_nSelectedEntityLayer=-1;
	m_nSelectedEntity=-1;
	m_nSelectedFormation=-1;
	m_piSTFps=NULL;
	m_piSTVertexOverhead=NULL;
	m_piSTFaceOverhead=NULL;

	m_piGROptionsPanel=NULL;
	m_piGRTerrainPanel=NULL;
	m_piGREntityPanel=NULL;
	m_piGREntitiesPanel=NULL;
	m_piGRFormationPanel=NULL;
	m_piGRFormationsPanel=NULL;
	m_piBTShowOptionsPanel=NULL;
	m_piBTShowTerrainPanel=NULL;
	m_piBTShowEntitiesPanel=NULL;
	m_piBTShowFormationsPanel=NULL;
	m_piBTShowFilePanel=NULL;
	m_piBTShowGeneralProperties=NULL;
	m_piBTShowWaterProperties=NULL;
	m_piBTShowFogProperties=NULL;
	m_piBTShowSunProperties=NULL;
	m_piBTShowSkyProperties=NULL;
	m_piBTShowPlayAreaProperties=NULL;

	m_piGREntityLayerList=NULL;
	m_piBTNewEntityLayer=NULL;
	m_piBTNewEntity=NULL;

	m_piBTNewFormation=NULL;

	m_piGRLayerList=NULL;
	m_piGRHeightLayerPanel=NULL;
	m_piGRColorLayerPanel=NULL;
	m_piGREntityLayerPanel=NULL;
	m_piGRWaterPanel=NULL;
	m_piGRFogPanel=NULL;
	m_piGRSunPanel=NULL;
	m_piGRSkyPanel=NULL;
	m_piGRPlayAreaPanel=NULL;
	m_piBTNewHeightLayer=NULL;
	m_piBTNewColorLayer=NULL;

	m_piSTHeightLayerName=NULL;
	m_piBTHeightLayerSample=NULL;
	m_piSTHeightLayerMaxHeight=NULL;
	m_piBTHeightLayerDecreaseMaxHeight=NULL;
	m_piBTHeightLayerIncreaseMaxHeight=NULL;
	m_piSTHeightLayerMinHeight=NULL;
	m_piBTHeightLayerDecreaseMinHeight=NULL;
	m_piBTHeightLayerIncreaseMinHeight=NULL;
	m_piSTHeightLayerMargin=NULL;
	m_piBTHeightLayerDecreaseMargin=NULL;
	m_piBTHeightLayerIncreaseMargin=NULL;
	m_piSTHeightLayerHorzResolution=NULL;
	m_piBTHeightLayerDecreaseHorzResolution=NULL;
	m_piBTHeightLayerIncreaseHorzResolution=NULL;
	m_piSTHeightLayerVertResolution=NULL;
	m_piBTHeightLayerDecreaseVertResolution=NULL;
	m_piBTHeightLayerIncreaseVertResolution=NULL;
	m_piBTHeightLayerRemove=NULL;
	m_piBTHeightLayerMoveUp=NULL;
	m_piBTHeightLayerMoveDown=NULL;

	m_piSTColorLayerName=NULL;
	m_piBTColorLayerSample=NULL;
	m_piBTColorLayerColorSample=NULL;
	m_piSTColorLayerHorzResolution=NULL;
	m_piBTColorLayerDecreaseHorzResolution=NULL;
	m_piBTColorLayerIncreaseHorzResolution=NULL;
	m_piSTColorLayerVertResolution=NULL;
	m_piBTColorLayerDecreaseVertResolution=NULL;
	m_piBTColorLayerIncreaseVertResolution=NULL;
	m_piBTColorLayerRemove=NULL;
	m_piBTColorLayerMoveUp=NULL;
	m_piBTColorLayerMoveDown=NULL;

	m_piBTEntityLayerRemove=NULL;
	m_piSTEntityLayerName=NULL;
	m_piBTEntityLayerSample=NULL;
	m_piSTEntityLayerObjectLabel=NULL;
	m_piBTEntityLayerColorSample=NULL;
	m_piSTEntityLayerSeparation=NULL;
	m_piBTEntityLayerDecreaseSeparation=NULL;
	m_piBTEntityLayerIncreaseSeparation=NULL;
	m_piSTEntityLayerPositionJitter=NULL;
	m_piBTEntityLayerDecreasePositionJitter=NULL;
	m_piBTEntityLayerIncreasePositionJitter=NULL;
	m_piSTEntityLayerYawJitter=NULL;
	m_piBTEntityLayerDecreaseYawJitter=NULL;
	m_piBTEntityLayerIncreaseYawJitter=NULL;


	m_piBTEntityRemove=NULL;
	m_piSTEntityName=NULL;
	m_piBTEntitySample=NULL;
	m_piSTEntityObjectLabel=NULL;
	m_piSTEntityYaw=NULL;
	m_piBTEntityDecreaseYaw=NULL;
	m_piBTEntityIncreaseYaw=NULL;
	m_piBTEntityConsiderAsTerrain=NULL;
	m_piBTEntityLocked=NULL;
	
	m_piBTFormationRemove=NULL;
	m_piSTFormationName=NULL;
	m_piBTFormationSample=NULL;
	m_piSTFormationObjectLabel=NULL;
	m_piBTFormationBonusSample=NULL;
	m_piSTFormationBonusObjectLabel=NULL;

	m_piBTOptionsTextures=NULL;
	m_piBTOptionsSolid=NULL;
	m_piBTOptionsBlend=NULL;
	m_piBTOptionsLayerOnly=NULL;
	m_piBTOptionsModelOriginal=NULL;
	m_piBTOptionsModelGenerated=NULL;

	m_piGRFile=NULL;
	m_piBTFileNew=NULL;
	m_piBTFileOpen=NULL;
	m_piBTFileSave=NULL;
	m_piBTFileSaveAs=NULL;
	m_piBTFileCompile=NULL;
	m_piBTFileExit=NULL;

	m_piBTGeneralChangeModel=NULL;
	m_piBTGeneralChangeColorMap=NULL;
	m_piSTGeneralModelName=NULL;
	m_piSTGeneralColorMapName=NULL;

	m_piSTWaterName=NULL;
	m_piBTWaterSample=NULL;
	m_piSTWaterSecondName=NULL;
	m_piBTWaterSecondSample=NULL;
	m_piSTWaterHeight=NULL;
	m_piBTWaterDecreaseHeight=NULL;
	m_piBTWaterIncreaseHeight=NULL;
	m_piSTWaterOpacity=NULL;
	m_piBTWaterDecreaseOpacity=NULL;
	m_piBTWaterIncreaseOpacity=NULL;
	m_piSTWaterSpeed=NULL;
	m_piBTWaterDecreaseSpeed=NULL;
	m_piBTWaterIncreaseSpeed=NULL;
	m_piSTWaterHorzResolution=NULL;
	m_piBTWaterDecreaseHorzResolution=NULL;
	m_piBTWaterIncreaseHorzResolution=NULL;
	m_piSTWaterVertResolution=NULL;
	m_piBTWaterDecreaseVertResolution=NULL;
	m_piBTWaterIncreaseVertResolution=NULL;
	m_piBTWaterEnable=NULL;

	m_piSTFogStart=NULL;
	m_piBTFogDecreaseStart=NULL;
	m_piBTFogIncreaseStart=NULL;
	m_piSTFogEnd=NULL;
	m_piBTFogDecreaseEnd=NULL;
	m_piBTFogIncreaseEnd=NULL;
	m_piBTFogColorSample=NULL;
	m_piBTFogEnable=NULL;
	m_piBTFogFitWater=NULL;
	m_piBTFogFitSky=NULL;

	m_piSTSunDistance=NULL;
	m_piBTSunDecreaseDistance=NULL;
	m_piBTSunIncreaseDistance=NULL;
	m_piSTSunElevation=NULL;
	m_piBTSunDecreaseElevation=NULL;
	m_piBTSunIncreaseElevation=NULL;
	m_piSTSunAzimuth=NULL;
	m_piBTSunDecreaseAzimuth=NULL;
	m_piBTSunIncreaseAzimuth=NULL;
	m_piBTSunColorSample=NULL;
	m_piBTAmbientColorSample=NULL;

	m_piSTSkyName=NULL;
	m_piBTSkySample=NULL;
	m_piSTSkyOpacity=NULL;
	m_piBTSkyDecreaseOpacity=NULL;
	m_piBTSkyIncreaseOpacity=NULL;
	m_piSTSkySpeed=NULL;
	m_piBTSkyDecreaseSpeed=NULL;
	m_piBTSkyIncreaseSpeed=NULL;
	m_piSTSkyHorzResolution=NULL;
	m_piBTSkyDecreaseHorzResolution=NULL;
	m_piBTSkyIncreaseHorzResolution=NULL;
	m_piSTSkyVertResolution=NULL;
	m_piBTSkyDecreaseVertResolution=NULL;
	m_piBTSkyIncreaseVertResolution=NULL;
	m_piBTSkyEnable=NULL;

	m_piSTPlayAreaCameraDistance=NULL;
	m_piBTPlayAreaDecreaseCameraDistance=NULL;
	m_piBTPlayAreaIncreaseCameraDistance=NULL;
	m_piSTPlayAreaCameraSpeed=NULL;
	m_piBTPlayAreaDecreaseCameraSpeed=NULL;
	m_piBTPlayAreaIncreaseCameraSpeed=NULL;
	m_piSTPlayAreaCameraViewAngle=NULL;
	m_piBTPlayAreaDecreaseCameraViewAngle=NULL;
	m_piBTPlayAreaIncreaseCameraViewAngle=NULL;
	m_piSTPlayAreaCameraAspectRatio=NULL;
	m_piBTPlayAreaDecreaseCameraAspectRatio=NULL;
	m_piBTPlayAreaIncreaseCameraAspectRatio=NULL;
	m_piSTPlayAreaAirPlane=NULL;
	m_piBTPlayAreaDecreaseAirPlane=NULL;
	m_piBTPlayAreaIncreaseAirPlane=NULL;
	m_piSTPlayAreaScroll=NULL;
	m_piBTPlayAreaDecreaseScroll=NULL;
	m_piBTPlayAreaIncreaseScroll=NULL;

	m_piGameSystem=NULL;
	m_bSimulationStarted=false;
}

CScenarioEditorMainWindow::~CScenarioEditorMainWindow(void)
{
}

bool CScenarioEditorMainWindow::Unserialize(ISystemPersistencyNode *piNode)
{
	bool bOk=CGameWindowBase::Unserialize(piNode);
	if(bOk)
	{
		CGameGUIManagerWrapper guiManager;
		guiManager.Attach("GameGUI","GUIManager");
		IGameWindow *piMainWindow=guiManager.m_piInterface->GetMainWindow();
		InitWindow(piMainWindow,false);
		Show(true);
		REL(piMainWindow);

		m_ColorSelector.Attach("GameGUI","ColorSelector");
		m_ObjectSelector.Attach("GameGUI","ObjectSelector");
	}
	if(bOk)
	{
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
	m_piSTFormationObjectLabel->Activate(false);
	m_piSTFormationBonusObjectLabel->Activate(false);


	SUBSCRIBE_TO_CAST(m_piBTShowOptionsPanel,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTShowEntitiesPanel,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTShowFormationsPanel,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTShowTerrainPanel,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTShowFilePanel,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTShowGeneralProperties,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTShowWaterProperties,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTShowFogProperties,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTShowSunProperties,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTShowSkyProperties,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTShowPlayAreaProperties,IGameGUIButtonEvents);

	SUBSCRIBE_TO_CAST(m_piBTNewEntityLayer,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTNewEntity,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTNewFormation,IGameGUIButtonEvents);

	SUBSCRIBE_TO_CAST(m_piBTNewColorLayer,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTNewHeightLayer,IGameGUIButtonEvents);

	SUBSCRIBE_TO_CAST(m_piBTHeightLayerSample,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTHeightLayerDecreaseMinHeight,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTHeightLayerIncreaseMinHeight,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTHeightLayerDecreaseMaxHeight,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTHeightLayerIncreaseMaxHeight,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTHeightLayerDecreaseMargin,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTHeightLayerIncreaseMargin,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTHeightLayerDecreaseHorzResolution,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTHeightLayerIncreaseHorzResolution,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTHeightLayerDecreaseVertResolution,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTHeightLayerIncreaseVertResolution,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTHeightLayerRemove,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTHeightLayerMoveUp,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTHeightLayerMoveDown,IGameGUIButtonEvents);

	SUBSCRIBE_TO_CAST(m_piBTColorLayerSample,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTColorLayerColorSample,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTColorLayerDecreaseHorzResolution,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTColorLayerIncreaseHorzResolution,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTColorLayerDecreaseVertResolution,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTColorLayerIncreaseVertResolution,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTColorLayerRemove,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTColorLayerMoveUp,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTColorLayerMoveDown,IGameGUIButtonEvents);

	SUBSCRIBE_TO_CAST(m_piBTEntityLayerRemove,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTEntityLayerSample,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTEntityLayerColorSample,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTEntityLayerDecreasePositionJitter,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTEntityLayerIncreasePositionJitter,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTEntityLayerDecreaseSeparation,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTEntityLayerIncreaseSeparation,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTEntityLayerDecreaseYawJitter,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTEntityLayerIncreaseYawJitter,IGameGUIButtonEvents);

	SUBSCRIBE_TO_CAST(m_piBTEntityRemove,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTEntitySample,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTEntityDecreaseYaw,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTEntityIncreaseYaw,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTEntityConsiderAsTerrain,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTEntityLocked,IGameGUIButtonEvents);

	SUBSCRIBE_TO_CAST(m_piBTFormationRemove,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTFormationSample,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTFormationBonusSample,IGameGUIButtonEvents);

	SUBSCRIBE_TO_CAST(m_piBTOptionsTextures,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTOptionsSolid,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTOptionsBlend,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTOptionsLayerOnly,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTOptionsModelOriginal,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTOptionsModelGenerated,IGameGUIButtonEvents);


	SUBSCRIBE_TO_CAST(m_piBTFileNew,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTFileOpen,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTFileSave,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTFileSaveAs,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTFileCompile,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTFileExit,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTGeneralChangeModel,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTGeneralChangeColorMap,IGameGUIButtonEvents);
	
	SUBSCRIBE_TO_CAST(m_piBTWaterSample,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTWaterSecondSample,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTWaterDecreaseOpacity,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTWaterIncreaseOpacity,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTWaterDecreaseHeight,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTWaterIncreaseHeight,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTWaterDecreaseSpeed,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTWaterIncreaseSpeed,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTWaterDecreaseHorzResolution,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTWaterIncreaseHorzResolution,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTWaterDecreaseVertResolution,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTWaterIncreaseVertResolution,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTWaterEnable,IGameGUIButtonEvents);

	SUBSCRIBE_TO_CAST(m_piBTFogDecreaseStart,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTFogIncreaseStart,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTFogDecreaseEnd,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTFogIncreaseEnd,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTFogEnable,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTFogFitWater,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTFogFitSky,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTFogColorSample,IGameGUIButtonEvents);

	SUBSCRIBE_TO_CAST(m_piBTSunDecreaseDistance,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTSunIncreaseDistance,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTSunDecreaseElevation,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTSunIncreaseElevation,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTSunDecreaseAzimuth,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTSunIncreaseAzimuth,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTSunColorSample,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTAmbientColorSample,IGameGUIButtonEvents);

	SUBSCRIBE_TO_CAST(m_piBTSkySample,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTSkyDecreaseOpacity,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTSkyIncreaseOpacity,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTSkyDecreaseSpeed,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTSkyIncreaseSpeed,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTSkyDecreaseHorzResolution,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTSkyIncreaseHorzResolution,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTSkyDecreaseVertResolution,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTSkyIncreaseVertResolution,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTSkyEnable,IGameGUIButtonEvents);

	SUBSCRIBE_TO_CAST(m_piBTPlayAreaDecreaseCameraDistance,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTPlayAreaIncreaseCameraDistance,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTPlayAreaDecreaseCameraSpeed,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTPlayAreaIncreaseCameraSpeed,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTPlayAreaDecreaseCameraViewAngle,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTPlayAreaIncreaseCameraViewAngle,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTPlayAreaDecreaseCameraAspectRatio,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTPlayAreaIncreaseCameraAspectRatio,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTPlayAreaDecreaseAirPlane,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTPlayAreaIncreaseAirPlane,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTPlayAreaDecreaseScroll,IGameGUIButtonEvents);
	SUBSCRIBE_TO_CAST(m_piBTPlayAreaIncreaseScroll,IGameGUIButtonEvents);

	UpdateLayerPanel();

	CSystemLoaderHelper     systemLoader;
	m_piGameSystem=systemLoader.LoadSystem("Scripts\\GameSystem.cfg","GameSystem");
	m_GameControllerWrapper.Attach("GameSystem","GameController");
	m_GameControllerWrapper.m_piGameController->SetupGame();
	m_GameControllerWrapper.m_piGameController->CreateScenario();

	m_GameRenderWrapper.Attach("GameSystem","GameRender");
	m_PlayAreaManagerWrapper.Attach("GameSystem","PlayAreaManager");
	m_FrameManager.Attach("GameSystem","FrameManager");
	m_WorldManagerWrapper.Attach("GameSystem","WorldManager");
	
//	OpenScenario("C:\\TerrainGenerator\\Resources\\IA.ges");
	return bOk;
}

void CScenarioEditorMainWindow::DestroyWindow()
{
	StopGameSimulation();
	Reset();
	m_GameControllerWrapper.m_piGameController->EndGame();
	m_PlayAreaManagerWrapper.Detach();
	m_GameControllerWrapper.Detach();
	m_GameRenderWrapper.Detach();
	m_WorldManagerWrapper.Detach();

	m_FrameManager.Detach();
	if(m_piGameSystem){m_piGameSystem->Destroy();}
	REL(m_piGameSystem);

	m_ColorSelector.Detach();
	m_ObjectSelector.Detach();
	m_Viewport.Detach();
	m_Viewport.Detach();

	UNSUBSCRIBE_FROM_ALL();
	CGameWindowBase::DestroyWindow();
}

void CScenarioEditorMainWindow::Reset()
{
	StopGameSimulation();
	if(m_GameControllerWrapper.m_piGameController){m_GameControllerWrapper.m_piGameController->CloseScenario();}
	if(m_piSTEntityLayerObjectLabel){m_piSTEntityLayerObjectLabel->SetObject(NULL);}
	if(m_piSTEntityObjectLabel){m_piSTEntityObjectLabel->SetObject(NULL);}
	if(m_piSTFormationObjectLabel){m_piSTFormationObjectLabel->SetObject(NULL);}
	if(m_piSTFormationBonusObjectLabel){m_piSTFormationBonusObjectLabel->SetObject(NULL);}
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

	UpdateCaption();
}

void CScenarioEditorMainWindow::ProcessInput(double dTimeFraction,double dRealTimeFraction)
{
	if(GetTickCount()>m_dwNexControlKey)
	{
		if(m_piGUIManager->IsKeyDown(VK_F1) && m_bSimulationStarted)
		{
			m_bInspectionMode=!m_bInspectionMode;
			m_dwNexControlKey=GetTickCount()+500;
		}
		if(m_piGUIManager->IsKeyDown(VK_F5))
		{
			if(m_piGUIManager->IsKeyDown(VK_LSHIFT))
			{
				StopGameSimulation();
			}
			else
			{
				StartGameSimulation();
			}
			m_dwNexControlKey=GetTickCount()+500;
		}
		if(m_piGUIManager->IsKeyDown(VK_F3)){ProcessFileOpen();m_dwNexControlKey=GetTickCount()+500;}
		if(m_piGUIManager->IsKeyDown(VK_F3)){ProcessFileOpen();m_dwNexControlKey=GetTickCount()+500;}
		if(m_piGUIManager->IsKeyDown(VK_F2)){ProcessFileSave();m_dwNexControlKey=GetTickCount()+500;}
		if(m_piGUIManager->IsKeyDown(VK_PAUSE)){m_FrameManager.m_piFrameManager->TogglePauseOnNextFrame();m_dwNexControlKey=GetTickCount()+100;}
		if(m_piGUIManager->IsKeyDown('T'))
		{
			m_bTextures=!m_bTextures;
			m_dwNexControlKey=GetTickCount()+500;
		}
		if(m_piGUIManager->IsKeyDown('G'))
		{
			m_bFog=!m_bFog;
			m_dwNexControlKey=GetTickCount()+500;
		}
		if(m_piGUIManager->IsKeyDown(VK_F10))
		{
			m_bShaders=!m_bShaders;
			m_dwNexControlKey=GetTickCount()+500;
		}
		if(m_piGUIManager->IsKeyDown('C'))
		{
			m_bColors=!m_bColors;
			m_dwNexControlKey=GetTickCount()+500;
		}
		if(m_piGUIManager->IsKeyDown('L'))
		{
			m_bSolid=!m_bSolid;
			m_dwNexControlKey=GetTickCount()+500;
		}
		if(m_piGUIManager->IsKeyDown('I'))
		{
			m_bLighting=!m_bLighting;
			m_dwNexControlKey=GetTickCount()+500;
		}
		if(m_piGUIManager->IsKeyDown('H'))
		{
			m_bShadows=!m_bShadows;
			m_dwNexControlKey=GetTickCount()+500;
		}
		if(m_piGUIManager->IsKeyDown('B')){m_bBlend=!m_bBlend;m_dwNexControlKey=GetTickCount()+500;}
		if(m_piGUIManager->IsKeyDown(VK_HOME)){CenterCamera();m_dwNexControlKey=GetTickCount()+500;}
	}
	if(!m_bSimulationStarted || m_bInspectionMode)
	{
		if(m_piGUIManager->IsKeyDown(VK_UP) || m_piGUIManager->IsKeyDown(VK_NUMPAD8) || m_piGUIManager->IsKeyDown('W')){ProcessKey(KEY_FORWARD,dTimeFraction,dRealTimeFraction);}
		if(m_piGUIManager->IsKeyDown(VK_DOWN) || m_piGUIManager->IsKeyDown(VK_NUMPAD2) || m_piGUIManager->IsKeyDown('S')){ProcessKey(KEY_BACK,dTimeFraction,dRealTimeFraction);}
		if(m_piGUIManager->IsKeyDown(VK_LEFT) || m_piGUIManager->IsKeyDown(VK_NUMPAD4) || m_piGUIManager->IsKeyDown('A')){ProcessKey(KEY_LEFT,dTimeFraction,dRealTimeFraction);}
		if(m_piGUIManager->IsKeyDown(VK_RIGHT) || m_piGUIManager->IsKeyDown(VK_NUMPAD6) || m_piGUIManager->IsKeyDown('D')){ProcessKey(KEY_RIGHT,dTimeFraction,dRealTimeFraction);}
		if(m_piGUIManager->IsKeyDown(VK_NUMPAD9) || m_piGUIManager->IsKeyDown('R')){ProcessKey(KEY_UP,dTimeFraction,dRealTimeFraction);}
		if(m_piGUIManager->IsKeyDown(VK_NUMPAD3) || m_piGUIManager->IsKeyDown('F')){ProcessKey(KEY_DOWN,dTimeFraction,dRealTimeFraction);}
	}
}

void CScenarioEditorMainWindow::ProcessKey(SHORT nKey,double dTimeFraction,double dRealTimeFraction)
{
	double dMovementInspectionSpeed=500.0;
	double dForwardSpeed=dMovementInspectionSpeed*dTimeFraction;
	double dCameraForwardSpeed=dMovementInspectionSpeed*dRealTimeFraction;
	bool   bMoveEntity=false;
	if(GetKeyState(VK_LSHIFT)&0x8000){dForwardSpeed*=3.0;}
	if(GetKeyState(VK_LCONTROL)&0x8000)
	{
		CVector vChange;
		if(nKey==KEY_FORWARD)	{vChange.c[PITCH]-=dCameraForwardSpeed*0.3;}
		else if(nKey==KEY_BACK)	{vChange.c[PITCH]+=dCameraForwardSpeed*0.3;}
		else if(nKey==KEY_LEFT)	{vChange.c[YAW]+=dCameraForwardSpeed*0.3;}
		else if(nKey==KEY_RIGHT)	{vChange.c[YAW]-=dCameraForwardSpeed*0.3;}
		m_Camera.m_piCamera->SetAngles(m_Camera.m_piCamera->GetAngles()+vChange);
	}
	else
	{
		if(nKey==KEY_UP)	{m_Camera.m_piCamera->SetPosition(m_Camera.m_piCamera->GetPosition()+m_Camera.m_piCamera->GetUpVector()*(dCameraForwardSpeed));}
		else if(nKey==KEY_DOWN)	{m_Camera.m_piCamera->SetPosition(m_Camera.m_piCamera->GetPosition()-m_Camera.m_piCamera->GetUpVector()*(dCameraForwardSpeed));}
		else if(nKey==KEY_LEFT)	{m_Camera.m_piCamera->SetPosition(m_Camera.m_piCamera->GetPosition()-m_Camera.m_piCamera->GetRightVector()*(dCameraForwardSpeed));}
		else if(nKey==KEY_RIGHT){m_Camera.m_piCamera->SetPosition(m_Camera.m_piCamera->GetPosition()+m_Camera.m_piCamera->GetRightVector()*(dCameraForwardSpeed));}
		else if(nKey==KEY_FORWARD)	{m_Camera.m_piCamera->SetPosition(m_Camera.m_piCamera->GetPosition()+m_Camera.m_piCamera->GetForwardVector()*(dCameraForwardSpeed));}
		else if(nKey==KEY_BACK)	{m_Camera.m_piCamera->SetPosition(m_Camera.m_piCamera->GetPosition()-m_Camera.m_piCamera->GetForwardVector()*(dCameraForwardSpeed));}
	}

}
void CScenarioEditorMainWindow::SetupRenderOptions(IGenericRender *piRender)
{
	double dNearPlane=0,dFarPlane=0;
	CVector vAngles,vPosition;
	vAngles=m_Camera.m_piCamera->GetAngles();
	vPosition=m_Camera.m_piCamera->GetPosition();
	m_Camera.m_piCamera->GetClippingPlanes(dNearPlane,dFarPlane);
	double dViewAngle=m_Camera.m_piCamera->GetViewAngle();

	piRender->SetPerspectiveProjection(dViewAngle,dNearPlane,dFarPlane);
	piRender->SetCamera(vPosition,vAngles.c[YAW],vAngles.c[PITCH],vAngles.c[ROLL]);
}
CVector vSunAngles;

void CScenarioEditorMainWindow::OnDraw(IGenericRender *piRender)
{
	if(!m_FrameManager.m_piFrameManager)
	{
		return;
	}

	m_FrameManager.m_piFrameManager->ProcessFrame();
	ProcessInput(m_FrameManager.m_piFrameManager->GetTimeFraction(),m_FrameManager.m_piFrameManager->GetRealTimeFraction());
	
	m_Render.m_piRender->ActivateDepth();
	m_Render.m_piRender->PushOptions();
	m_Render.m_piRender->PushState();

	m_bTextures?m_Render.m_piRender->EnableTextures():m_Render.m_piRender->DisableTextures();
	m_bSolid?m_Render.m_piRender->EnableSolid():m_Render.m_piRender->DisableSolid();
	m_bBlend?m_Render.m_piRender->EnableBlending():m_Render.m_piRender->DisableBlending();
	m_bLighting?m_Render.m_piRender->EnableLighting():m_Render.m_piRender->DisableLighting();
	m_bShadows?m_Render.m_piRender->EnableShadows():m_Render.m_piRender->DisableShadows();
	m_bShaders?m_Render.m_piRender->EnableShaders():m_Render.m_piRender->DisableShaders();
	m_bFog?m_Render.m_piRender->EnableHeightFog():m_Render.m_piRender->DisableHeightFog();

	char A[200];
	sprintf(A,"Shader: %s",m_Render.m_piRender->AreShadersEnabled()?"yes":"no");
	m_piSTVertexOverhead->SetText(A);

	if(m_bSimulationStarted)
	{

		m_PlayAreaManagerWrapper.m_piPlayAreaManager->ProcessInput(m_piGUIManager);
		m_GameControllerWrapper.m_piGameController->ProcessFrame(m_FrameManager.m_piFrameManager->GetCurrentTime(),m_FrameManager.m_piFrameManager->GetTimeFraction());
		IGenericCamera *piCamera=m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCamera();
		if(piCamera)
		{
			double dAspectRatio=m_rRealRect.h/m_rRealRect.w;
			double dNearPlane=0,dFarPlane=0;
			CVector vAngles,vPosition;
			double dViewAngle=0;
			double dPlayAreaAspectRatio=1;

			if(m_bInspectionMode)
			{
				dViewAngle=m_Camera.m_piCamera->GetViewAngle();
				dPlayAreaAspectRatio=m_Camera.m_piCamera->GetAspectRatio();
				m_Camera.m_piCamera->GetClippingPlanes(dNearPlane,dFarPlane);
				vAngles=m_Camera.m_piCamera->GetAngles();
				vPosition=m_Camera.m_piCamera->GetPosition();
			}
			else
			{
				dViewAngle=piCamera->GetViewAngle();
				dPlayAreaAspectRatio=piCamera->GetAspectRatio();
				piCamera->GetClippingPlanes(dNearPlane,dFarPlane);
				vAngles=piCamera->GetAngles();
				vPosition=piCamera->GetPosition();

				m_Camera.m_piCamera->SetAngles(piCamera->GetAngles());
				m_Camera.m_piCamera->SetPosition(piCamera->GetPosition());
				m_Camera.m_piCamera->SetViewAngle(piCamera->GetViewAngle());
				m_Camera.m_piCamera->SetClippingPlanes(dNearPlane,dFarPlane);
			}

			double cx=m_rRealRect.h*dPlayAreaAspectRatio;
			double dx=(m_rRealRect.w-cx)*0.5;
			piRender->SetViewport(dx,0,cx,m_rRealRect.h);
			piRender->SetPerspectiveProjection(dViewAngle,dNearPlane,dFarPlane);
			piRender->SetCamera(vPosition,vAngles.c[YAW],vAngles.c[PITCH],vAngles.c[ROLL]);

			m_GameRenderWrapper.m_piRender->Render(piRender,piCamera);
		}
		REL(piCamera);
	}
	else
	{
		SetupRenderOptions(piRender);
		piRender->StartStagedRendering();
		
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
			CVector vPos=m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetPosition();
			CVector vAngles=m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetAngles();
			m_vEntityControls[m_nSelectedEntity]->m_piDesignObject->DesignRender(piRender,vPos,vAngles,true);
		}
		piRender->EndStagedRendering();

/*
		if(m_bShowPlayAreaPanel)
		{
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			CVector vPlayAreaMins,vPlayAreaMaxs;
			SPlayAreaConfig vPlayAreaConfig;
			m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetPlayAreaConfig(&vPlayAreaConfig);
			m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetAirPlayPlane(&vPlayAreaMins,&vPlayAreaMaxs);
			CVector vCenter=(vPlayAreaMaxs+vPlayAreaMins)*0.5;
			CVector vSize=(vPlayAreaMaxs-vPlayAreaMins);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_TEXTURE_2D);
			glColor4d(1,1,1,0.1);
			piRender->RenderRect(vCenter,AxisPosX,AxisPosZ,vSize.c[0],vSize.c[2]);
			//Left scroll
			glColor4d(1,1,1,0.05);
			piRender->RenderRect(vCenter-CVector(0,0,vSize.c[2]*0.5+vPlayAreaConfig.dCameraScroll*0.5),AxisPosX,AxisPosZ,vSize.c[0],vPlayAreaConfig.dCameraScroll);
			//Right scroll
			glColor4d(1,1,1,0.05);
			piRender->RenderRect(vCenter+CVector(0,0,vSize.c[2]*0.5+vPlayAreaConfig.dCameraScroll*0.5),AxisPosX,AxisPosZ,vSize.c[0],vPlayAreaConfig.dCameraScroll);

			glPopAttrib();
		}*/

	}
	m_Render.m_piRender->PopOptions();
	m_Render.m_piRender->PopState();

	CVector vSunPosition;
	m_Render.m_piRender->GetSunLight(&vSunPosition,NULL,NULL);
	m_Render.m_piRender->RenderPoint(vSunPosition,10,CVector(0,0,1),1.0);
	m_Render.m_piRender->RenderLine(vSunPosition,vSunPosition+CVector(0,-vSunPosition.c[1],0),CVector(0,0,1),0x8888);
	
	double dAspectRatio=m_rRealRect.h/m_rRealRect.w;
	double dNearPlane=0,dFarPlane=0;
	double dViewAngle=m_Camera.m_piCamera->GetViewAngle();
	CVector vAngles,vPosition;

	m_Camera.m_piCamera->SetAspectRatio(dAspectRatio);
	m_Camera.m_piCamera->GetClippingPlanes(dNearPlane,dFarPlane);
	vAngles=m_Camera.m_piCamera->GetAngles();
	vPosition=m_Camera.m_piCamera->GetPosition();

	if(m_piSTFps)
	{
		char A[200];
		sprintf(A,"Fps: %.02f",m_FrameManager.m_piFrameManager->GetCurrentFps());
		m_piSTFps->SetText(A);
	}
	double dVertexOverhead=0,dFaceOverHead=0;
	bool bOverHeadData=m_WorldManagerWrapper.m_piTerrain->GetTerrainStats(&dVertexOverhead,&dFaceOverHead);
	if(m_piSTVertexOverhead)
	{
		/*if(!bOverHeadData)
		{
			m_piSTVertexOverhead->SetText("Shader: N/A");
		}
		else
		{ */

		//}
	}
	if(m_piSTFaceOverhead)
	{
		CFrame *pOriginalFrame=NULL;
		if(!bOverHeadData)
		{
			m_piSTFaceOverhead->SetText("Faces   : N/A");
		}
		else
		{
			char A[200];
			sprintf(A,"Faces   : %.02f%%",(float)dFaceOverHead);
			m_piSTFaceOverhead->SetText(A);
		}
	}
	UpdateLayerPanel();
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
	std::string sScenario=m_sFile;
	if(OpenFileDialog("Load scenario...","Scenario files (*.ges)\0*.ges\0\0",&sScenario))
	{
		OpenScenario(sScenario);

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
	}
}
void CScenarioEditorMainWindow::ProcessFileExit()
{
	if(ConfirmDialog("Do you really want to exit?","Scenario Editor",eMessageDialogType_Question))
	{
		m_piGUIManager->ExitGUILoop();
	}
}
void GetFrameGeometry(CFrame *pFrame,vector<CPolygon *> *pvGeometry)
{
	for(long x=0;x<pFrame->m_nPolygons;x++)
	{
		CPolygon *pPolygon=new CPolygon;
		*pPolygon=pFrame->m_pPolygons[x];
		pPolygon->CalcPlane();
		pvGeometry->push_back(&pFrame->m_pPolygons[x]);
	}
}

void CScenarioEditorMainWindow::OnButtonClicked(IGameGUIButton *piControl)
{
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
		std::string sBaseModel;
		m_WorldManagerWrapper.m_piTerrain->GetTerrainBaseModel(&sBaseModel,NULL);
		std::string sFile="*.ase";
		if(OpenFileDialog("Select model...","All supported files\0*.ase;*.mdl\0ASE files (*.ase)\0*.ase\0Quake Models (*.mdl)\0*.mdl\0\0",&sBaseModel))
		{
			m_WorldManagerWrapper.m_piTerrain->SetTerrainBaseModel(sBaseModel);
		}
		UpdateTexturization();
		CenterCamera();
	}
	if(m_piBTGeneralChangeColorMap==piControl)
	{
		std::string sColorMap;
		m_WorldManagerWrapper.m_piTerrain->GetTerrainColorMap(&sColorMap,NULL);
		if(OpenFileDialog("Select layer texture...","All supported files\0*.jpg;*.jpeg;*.bmp\0JPEG files (*.jpg)\0*.jpg;*.jpeg\0BMP files (*.bmp)\0*.bmp\0\0",&sColorMap))
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
		}
		for(unsigned long x=0;x<vEntityTypes.size();x++){IDesignObject *piEntityType=vEntityTypes[x];REL(piEntityType);}
	}
	if(m_piBTNewEntityLayer==piControl)
	{
		SEntityLayer layer;
		unsigned long nSelectedEntityType=0;
		std::vector<IDesignObject *> vEntityTypes;
		GetSystemObjects("EntityTypes",&vEntityTypes);
		if(m_ObjectSelector.m_piObjectSelector->SelectObject(this,&vEntityTypes,&nSelectedEntityType))
		{
			ISystemObject *piObject=QI(ISystemObject,vEntityTypes[nSelectedEntityType]);
			layer.sEntityType=piObject?piObject->GetName():"";
			REL(piObject);

			unsigned long nLayerIndex=m_PlayAreaManagerWrapper.m_piPlayAreaDesign->AddEntityLayer(&layer);
			UpdateEntityLayerControls();
			UpdateTexturization();	
			m_nSelectedEntityLayer=m_vEntityLayerControls.size()-1;
			m_nSelectedEntity=-1;	
			m_nSelectedFormation=-1;
		}
		for(unsigned long x=0;x<vEntityTypes.size();x++){IDesignObject *piEntityType=vEntityTypes[x];REL(piEntityType);}

	}
	if(m_piBTNewFormation==piControl)
	{
		unsigned long nSelectedFormationType=0;
		std::vector<IDesignObject *> vFormationTypes;
		GetSystemObjects("FormationTypes",&vFormationTypes);
		SPlayAreaConfig sPlayAreaConfig;
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetPlayAreaConfig(&sPlayAreaConfig);
		if(m_ObjectSelector.m_piObjectSelector->SelectObject(this,&vFormationTypes,&nSelectedFormationType,96.0,(96.0)/sPlayAreaConfig.dCameraAspectRatio))
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
		}
		for(unsigned long x=0;x<vFormationTypes.size();x++){IDesignObject *piFormationType=vFormationTypes[x];REL(piFormationType);}
	}
	if(m_piBTNewColorLayer==piControl)
	{
		STerrainColorLayer layer;
		layer.dHorizontalResolution=1;
		layer.dVerticalResolution=1;
		if(OpenFileDialog("Select layer texture...","All supported files\0*.jpg;*.jpeg;*.bmp\0JPEG files (*.jpg)\0*.jpg;*.jpeg\0BMP files (*.bmp)\0*.bmp\0\0",&layer.sTextureFile))
		{
			unsigned long nLayerIndex=m_WorldManagerWrapper.m_piTerrain->AddTerrainColorLayer(&layer);
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
		if(OpenFileDialog("Select layer texture...","All supported files\0*.jpg;*.jpeg;*.bmp\0JPEG files (*.jpg)\0*.jpg;*.jpeg\0BMP files (*.bmp)\0*.bmp\0\0",&layer.sTextureFile))
		{
			unsigned long nLayerIndex=m_WorldManagerWrapper.m_piTerrain->AddTerrainHeightLayer(&layer);
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
		}
	}
	for(unsigned int x=0;x<m_vEntityControls.size();x++)
	{
		if(m_vEntityControls[x]->m_BTListRow.m_piButton==piControl)
		{
			m_nSelectedEntityLayer=-1;
			m_nSelectedEntity=x;
			m_nSelectedFormation=-1;
		}
	}
	for(unsigned int x=0;x<m_vFormationControls.size();x++)
	{
		if(m_vFormationControls[x]->m_BTListRow.m_piButton==piControl)
		{
			m_nSelectedFormation=x;
			m_nSelectedEntity=-1;
			m_nSelectedEntityLayer=-1;
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
		m_WorldManagerWrapper.m_piTerrain->GetTerrainHeightLayer(m_nSelectedHeightLayer,&heightLayer,NULL);

		if(piControl==m_piBTHeightLayerRemove)
		{
			m_WorldManagerWrapper.m_piTerrain->RemoveTerrainHeightLayer(m_nSelectedHeightLayer);
			UpdateHeightLayerControls();
			UpdateTexturization();
		}
		else if(piControl==m_piBTHeightLayerMoveUp)
		{
			if(m_nSelectedHeightLayer<(m_vHeightLayerControls.size()-1))
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
			if(OpenFileDialog("Open texture...","All supported files\0*.jpg;*.jpeg;*.bmp\0JPEG files (*.jpg)\0*.jpg;*.jpeg\0BMP files (*.bmp)\0*.bmp\0\0",&heightLayer.sTextureFile))
			{
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
		m_WorldManagerWrapper.m_piTerrain->GetTerrainColorLayer(m_nSelectedColorLayer,&colorLayer,NULL);

		if(piControl==m_piBTColorLayerRemove)
		{
			m_WorldManagerWrapper.m_piTerrain->RemoveTerrainColorLayer(m_nSelectedColorLayer);
			UpdateColorLayerControls();
			UpdateTexturization();
		}
		else if(piControl==m_piBTColorLayerMoveUp)
		{
			if(m_nSelectedColorLayer<(m_vColorLayerControls.size()-1))
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
			if(OpenFileDialog("Open texture...","All supported files\0*.jpg;*.jpeg;*.bmp\0JPEG files (*.jpg)\0*.jpg;*.jpeg\0BMP files (*.bmp)\0*.bmp\0\0",&colorLayer.sTextureFile))
			{
				bChange=true;
			}
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
			if(m_ObjectSelector.m_piObjectSelector->SelectObject(this,&vEntityTypes,&nSelectedEntityType))
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
			UpdateTexturization();
		}
		else if(piControl==m_piBTEntitySample)
		{
			unsigned long nSelectedEntityType=0;
			std::vector<IDesignObject *> vEntityTypes;
			GetSystemObjects("EntityTypes",&vEntityTypes);
			if(m_ObjectSelector.m_piObjectSelector->SelectObject(this,&vEntityTypes,&nSelectedEntityType))
			{
				IEntityType *piEntityType=QI(IEntityType,vEntityTypes[nSelectedEntityType]);
				pEntity->m_piPlayAreaEntity->SetEntityType(piEntityType);
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
			UpdateTexturization();
		}
		else if(piControl==m_piBTEntityDecreaseYaw)
		{
			CVector vAngles=pEntity->m_piPlayAreaEntity->GetAngles();
			vAngles.c[YAW]-=15.0;
			if(vAngles.c[YAW]<0){vAngles.c[YAW]=360+vAngles.c[YAW];}
			pEntity->m_piPlayAreaEntity->SetAngles(vAngles);
			UpdateTexturization();
		}/*
		else if(piControl==m_piBTEntityConsiderAsTerrain)
		{
			pObject->m_ObjectConfig.bConsiderAsTerrain=!pObject->m_ObjectConfig.bConsiderAsTerrain;
		}	
		*/
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
			unsigned long nSelectedFormationType=0;
			std::vector<IDesignObject *> vFormationTypes;
			GetSystemObjects("FormationTypes",&vFormationTypes);
			if(m_ObjectSelector.m_piObjectSelector->SelectObject(this,&vFormationTypes,&nSelectedFormationType))
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
			if(m_ObjectSelector.m_piObjectSelector->SelectObject(this,&vEntityTypes,&nSelectedEntityType))
			{
				IEntityType *piEntityType=QI(IEntityType,vEntityTypes[nSelectedEntityType]);
				pFormation->m_piPlayAreaFormation->SetBonusType(piEntityType);
				REL(piEntityType);

				UpdateFormationControls();
			}
			for(unsigned long x=0;x<vEntityTypes.size();x++){IDesignObject *piEntityType=vEntityTypes[x];REL(piEntityType);}
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
	else if(piControl==m_piBTWaterSample)
	{
		if(OpenFileDialog("Open texture...","All supported files\0*.jpg;*.jpeg;*.bmp\0JPEG files (*.jpg)\0*.jpg;*.jpeg\0BMP files (*.bmp)\0*.bmp\0\0",&sWater.sTextureFile1))
		{
			bWaterChanged=true;
		}
	}
	else if(piControl==m_piBTWaterSecondSample)
	{
		if(OpenFileDialog("Open texture...","All supported files\0*.jpg;*.jpeg;*.bmp\0JPEG files (*.jpg)\0*.jpg;*.jpeg\0BMP files (*.bmp)\0*.bmp\0\0",&sWater.sTextureFile2))
		{
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
		sWater.dHeight+=0.02;
		if(sWater.dHeight>1){sWater.dHeight=1;}
		bWaterChanged=true;
	}
	else if(piControl==m_piBTWaterDecreaseHeight)
	{
		sWater.dHeight-=0.02;
		if(sWater.dHeight<0){sWater.dHeight=0;}
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
		if(OpenFileDialog("Open texture...","All supported files\0*.jpg;*.jpeg;*.bmp\0JPEG files (*.jpg)\0*.jpg;*.jpeg\0BMP files (*.bmp)\0*.bmp\0\0",&sSky.sTextureFile))
		{
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
		CVector vMins,vMaxs,vSize;
		IGenericModel *piModel;
		m_WorldManagerWrapper.m_piTerrain->GetTerrainModel(&piModel);
		m_WorldManagerWrapper.m_piTerrain->GetTerrainWater(&sWaterConfig,NULL,NULL);
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetPlayAreaConfig(&sPlayAreaConfig);
		if(piModel){piModel->GetFrameBBox(0,0,&vMins,&vMaxs);}
		if(piModel){vSize=piModel->GetFrameSize(0,0);}
		REL(piModel);

		//sFog.dStart=sPlayAreaConfig.dCameraDistance+sPlayAreaConfig.dAirPlaneHeight+(1.0-sWater.dHeight)*vSize.c[1];
		//sFog.dEnd=sPlayAreaConfig.dCameraDistance+sPlayAreaConfig.dAirPlaneHeight+vSize.c[1];

		sFog.dStart=vMins.c[1];
		sFog.dEnd=vMins.c[1]+sWater.dHeight*vSize.c[1];
		bFogChanged=true;
	}
	if(piControl==m_piBTFogFitSky)
	{
		STerrainWater sWaterConfig;
		SPlayAreaConfig sPlayAreaConfig;
		CVector vMins,vMaxs,vSize;
		IGenericModel *piModel;
		m_WorldManagerWrapper.m_piTerrain->GetTerrainModel(&piModel);
		m_WorldManagerWrapper.m_piTerrain->GetTerrainWater(&sWaterConfig,NULL,NULL);
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetPlayAreaConfig(&sPlayAreaConfig);
		if(piModel){piModel->GetFrameBBox(0,0,&vMins,&vMaxs);}
		if(piModel){vSize=piModel->GetFrameSize(0,0);}
		REL(piModel);

		sFog.dStart=sPlayAreaConfig.dCameraDistance;
		sFog.dEnd=sPlayAreaConfig.dCameraDistance+sPlayAreaConfig.dAirPlaneHeight+(1.0-sWater.dHeight)*vSize.c[1];
		bFogChanged=true;
	}
	else if(piControl==m_piBTFogColorSample)
	{
		if(SelectColorDialog("Select fog color...",&sFog.vColor))
		{
			bFogChanged=true;
		}
	}
	else if(piControl==m_piBTFogIncreaseStart)
	{
		sFog.dStart+=1;
		if(sFog.dStart+1>sFog.dEnd){sFog.dStart=sFog.dEnd-1;}
		bFogChanged=true;
	}
	else if(piControl==m_piBTFogDecreaseStart)
	{
		sFog.dStart-=1;
		bFogChanged=true;
	}
	else if(piControl==m_piBTFogIncreaseEnd)
	{
		sFog.dEnd+=1;
		bFogChanged=true;
	}
	else if(piControl==m_piBTFogDecreaseEnd)
	{
		sFog.dEnd-=1;
		if(sFog.dEnd-1<sFog.dStart){sFog.dEnd=sFog.dStart+1;}
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
		if(SelectColorDialog("Select sun color...",&sSun.vColor))
		{
			bSunChanged=true;
		}
	}
	else if(piControl==m_piBTAmbientColorSample)
	{
		if(SelectColorDialog("Select ambient color...",&vAmbientColor))
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
	SPlayAreaConfig sPlayAreaConfig;
	if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign)
	{
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetPlayAreaConfig(&sPlayAreaConfig);
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
	if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign && bPlayAreaChange)
	{
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->SetPlayAreaConfig(&sPlayAreaConfig);
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
}

void CScenarioEditorMainWindow::UpdateCaption()
{
	if(m_Viewport.m_piViewport)
	{
		std::string sCaption="Scenario Editor";
		if(m_sFile.length()){sCaption+=" - "+m_sFile;}
		m_Viewport.m_piViewport->SetCaption(sCaption);
	}
};

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
	m_piGRSunPanel->Show(m_bShowSunPanel && m_bShowTerrainPanel);
	m_piGRSkyPanel->Show(m_bShowSkyPanel && m_bShowTerrainPanel);
	m_piGRPlayAreaPanel->Show(m_bShowPlayAreaPanel && m_bShowTerrainPanel);
	m_piGREntitiesPanel->Show(m_bShowEntitiesPanel);
	m_piGRFormationsPanel->Show(m_bShowFormationsPanel);
	m_piGROptionsPanel->Show(m_bShowOptionsPanel);
	m_piGRTerrainPanel->Show(m_bShowTerrainPanel);
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
		m_WorldManagerWrapper.m_piTerrain->GetTerrainHeightLayer(m_nSelectedHeightLayer,&layer,&piTexture);
		m_piBTHeightLayerSample->SetBackgroundTexture(piTexture);
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
		m_piBTHeightLayerIncreaseMaxHeight->Show(m_nSelectedHeightLayer<(m_vHeightLayerControls.size()-1));
		m_piBTHeightLayerDecreaseMaxHeight->Show(m_nSelectedHeightLayer<(m_vHeightLayerControls.size()-1));

		REL(piTexture);
	}
	else if(m_nSelectedColorLayer!=-1)
	{
		STerrainColorLayer layer;
		IGenericTexture *piTexture=NULL;
		m_WorldManagerWrapper.m_piTerrain->GetTerrainColorLayer(m_nSelectedColorLayer,&layer,&piTexture);
		m_piBTColorLayerSample->SetBackgroundTexture(piTexture);
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
		m_piGREntityPanel->Show(m_bShowEntitiesPanel);

		CVector vAngles=m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetAngles();
		char A[MAX_PATH];
		sprintf(A,"Yaw      : %.02f",vAngles.c[YAW]);
		m_piSTEntityYaw->SetText(A);
/*
		sprintf(A,"Height   : %.02f",m_vEntityControls[m_nSelectedEntity]->m_ObjectConfig.dHeight);
		m_piSTEntityHeight->SetText(A);

		m_piBTEntityGroundTied->SetText(m_vEntityControls[m_nSelectedEntity]->m_ObjectConfig.bGroundTied?"From Ground":"Absolute");
		m_piBTEntityConsiderAsTerrain->SetText(m_vEntityControls[m_nSelectedEntity]->m_ObjectConfig.bConsiderAsTerrain?"Is Terrain":"Is Object");
		m_piBTEntityLocked->SetText(m_vEntityControls[m_nSelectedEntity]->m_ObjectConfig.bLocked?"Locked":"Unlocked");
		m_piBTEntityLocked->SetBackgroundColor(m_vEntityControls[m_nSelectedEntity]->m_ObjectConfig.bLocked?CVector(0.8,0.8,0.0):CVector(0.6,0.6,0.0),1.0);
*/
	}
	if(m_nSelectedFormation!=-1)
	{
		m_piSTFormationName->SetText(m_vFormationControls[m_nSelectedFormation]->m_piObject->GetName());
		m_piSTFormationObjectLabel->SetObject(m_vFormationControls[m_nSelectedFormation]->m_piDesignObject);
		m_piSTFormationBonusObjectLabel->SetObject(m_vFormationControls[m_nSelectedFormation]->m_piBonusDesignObject);
		m_piGRFormationPanel->Show(m_bShowFormationsPanel);
	}
	STerrainWater sWater;
	IGenericTexture *piWaterTexture1=NULL;
	IGenericTexture *piWaterTexture2=NULL;
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

	sprintf(A,"Height  : %.02f",sWater.dHeight);
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
	IGenericTexture *piFogTexture1=NULL;
	IGenericTexture *piFogTexture2=NULL;
	if(m_WorldManagerWrapper.m_piTerrain)
	{
		m_WorldManagerWrapper.m_piTerrain->GetTerrainFog(&sFog);
	}

	m_piBTFogColorSample->SetBackgroundColor(sFog.vColor,1);

	sprintf(A,"Min    : %.f",sFog.dStart);
	m_piSTFogStart->SetText(A);

	sprintf(A,"Max    : %.f",sFog.dEnd);
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
		m_WorldManagerWrapper.m_piTerrain->GetTerrainColorLayer(x,&layer,&piTexture);

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
		m_WorldManagerWrapper.m_piTerrain->GetTerrainHeightLayer(x,&layer,&piTexture);

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

	if(m_piGRFormationsPanel){m_piGRFormationsPanel->GetRealRect(&sListRect);}
	dCurrentY=sListRect.h;

	if(m_piBTNewFormation)
	{
		dCurrentY-=20+6;
		SGameRect sRowRect(3,dCurrentY,sListRect.w-6,20);
		m_piBTNewFormation->SetRect(&sRowRect);
	}
	dCurrentX=3;
	if(m_vFormationControls.size()){dCurrentY-=41;}
	for(int x=m_vFormationControls.size()-1;x>=0;x--)
	{	
		if(dCurrentX+38>sListRect.w){dCurrentX=3;dCurrentY-=41;}

		SGameRect sRowRect(dCurrentX,dCurrentY,38,38);
		SFormationControls *pData=m_vFormationControls[x];
		if(pData->m_BTListRow.m_piButton)
		{
			pData->m_BTListRow.m_piButton->SetRect(&sRowRect);
			pData->m_BTListRow.m_piButton->SetBackgroundColor(CVector(1,1,1),x==m_nSelectedFormation?0.8:0.1);
		}
		if(pData->m_STFormation.m_piLabel)
		{
			SGameRect sRect(3,3,32,32);
			pData->m_STFormation.m_piLabel->SetRect(&sRect);
			pData->m_STFormation.m_piLabel->SetObject(m_vFormationControls[x]->m_piDesignObject);
		}
		dCurrentX+=41;
	}

	std::string sModelName;
	if(m_WorldManagerWrapper.m_piTerrain){m_WorldManagerWrapper.m_piTerrain->GetTerrainBaseModel(&sModelName,NULL);}
	sprintf(A,"Model    : %s",sModelName.c_str());
	m_piSTGeneralModelName->SetText(A);

	std::string sColorMap;
	if(m_WorldManagerWrapper.m_piTerrain){m_WorldManagerWrapper.m_piTerrain->GetTerrainColorMap(&sColorMap,NULL);}
	sprintf(A,"Color Map: %s",sColorMap.c_str());
	m_piSTGeneralColorMapName->SetText(A);
}

void CScenarioEditorMainWindow::CenterCamera()
{
	bool bCenter=false;
	CVector vCenter,vSize;
	if(m_nSelectedFormation!=-1 && m_vFormationControls[m_nSelectedFormation]->m_piDesignObject)
	{
		CVector vMins,vMaxs;
		m_vFormationControls[m_nSelectedFormation]->m_piDesignObject->DesignGetBBox(&vMins,&vMaxs);
		CVector vPos=m_vFormationControls[m_nSelectedFormation]->m_piPlayAreaFormation->GetPosition();
		bCenter=true;
		vCenter=vPos+(vMaxs+vMins)*0.5;
		vSize=(vMaxs-vMins);
	}
	else if(m_nSelectedEntity!=-1 && m_vEntityControls[m_nSelectedEntity]->m_piDesignObject)
	{
		CVector vMins,vMaxs;
		m_vEntityControls[m_nSelectedEntity]->m_piDesignObject->DesignGetBBox(&vMins,&vMaxs);
		CVector vPos=m_vEntityControls[m_nSelectedEntity]->m_piPlayAreaEntity->GetPosition();
		bCenter=true;
		vCenter=vPos+(vMaxs+vMins)*0.5;
		vSize=(vMaxs-vMins);
	}
	else 
	{
		CVector vMins,vMaxs;
		IGenericModel *piModel=NULL;
		m_WorldManagerWrapper.m_piTerrain->GetTerrainModel(&piModel);
		if(piModel){piModel->GetFrameBBox(0,0,&vMins,&vMaxs);}
		REL(piModel);

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

void CScenarioEditorMainWindow::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==VK_ESCAPE)
	{
		*pbProcessed=true;

		ProcessFileExit();
	}
}

void CScenarioEditorMainWindow::OnMouseDown( int nButton,double dx,double dy )
{
	if(m_vEntityControls.size()==0 && m_vFormationControls.size()==0){return;}

	GLint  viewport[4]={0};
	GLuint buffer[1024];
	GLuint *pSelectionBuffer=buffer;

	double dNearPlane=0,dFarPlane=0;
	m_Camera.m_piCamera->GetClippingPlanes(dNearPlane,dFarPlane);

	glSelectBuffer(1024,pSelectionBuffer);
	glRenderMode(GL_SELECT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glGetIntegerv(GL_VIEWPORT,viewport);
	glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
	gluPickMatrix(m_rRealRect.x+dx,m_rRealRect.y+dy,2,2,viewport);
	gluPerspective(m_Camera.m_piCamera->GetViewAngle(),m_rRealRect.w/m_rRealRect.h,dNearPlane,dFarPlane);

	double dRoll=m_Camera.m_piCamera->GetAngles().c[ROLL];
	double dPitch=m_Camera.m_piCamera->GetAngles().c[PITCH];
	double dYaw=m_Camera.m_piCamera->GetAngles().c[YAW];
	CVector vPosition=m_Camera.m_piCamera->GetPosition();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLoadIdentity();
	glRotated((0-dRoll)	,AxisPosZ.c[0],AxisPosZ.c[1],AxisPosZ.c[2]);
	glRotated((0-dPitch),AxisPosX.c[0],AxisPosX.c[1],AxisPosX.c[2]);
	glRotated((0-(dYaw-90))	,AxisPosY.c[0],AxisPosY.c[1],AxisPosY.c[2]);
	glTranslated(-vPosition.c[0],-vPosition.c[1],-vPosition.c[2]);
	glInitNames();

	for(int x=0;x<m_vEntityControls.size();x++)
	{
		glPushName(x);
		SEntityControls *pEntity=m_vEntityControls[x];
		pEntity->m_piPlayAreaEntity->DesignRender(m_Render.m_piRender,false);
		glPopName();
	}
	for(int x=0;x<m_vFormationControls.size();x++)
	{
		glPushName(SELECT_FORMATION_BASE_INDEX+x);
		SFormationControls *pFormation=m_vFormationControls[x];
		pFormation->m_piPlayAreaFormation->DesignRender(m_Render.m_piRender,false);
		glPopName();
	}

	glFlush();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	int nNewIndex=-1;
	double dCurrentMin=100000000.0;
	int nHits = glRenderMode(GL_RENDER);
	if(nHits)
	{
		for(int x=0;x<nHits;x++)
		{
			int nIds=*pSelectionBuffer++;
			int nMin=*pSelectionBuffer++;
			int nMax=*pSelectionBuffer++;
			for(int y=0;y<nIds;y++,pSelectionBuffer++)
			{
				if(y==0 && nMin<dCurrentMin)
				{
					nNewIndex=*pSelectionBuffer;
					dCurrentMin=nMin;
				}
			}
		}
	}
	if(nNewIndex==-1)
	{
		m_nSelectedFormation=-1;
		m_nSelectedEntity=-1;
	}
	else if(nNewIndex<SELECT_FORMATION_BASE_INDEX)
	{
		int nNewSelection=nNewIndex;
		if(m_nSelectedEntity==nNewSelection)
		{
			if(nNewSelection!=-1 /*&& !m_vEntityControls[nNewSelection]->m_ObjectConfig.bLocked*/)
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
		}
	}
	else
	{
		int nNewSelection=nNewIndex-SELECT_FORMATION_BASE_INDEX;
		if(m_nSelectedFormation==nNewSelection)
		{
			if(nNewSelection!=-1 /*&& !m_vEntityControls[nNewSelection]->m_ObjectConfig.bLocked*/)
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
		}
	}

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
	double dRightDisplace=(x-m_rRealRect.w*0.5)/(m_rRealRect.w*0.5);
	double dUpDisplace=(y-m_rRealRect.h*0.5)/(m_rRealRect.h*0.5);
	dRightDisplace*=m_rRealRect.w/m_rRealRect.h;

	double dRightAngle=m_Camera.m_piCamera->GetViewAngle()*0.5*dRightDisplace;
	double dUpAngle=m_Camera.m_piCamera->GetViewAngle()*0.5*dUpDisplace;
	CVector vPoint1,vPoint2;
	vPoint1=m_Camera.m_piCamera->GetPosition();
	vPoint2=m_Camera.m_piCamera->GetPosition()+m_Camera.m_piCamera->GetForwardVector()*1000.0;
	vPoint2+=m_Camera.m_piCamera->GetRightVector()*1000.0*sin(DegreesToRadians(dRightAngle));
	vPoint2+=m_Camera.m_piCamera->GetUpVector()*1000.0*sin(DegreesToRadians(dUpAngle));


	double dAirPlaneHeight=GetAirPlaneAbsoluteHeight();

	CPlane airPlane(AxisPosY,CVector(0,dAirPlaneHeight,0));
	double dSide1=airPlane.GetSide(vPoint1);
	double dSide2=airPlane.GetSide(vPoint2);
	double dLength=(dSide1-dSide2);
	double dFraction=dLength?dSide1/dLength:0;
	CVector vPos=vPoint1+(vPoint2-vPoint1)*dFraction;

	if(m_PlayAreaManagerWrapper.m_piPlayAreaManager)
	{
		CVector vPlayerRouteStart,vPlayerRouteEnd;
		CVector vPlayAreaMins,vPlayAreaMaxs;
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayerRoute(&vPlayerRouteStart,&vPlayerRouteEnd);
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayAreaPlaneAt(vPos,&vPlayAreaMins,&vPlayAreaMaxs);
		// Se mueve la posicion para que el centro de la camara sea el centro del play area
		vPos.c[0]+=(vPlayAreaMaxs.c[0]-vPlayAreaMins.c[0])*0.5;
		// Center the coordinate in the player route
		vPos.c[2]=vPlayerRouteStart.c[2];
	}


	if(pAirPlanePos){*pAirPlanePos=vPos;}

	return (dSide1*dSide2)<0;
}

bool CScenarioEditorMainWindow::GetTerrainCoordinatesFromCursorPos(double x,double y,bool bIgnoreTerrainObjects, CVector *pTerrainPos)
{
	double dRightDisplace=(x-m_rRealRect.w*0.5)/(m_rRealRect.w*0.5);
	double dUpDisplace=(y-m_rRealRect.h*0.5)/(m_rRealRect.h*0.5);
	dRightDisplace*=m_rRealRect.w/m_rRealRect.h;

	double dRightAngle=m_Camera.m_piCamera->GetViewAngle()*0.5*dRightDisplace;
	double dUpAngle=m_Camera.m_piCamera->GetViewAngle()*0.5*dUpDisplace;
	CVector vPoint1,vPoint2;
	vPoint1=m_Camera.m_piCamera->GetPosition();
	vPoint2=m_Camera.m_piCamera->GetPosition()+m_Camera.m_piCamera->GetForwardVector()*1000.0;
	vPoint2+=m_Camera.m_piCamera->GetRightVector()*1000.0*sin(DegreesToRadians(dRightAngle));
	vPoint2+=m_Camera.m_piCamera->GetUpVector()*1000.0*sin(DegreesToRadians(dUpAngle));

	if(m_WorldManagerWrapper.m_piTerrain)
	{
		bool bHit=m_WorldManagerWrapper.m_piTerrain->GetTerrainTrace(vPoint1,vPoint2,pTerrainPos);
		for(int x=0;x<m_vEntityControls.size();x++)
		{
			if(x==m_nSelectedEntity){continue;}
			CVector vPos=m_vEntityControls[x]->m_piPlayAreaEntity->GetPosition();
			CVector vAngles=m_vEntityControls[x]->m_piPlayAreaEntity->GetAngles();
			CTraceInfo info=m_vEntityControls[x]->m_piDesignObject->DesignGetTrace(vPos,vAngles,vPoint1,vPoint2);
			if(info.m_bTraceHit)
			{
				if((info.m_vTracePos-vPoint1)<(*pTerrainPos-vPoint1))
				{
					*pTerrainPos=info.m_vTracePos;
				}
			}
		}
		pTerrainPos->c[1]+=FP_PRECISION;
		return bHit;
	}
	return false;
}

void CScenarioEditorMainWindow::OnMouseMove( double x,double y )
{
	if(m_bMovingObject && m_nSelectedEntity!=-1)
	{
		SEntityControls *pObject=m_vEntityControls[m_nSelectedEntity];
		CVector vTemp;
		if(GetTerrainCoordinatesFromCursorPos(x,y,false,&vTemp))
		{
			pObject->m_piPlayAreaEntity->SetPosition(vTemp);
		}
	}
	if(m_bMovingObject && m_nSelectedFormation!=-1)
	{
		SFormationControls *pObject=m_vFormationControls[m_nSelectedFormation];
		CVector vTemp;
		if(GetAirPlaneCoordinatesFromCursorPos(x,y,&vTemp))
		{
			pObject->m_piPlayAreaFormation->SetPosition(m_vObjectOriginalPosition+(vTemp-m_vCursorOriginalPosition));
		}
	}
}

void CScenarioEditorMainWindow::OnMouseUp( int nButton,double x,double y )
{
	IGameWindow *piCapture=m_piGUIManager->GetMouseCapture();
	if(piCapture==this){m_piGUIManager->ReleaseMouseCapture();}
	REL(piCapture);
	m_bMovingObject=false;
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
	m_GameControllerWrapper.m_piGameController->Start();
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
	if(m_nSelectedEntityLayer!=-1 && m_nSelectedEntityLayer>=m_vEntityLayerControls.size())
	{
		m_nSelectedEntityLayer=m_vEntityLayerControls.size()-1;
	}
}

void CScenarioEditorMainWindow::UpdateEntityControls()
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
	if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign)
	{
		for(x=0;x<m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetElements();x++)
		{
			IPlayAreaElement *piElement=NULL;
			m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetElement(x,&piElement);
			IPlayAreaEntity  *piEntity=QI(IPlayAreaEntity,piElement);
			IEntityType		 *piEntityType=NULL;
			if(piEntity){piEntity->GetEntityType(&piEntityType);}			
			IDesignObject *piDesignObject=QI(IDesignObject,piEntityType);
			ISystemObject *piObject=QI(ISystemObject,piDesignObject);
			if(!piObject)
			{
				REL(piEntityType);
				REL(piEntity);
				REL(piElement);
				REL(piDesignObject);
				continue;
			}

			SEntityControls *pControls=new SEntityControls;
			pControls->m_nPlayAreaElementId=x;
			pControls->m_BTListRow.Create(m_piSystem,"CGameGUIButton","");
			pControls->m_piEntityType=ADD(piEntityType);
			pControls->m_piDesignObject=ADD(piDesignObject);
			pControls->m_piPlayAreaEntity=ADD(piEntity);
			pControls->m_piObject=QI(ISystemObject,piEntityType);
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
			}
			REL(piEntityType);
			REL(piDesignObject);
			REL(piElement);
			REL(piEntity);
			REL(piObject);
			m_vEntityControls.push_back(pControls);
		}
	}
	if(m_nSelectedEntity!=-1 && m_nSelectedEntity>=m_vEntityControls.size())
	{
		m_nSelectedEntity=m_vEntityControls.size()-1;
	}
}

void CScenarioEditorMainWindow::UpdateFormationControls()
{
	unsigned int x;
	std::vector<SFormationControls *>::iterator i;
	for(x=0;x<m_vFormationControls.size();x++)
	{
		SFormationControls *pControls=m_vFormationControls[x];
		UNSUBSCRIBE_FROM_CAST(pControls->m_BTListRow.m_piButton,IGameGUIButtonEvents);
		pControls->m_BTListRow.m_piButton->DestroyWindow();
		pControls->m_STFormation.m_piLabel->DestroyWindow();
		delete pControls;
		pControls=NULL;
	}
	m_vFormationControls.clear();
	if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign)
	{
		for(x=0;x<m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetElements();x++)
		{
			IPlayAreaElement *piElement=NULL;
			m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetElement(x,&piElement);
			IPlayAreaFormation  *piFormation=QI(IPlayAreaFormation,piElement);
			IFormationType		*piFormationType=NULL;
			IEntityType			*piBonusType=NULL;
			if(piFormation){piFormation->GetFormationType(&piFormationType);}			
			if(piFormation){piFormation->GetBonusType(&piBonusType);}			
			IDesignObject *piBonusDesignObject=QI(IDesignObject,piBonusType);
			IDesignObject *piDesignObject=QI(IDesignObject,piFormationType);
			ISystemObject *piObject=QI(ISystemObject,piDesignObject);
			if(!piObject)
			{
				REL(piBonusType);
				REL(piBonusDesignObject);
				REL(piFormationType);
				REL(piFormation);
				REL(piElement);
				REL(piDesignObject);
				continue;
			}

			SFormationControls *pControls=new SFormationControls;
			pControls->m_nPlayAreaElementId=x;
			pControls->m_BTListRow.Create(m_piSystem,"CGameGUIButton","");
			pControls->m_piFormationType=ADD(piFormationType);
			pControls->m_piDesignObject=ADD(piDesignObject);
			pControls->m_piPlayAreaFormation=ADD(piFormation);
			pControls->m_piObject=QI(ISystemObject,piFormationType);
			pControls->m_piBonusDesignObject=ADD(piBonusDesignObject);
			if(pControls->m_BTListRow.m_piButton)
			{
				pControls->m_BTListRow.m_piButton->InitWindow(m_piGRFormationsPanel,false);
				pControls->m_BTListRow.m_piButton->SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
				pControls->m_BTListRow.m_piButton->SetBackgroundColor(CVector(1,1,1),0.1);
				pControls->m_BTListRow.m_piButton->Show(true);
				pControls->m_BTListRow.m_piButton->Activate(true);
				SUBSCRIBE_TO_CAST(pControls->m_BTListRow.m_piButton,IGameGUIButtonEvents);

				pControls->m_STFormation.Create(m_piSystem,"CScenarioEditorObjectLabel","");
				if(pControls->m_STFormation.m_piLabel)
				{
					pControls->m_STFormation.m_piLabel->InitWindow(pControls->m_BTListRow.m_piButton,false);
					pControls->m_STFormation.m_piLabel->SetObject(piDesignObject);
					pControls->m_STFormation.m_piLabel->SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
					pControls->m_STFormation.m_piLabel->SetBackgroundColor(CVector(0,0,0),1);
					pControls->m_STFormation.m_piLabel->Show(true);
					pControls->m_STFormation.m_piLabel->Activate(false);
				}
			}
			REL(piBonusType);
			REL(piBonusDesignObject);
			REL(piFormationType);
			REL(piDesignObject);
			REL(piElement);
			REL(piFormation);
			REL(piObject);
			m_vFormationControls.push_back(pControls);
		}
	}
	if(m_nSelectedFormation!=-1 && m_nSelectedFormation>=m_vFormationControls.size())
	{
		m_nSelectedFormation=m_vFormationControls.size()-1;
		m_nSelectedEntity=-1;
		m_nSelectedEntityLayer=-1;
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
			m_WorldManagerWrapper.m_piTerrain->GetTerrainColorLayer(x,&sLayer,NULL);

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
	if(m_nSelectedColorLayer!=-1 && m_nSelectedColorLayer>=m_vColorLayerControls.size())
	{
		m_nSelectedColorLayer=m_vColorLayerControls.size()-1;
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
			m_WorldManagerWrapper.m_piTerrain->GetTerrainHeightLayer(x,&sLayer,NULL);
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
	if(m_nSelectedHeightLayer!=-1 && m_nSelectedHeightLayer>=m_vHeightLayerControls.size())
	{
		m_nSelectedHeightLayer=m_vHeightLayerControls.size()-1;
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
	IGenericModel *piModel=NULL;
	m_WorldManagerWrapper.m_piTerrain->GetTerrainModel(&piModel);
	if(piModel)
	{
		CVector vMins,vMaxs;
		piModel->GetFrameBBox(0,0,&vMins,&vMaxs);
		dAirPlaneHeight=vMaxs.c[1];
	}
	SPlayAreaConfig sPlayAreaConfig;
	if(m_PlayAreaManagerWrapper.m_piPlayAreaDesign)
	{
		m_PlayAreaManagerWrapper.m_piPlayAreaDesign->GetPlayAreaConfig(&sPlayAreaConfig);
		dAirPlaneHeight+=sPlayAreaConfig.dAirPlaneHeight;
	}
	REL(piModel);
	return dAirPlaneHeight;
}

void CScenarioEditorMainWindow::OpenScenario( std::string sScenario )
{
	Reset();
	m_GameControllerWrapper.m_piGameController->LoadScenario(sScenario);	
	m_sFile=sScenario;
	UpdateColorLayerControls();
	UpdateHeightLayerControls();
	UpdateEntityLayerControls();
	UpdateEntityControls();
	UpdateFormationControls();
	UpdateLayerPanel();
	UpdateCaption();
	CenterCamera();
}