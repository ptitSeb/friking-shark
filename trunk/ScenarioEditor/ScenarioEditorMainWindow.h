#pragma once

#include "ScenarioEditor.h"
#include "ScenarioEditorColorSelector.h"
#include "ScenarioEditorObjectSelector.h"

DECLARE_CUSTOM_WRAPPER1(CScenarioEditorColorSelectorWrapper,IScenarioEditorColorSelector,m_piColorSelector);
DECLARE_CUSTOM_WRAPPER1(CScenarioEditorObjectSelectorWrapper,IScenarioEditorObjectSelector,m_piObjectSelector);

#define KEY_FORWARD		1
#define KEY_BACK		2
#define KEY_RIGHT		3
#define KEY_LEFT		4
#define KEY_UP			5
#define KEY_DOWN		6
#define KEY_PAUSE		7
#define KEY_PROCESS_ONE_FRAME		8

struct SHeightLayerControls
{
	CButtonWrapper						m_BTListRow;
	CLabelWrapper						m_STTexture;
	CLabelWrapper						m_STDescription;
};

struct SColorLayerControls
{
	CButtonWrapper					m_BTListRow;
	CLabelWrapper					m_STTexture;
	CLabelWrapper					m_STDescription;
};

struct SEntityLayerControls
{
	CButtonWrapper					m_BTListRow;
	CObjectLabelWrapper				m_STEntity;
	CLabelWrapper					m_STDescription;
};

struct SEntityControls
{
	CButtonWrapper					m_BTListRow;
	CObjectLabelWrapper				m_STEntity;
	unsigned long					m_nPlayAreaElementId;
	IPlayAreaEntity					*m_piPlayAreaEntity;
	IEntityType						*m_piEntityType;
	IDesignObject					*m_piDesignObject;
	ISystemObject					*m_piObject;
	
	SEntityControls(){m_piPlayAreaEntity=NULL;m_piEntityType=NULL;m_piDesignObject=NULL;m_piObject=NULL;m_nPlayAreaElementId=0;}
	~SEntityControls(){REL(m_piPlayAreaEntity);REL(m_piEntityType);REL(m_piDesignObject);REL(m_piObject);}
};

struct SFormationControls
{
	CButtonWrapper					m_BTListRow;
	CObjectLabelWrapper				m_STFormation;
	unsigned long					m_nPlayAreaElementId;
	IPlayAreaFormation				*m_piPlayAreaFormation;
	IFormationType					*m_piFormationType;
	IDesignObject					*m_piDesignObject;
	ISystemObject					*m_piObject;
	IDesignObject					*m_piBonusDesignObject;

	SFormationControls(){m_piPlayAreaFormation=NULL;m_piFormationType=NULL;m_piDesignObject=NULL;m_piObject=NULL;m_piBonusDesignObject=NULL;m_nPlayAreaElementId=0;}
	~SFormationControls(){REL(m_piPlayAreaFormation);REL(m_piFormationType);REL(m_piDesignObject);REL(m_piObject);REL(m_piBonusDesignObject);}
};

class CScenarioEditorMainWindow: public CGameWindowBase, public IGameGUIButtonEvents
{
public:
	CConfigFile				m_GUIConfigFile;
	CViewportWrapper		m_Viewport;
	CRenderWrapper			m_Render;
	CGenericCameraWrapper	m_Camera;
	CFrameManagerWrapper	m_FrameManager;

	std::vector<SHeightLayerControls *> m_vHeightLayerControls;
	std::vector<SColorLayerControls *>	m_vColorLayerControls;
	std::vector<SEntityLayerControls *> m_vEntityLayerControls;
	std::vector<SEntityControls *>		m_vEntityControls;
	std::vector<SFormationControls *>	m_vFormationControls;
	std::string							m_sFile;

	unsigned long		m_dwNexControlKey;
	bool				m_bBlend;
	bool				m_bSolid;
	bool				m_bTextures;
	bool				m_bFog;
	bool				m_bShaders;
	bool				m_bColors;
	bool				m_bShadows;
	bool				m_bLighting;
	bool				m_bShowFilePanel;
	bool				m_bShowEntitiesPanel;
	bool				m_bShowEntityPanel;
	bool				m_bShowFormationsPanel;
	bool				m_bShowFormationPanel;
	bool				m_bShowTerrainPanel;
	bool				m_bShowOptionsPanel;
	bool				m_bShowGeneralPanel;
	bool				m_bShowWaterPanel;
	bool				m_bShowFogPanel;
	bool				m_bShowSunPanel;
	bool				m_bShowSkyPanel;
	bool				m_bShowPlayAreaPanel;

	bool				m_bMovingObject;
	CVector				m_vObjectOriginalPosition;
	CVector				m_vCursorOriginalPosition;

	int					m_nSelectedHeightLayer;
	int					m_nSelectedColorLayer;
	int					m_nSelectedEntityLayer;
	int					m_nSelectedEntity;
	int					m_nSelectedFormation;

	bool				m_bAutoGenerateBSP;
	bool				m_bAutoUpdateBSP;


	IGameGUILabel *m_piSTFps;
	IGameGUILabel *m_piSTVertexOverhead;
	IGameGUILabel *m_piSTFaceOverhead;

	IGameWindow	  *m_piGRHeightLayerPanel;
	IGameWindow	  *m_piGRColorLayerPanel;
	IGameWindow	  *m_piGREntityLayerPanel;
	IGameWindow	  *m_piGREntityPanel;
	IGameWindow	  *m_piGRFormationPanel;
	IGameWindow	  *m_piGRGeneralPanel;
	IGameWindow	  *m_piGRWaterPanel;
	IGameWindow	  *m_piGRFogPanel;
	IGameWindow	  *m_piGRSunPanel;
	IGameWindow	  *m_piGRSkyPanel;
	IGameWindow	  *m_piGRPlayAreaPanel;
	IGameWindow	  *m_piGREntitiesPanel;
	IGameWindow	  *m_piGRFormationsPanel;
	IGameWindow	  *m_piGROptionsPanel;
	IGameWindow	  *m_piGRTerrainPanel;
	IGameWindow	  *m_piGRLayerList;

	IGameGUIButton *m_piBTShowOptionsPanel;
	IGameGUIButton *m_piBTShowEntitiesPanel;
	IGameGUIButton *m_piBTShowFormationsPanel;
	IGameGUIButton *m_piBTShowTerrainPanel;
	IGameGUIButton *m_piBTShowFilePanel;
	IGameGUIButton *m_piBTShowGeneralProperties;
	IGameGUIButton *m_piBTShowWaterProperties;
	IGameGUIButton *m_piBTShowFogProperties;
	IGameGUIButton *m_piBTShowSunProperties;
	IGameGUIButton *m_piBTShowSkyProperties;
	IGameGUIButton *m_piBTShowPlayAreaProperties;

	// Height Layers

	IGameGUIButton *m_piBTNewHeightLayer;
	IGameGUILabel  *m_piSTHeightLayerName;
	IGameGUIButton *m_piBTHeightLayerSample;
	IGameGUILabel  *m_piSTHeightLayerMaxHeight;
	IGameGUIButton *m_piBTHeightLayerDecreaseMaxHeight;
	IGameGUIButton *m_piBTHeightLayerIncreaseMaxHeight;
	IGameGUILabel  *m_piSTHeightLayerMinHeight;
	IGameGUIButton *m_piBTHeightLayerDecreaseMinHeight;
	IGameGUIButton *m_piBTHeightLayerIncreaseMinHeight;
	IGameGUILabel  *m_piSTHeightLayerMargin;
	IGameGUIButton *m_piBTHeightLayerDecreaseMargin;
	IGameGUIButton *m_piBTHeightLayerIncreaseMargin;
	IGameGUILabel  *m_piSTHeightLayerHorzResolution;
	IGameGUIButton *m_piBTHeightLayerDecreaseHorzResolution;
	IGameGUIButton *m_piBTHeightLayerIncreaseHorzResolution;
	IGameGUILabel  *m_piSTHeightLayerVertResolution;
	IGameGUIButton *m_piBTHeightLayerDecreaseVertResolution;
	IGameGUIButton *m_piBTHeightLayerIncreaseVertResolution;
	IGameGUIButton *m_piBTHeightLayerRemove;
	IGameGUIButton *m_piBTHeightLayerMoveUp;
	IGameGUIButton *m_piBTHeightLayerMoveDown;

	// Color Layers

	IGameGUIButton *m_piBTNewColorLayer;
	IGameGUILabel  *m_piSTColorLayerName;
	IGameGUIButton *m_piBTColorLayerSample;
	IGameGUIButton *m_piBTColorLayerColorSample;
	IGameGUILabel  *m_piSTColorLayerHorzResolution;
	IGameGUIButton *m_piBTColorLayerDecreaseHorzResolution;
	IGameGUIButton *m_piBTColorLayerIncreaseHorzResolution;
	IGameGUILabel  *m_piSTColorLayerVertResolution;
	IGameGUIButton *m_piBTColorLayerDecreaseVertResolution;
	IGameGUIButton *m_piBTColorLayerIncreaseVertResolution;
	IGameGUIButton *m_piBTColorLayerRemove;
	IGameGUIButton *m_piBTColorLayerMoveUp;
	IGameGUIButton *m_piBTColorLayerMoveDown;

	// Entity Layer

	IGameGUIButton *m_piBTNewEntityLayer;
	IGameGUILabel  *m_piSTEntityLayerName;
	IGameGUIButton *m_piBTEntityLayerSample;
	IScenarioEditorObjectLabel *m_piSTEntityLayerObjectLabel;
	IGameGUIButton *m_piBTEntityLayerColorSample;
	IGameGUIButton *m_piBTEntityLayerRemove;
	IGameGUILabel  *m_piSTEntityLayerSeparation;
	IGameGUIButton *m_piBTEntityLayerDecreaseSeparation;
	IGameGUIButton *m_piBTEntityLayerIncreaseSeparation;
	IGameGUILabel  *m_piSTEntityLayerPositionJitter;
	IGameGUIButton *m_piBTEntityLayerDecreasePositionJitter;
	IGameGUIButton *m_piBTEntityLayerIncreasePositionJitter;
	IGameGUILabel  *m_piSTEntityLayerYawJitter;
	IGameGUIButton *m_piBTEntityLayerDecreaseYawJitter;
	IGameGUIButton *m_piBTEntityLayerIncreaseYawJitter;

	// Entity

	IGameGUIButton *m_piBTNewEntity;
	IGameGUILabel  *m_piSTEntityName;
	IScenarioEditorObjectLabel *m_piSTEntityObjectLabel;
	IGameGUIButton *m_piBTEntitySample;
	IGameGUIButton *m_piBTEntityRemove;
	IGameGUILabel  *m_piSTEntityYaw;
	IGameGUIButton *m_piBTEntityDecreaseYaw;
	IGameGUIButton *m_piBTEntityIncreaseYaw;
	IGameGUIButton *m_piBTEntityConsiderAsTerrain;
	IGameGUIButton *m_piBTEntityLocked;

	// Formation

	IGameGUIButton *m_piBTNewFormation;
	IGameGUILabel  *m_piSTFormationName;
	IGameGUIButton *m_piBTFormationSample;
	IGameGUIButton *m_piBTFormationBonusSample;
	IScenarioEditorObjectLabel *m_piSTFormationObjectLabel;
	IScenarioEditorObjectLabel *m_piSTFormationBonusObjectLabel;
	IGameGUIButton *m_piBTFormationRemove;

	// Entity Layers

	IGameWindow *m_piGREntityLayerList;

	// Options

	IGameGUIButton *m_piBTOptionsTextures;
	IGameGUIButton *m_piBTOptionsSolid;
	IGameGUIButton *m_piBTOptionsBlend;
	IGameGUIButton *m_piBTOptionsLayerOnly;
	IGameGUIButton *m_piBTOptionsModelOriginal;
	IGameGUIButton *m_piBTOptionsModelGenerated;

	// File
	IGameWindow	   *m_piGRFile;
	IGameGUIButton *m_piBTFileNew;
	IGameGUIButton *m_piBTFileOpen;
	IGameGUIButton *m_piBTFileSave;
	IGameGUIButton *m_piBTFileSaveAs;
	IGameGUIButton *m_piBTFileCompile;
	IGameGUIButton *m_piBTFileExit;

	// General
	IGameGUILabel  *m_piSTGeneralModelName;
	IGameGUILabel  *m_piSTGeneralColorMapName;
	IGameGUIButton *m_piBTGeneralChangeModel;
	IGameGUIButton *m_piBTGeneralChangeColorMap;

	// Water 

	IGameGUILabel  *m_piSTWaterName;
	IGameGUILabel  *m_piSTWaterSecondName;
	IGameGUIButton *m_piBTWaterSample;
	IGameGUIButton *m_piBTWaterSecondSample;
	IGameGUILabel  *m_piSTWaterHeight;
	IGameGUIButton *m_piBTWaterDecreaseHeight;
	IGameGUIButton *m_piBTWaterIncreaseHeight;
	IGameGUILabel  *m_piSTWaterOpacity;
	IGameGUIButton *m_piBTWaterDecreaseOpacity;
	IGameGUIButton *m_piBTWaterIncreaseOpacity;
	IGameGUILabel  *m_piSTWaterSpeed;
	IGameGUIButton *m_piBTWaterDecreaseSpeed;
	IGameGUIButton *m_piBTWaterIncreaseSpeed;
	IGameGUILabel  *m_piSTWaterHorzResolution;
	IGameGUIButton *m_piBTWaterDecreaseHorzResolution;
	IGameGUIButton *m_piBTWaterIncreaseHorzResolution;
	IGameGUILabel  *m_piSTWaterVertResolution;
	IGameGUIButton *m_piBTWaterDecreaseVertResolution;
	IGameGUIButton *m_piBTWaterIncreaseVertResolution;
	IGameGUIButton *m_piBTWaterEnable;


	// Fog 

	IGameGUILabel  *m_piSTFogStart;
	IGameGUIButton *m_piBTFogDecreaseStart;
	IGameGUIButton *m_piBTFogIncreaseStart;
	IGameGUILabel  *m_piSTFogEnd;
	IGameGUIButton *m_piBTFogDecreaseEnd;
	IGameGUIButton *m_piBTFogIncreaseEnd;
	IGameGUIButton *m_piBTFogColorSample;
	IGameGUIButton *m_piBTFogEnable;
	IGameGUIButton *m_piBTFogFitWater;
	IGameGUIButton *m_piBTFogFitSky;

	// Sun 

	IGameGUILabel  *m_piSTSunDistance;
	IGameGUIButton *m_piBTSunDecreaseDistance;
	IGameGUIButton *m_piBTSunIncreaseDistance;
	IGameGUILabel  *m_piSTSunElevation;
	IGameGUIButton *m_piBTSunDecreaseElevation;
	IGameGUIButton *m_piBTSunIncreaseElevation;
	IGameGUILabel  *m_piSTSunAzimuth;
	IGameGUIButton *m_piBTSunDecreaseAzimuth;
	IGameGUIButton *m_piBTSunIncreaseAzimuth;
	IGameGUIButton *m_piBTSunColorSample;
	IGameGUIButton *m_piBTAmbientColorSample;

	// Sky 

	IGameGUILabel  *m_piSTSkyName;
	IGameGUIButton *m_piBTSkySample;
	IGameGUILabel  *m_piSTSkyOpacity;
	IGameGUIButton *m_piBTSkyDecreaseOpacity;
	IGameGUIButton *m_piBTSkyIncreaseOpacity;
	IGameGUILabel  *m_piSTSkySpeed;
	IGameGUIButton *m_piBTSkyDecreaseSpeed;
	IGameGUIButton *m_piBTSkyIncreaseSpeed;
	IGameGUILabel  *m_piSTSkyHorzResolution;
	IGameGUIButton *m_piBTSkyDecreaseHorzResolution;
	IGameGUIButton *m_piBTSkyIncreaseHorzResolution;
	IGameGUILabel  *m_piSTSkyVertResolution;
	IGameGUIButton *m_piBTSkyDecreaseVertResolution;
	IGameGUIButton *m_piBTSkyIncreaseVertResolution;
	IGameGUIButton *m_piBTSkyEnable;

	// Play Area

	IGameGUILabel  *m_piSTPlayAreaCameraDistance;
	IGameGUIButton *m_piBTPlayAreaDecreaseCameraDistance;
	IGameGUIButton *m_piBTPlayAreaIncreaseCameraDistance;
	IGameGUILabel  *m_piSTPlayAreaCameraSpeed;
	IGameGUIButton *m_piBTPlayAreaDecreaseCameraSpeed;
	IGameGUIButton *m_piBTPlayAreaIncreaseCameraSpeed;
	IGameGUILabel  *m_piSTPlayAreaCameraViewAngle;
	IGameGUIButton *m_piBTPlayAreaDecreaseCameraViewAngle;
	IGameGUIButton *m_piBTPlayAreaIncreaseCameraViewAngle;
	IGameGUILabel  *m_piSTPlayAreaCameraAspectRatio;
	IGameGUIButton *m_piBTPlayAreaDecreaseCameraAspectRatio;
	IGameGUIButton *m_piBTPlayAreaIncreaseCameraAspectRatio;
	IGameGUILabel  *m_piSTPlayAreaAirPlane;
	IGameGUIButton *m_piBTPlayAreaDecreaseAirPlane;
	IGameGUIButton *m_piBTPlayAreaIncreaseAirPlane;
	IGameGUILabel  *m_piSTPlayAreaScroll;
	IGameGUIButton *m_piBTPlayAreaDecreaseScroll;
	IGameGUIButton *m_piBTPlayAreaIncreaseScroll;
	IGameGUIButton *m_piBTPlayAreaEnable;

	ISystem					*m_piGameSystem;
	CGameControllerWrapper   m_GameControllerWrapper;
	CPlayAreaManagerWrapper  m_PlayAreaManagerWrapper;
	CGameRenderWrapper		 m_GameRenderWrapper;
	CWorldManagerWrapper	 m_WorldManagerWrapper;

	void ProcessInput(double dTimeFraction,double dRealTimeFraction);
	void ProcessKey(SHORT nKey,double dTimeFraction,double dRealTimeFraction);

	void ProcessFileNew();
	void ProcessFileOpen();

	void OpenScenario( std::string sScenario );
	void ProcessFileSave();
	void ProcessFileSaveAs();
	void ProcessFileExit();

	void Reset();

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("FPS",m_piSTFps);
		CHILD_MAP_ENTRY("VertexOverHead",m_piSTVertexOverhead);
		CHILD_MAP_ENTRY("FaceOverHead",m_piSTFaceOverhead);
		CHILD_MAP_ENTRY("Options",m_piBTShowOptionsPanel);
		CHILD_MAP_ENTRY("Terrain",m_piBTShowTerrainPanel);
		CHILD_MAP_ENTRY("Entities",m_piBTShowEntitiesPanel);
		CHILD_MAP_ENTRY("Formations",m_piBTShowFormationsPanel);
		CHILD_MAP_ENTRY("File",m_piBTShowFilePanel);

		CHILD_MAP_ENTRY("EntitiesPanel",m_piGREntitiesPanel);
		CHILD_MAP_ENTRY("EntityLayerList",m_piGREntityLayerList);
		CHILD_MAP_ENTRY("NewEntityLayer",m_piBTNewEntityLayer);
		CHILD_MAP_ENTRY("NewEntity",m_piBTNewEntity);

		CHILD_MAP_ENTRY("FormationsPanel",m_piGRFormationsPanel);
		CHILD_MAP_ENTRY("NewFormation",m_piBTNewFormation);

		CHILD_MAP_ENTRY("TerrainPanel",m_piGRTerrainPanel);
		CHILD_MAP_ENTRY("GeneralProperties",m_piBTShowGeneralProperties);
		CHILD_MAP_ENTRY("WaterProperties",m_piBTShowWaterProperties);
		CHILD_MAP_ENTRY("FogProperties",m_piBTShowFogProperties);
		CHILD_MAP_ENTRY("SunProperties",m_piBTShowSunProperties);
		CHILD_MAP_ENTRY("SkyProperties",m_piBTShowSkyProperties);
		CHILD_MAP_ENTRY("PlayAreaProperties",m_piBTShowPlayAreaProperties);
		CHILD_MAP_ENTRY("LayerList",m_piGRLayerList);
		CHILD_MAP_ENTRY("NewHeightLayer",m_piBTNewHeightLayer);
		CHILD_MAP_ENTRY("NewColorLayer",m_piBTNewColorLayer);
		
		CHILD_MAP_ENTRY("HeightLayerPanel",m_piGRHeightLayerPanel);
		CHILD_MAP_ENTRY("HeightLayerTextureName",m_piSTHeightLayerName);
		CHILD_MAP_ENTRY("HeightLayerTextureSample",m_piBTHeightLayerSample);
		CHILD_MAP_ENTRY("HeightLayerMaxHeight",m_piSTHeightLayerMaxHeight);
		CHILD_MAP_ENTRY("HeightLayerDecreaseMaxHeight",m_piBTHeightLayerDecreaseMaxHeight);
		CHILD_MAP_ENTRY("HeightLayerIncreaseMaxHeight",m_piBTHeightLayerIncreaseMaxHeight);
		CHILD_MAP_ENTRY("HeightLayerMinHeight",m_piSTHeightLayerMinHeight);
		CHILD_MAP_ENTRY("HeightLayerDecreaseMinHeight",m_piBTHeightLayerDecreaseMinHeight);
		CHILD_MAP_ENTRY("HeightLayerIncreaseMinHeight",m_piBTHeightLayerIncreaseMinHeight);
		CHILD_MAP_ENTRY("HeightLayerMargin",m_piSTHeightLayerMargin);
		CHILD_MAP_ENTRY("HeightLayerDecreaseMargin",m_piBTHeightLayerDecreaseMargin);
		CHILD_MAP_ENTRY("HeightLayerIncreaseMargin",m_piBTHeightLayerIncreaseMargin);
		CHILD_MAP_ENTRY("HeightLayerTextureHorzResolution",m_piSTHeightLayerHorzResolution);
		CHILD_MAP_ENTRY("HeightLayerDecreaseHorzResolution",m_piBTHeightLayerDecreaseHorzResolution);
		CHILD_MAP_ENTRY("HeightLayerIncreaseHorzResolution",m_piBTHeightLayerIncreaseHorzResolution);
		CHILD_MAP_ENTRY("HeightLayerTextureVertResolution",m_piSTHeightLayerVertResolution);
		CHILD_MAP_ENTRY("HeightLayerDecreaseVertResolution",m_piBTHeightLayerDecreaseVertResolution);
		CHILD_MAP_ENTRY("HeightLayerIncreaseVertResolution",m_piBTHeightLayerIncreaseVertResolution);
		CHILD_MAP_ENTRY("HeightLayerRemove",m_piBTHeightLayerRemove);
		CHILD_MAP_ENTRY("HeightLayerMoveUp",m_piBTHeightLayerMoveUp);
		CHILD_MAP_ENTRY("HeightLayerMoveDown",m_piBTHeightLayerMoveDown);
	
		CHILD_MAP_ENTRY("ColorLayerPanel",m_piGRColorLayerPanel);
		CHILD_MAP_ENTRY("ColorLayerTextureName",m_piSTColorLayerName);
		CHILD_MAP_ENTRY("ColorLayerTextureSample",m_piBTColorLayerSample);
		CHILD_MAP_ENTRY("ColorLayerColorSample",m_piBTColorLayerColorSample);
		CHILD_MAP_ENTRY("ColorLayerTextureHorzResolution",m_piSTColorLayerHorzResolution);
		CHILD_MAP_ENTRY("ColorLayerDecreaseHorzResolution",m_piBTColorLayerDecreaseHorzResolution);
		CHILD_MAP_ENTRY("ColorLayerIncreaseHorzResolution",m_piBTColorLayerIncreaseHorzResolution);
		CHILD_MAP_ENTRY("ColorLayerTextureVertResolution",m_piSTColorLayerVertResolution);
		CHILD_MAP_ENTRY("ColorLayerDecreaseVertResolution",m_piBTColorLayerDecreaseVertResolution);
		CHILD_MAP_ENTRY("ColorLayerIncreaseVertResolution",m_piBTColorLayerIncreaseVertResolution);
		CHILD_MAP_ENTRY("ColorLayerRemove",m_piBTColorLayerRemove);
		CHILD_MAP_ENTRY("ColorLayerMoveUp",m_piBTColorLayerMoveUp);
		CHILD_MAP_ENTRY("ColorLayerMoveDown",m_piBTColorLayerMoveDown);

		CHILD_MAP_ENTRY("EntityLayerPanel",m_piGREntityLayerPanel);
		CHILD_MAP_ENTRY("EntityLayerModelName",m_piSTEntityLayerName);
		CHILD_MAP_ENTRY("EntityLayerModelSample",m_piBTEntityLayerSample);
		CHILD_MAP_ENTRY("EntityLayerObjectLabel",m_piSTEntityLayerObjectLabel);
		CHILD_MAP_ENTRY("EntityLayerColorSample",m_piBTEntityLayerColorSample);
		CHILD_MAP_ENTRY("EntityLayerRemove",m_piBTEntityLayerRemove);
		CHILD_MAP_ENTRY("EntityLayerSeparation",m_piSTEntityLayerSeparation);
		CHILD_MAP_ENTRY("EntityLayerDecreaseSeparation",m_piBTEntityLayerDecreaseSeparation);
		CHILD_MAP_ENTRY("EntityLayerIncreaseSeparation",m_piBTEntityLayerIncreaseSeparation);
		CHILD_MAP_ENTRY("EntityLayerPositionJitter",m_piSTEntityLayerPositionJitter);
		CHILD_MAP_ENTRY("EntityLayerDecreasePositionJitter",m_piBTEntityLayerDecreasePositionJitter);
		CHILD_MAP_ENTRY("EntityLayerIncreasePositionJitter",m_piBTEntityLayerIncreasePositionJitter);
		CHILD_MAP_ENTRY("EntityLayerYawJitter",m_piSTEntityLayerYawJitter);
		CHILD_MAP_ENTRY("EntityLayerDecreaseYawJitter",m_piBTEntityLayerDecreaseYawJitter);
		CHILD_MAP_ENTRY("EntityLayerIncreaseYawJitter",m_piBTEntityLayerIncreaseYawJitter);
		

		CHILD_MAP_ENTRY("EntityPanel",m_piGREntityPanel);
		CHILD_MAP_ENTRY("EntityName",m_piSTEntityName);
		CHILD_MAP_ENTRY("EntityObjectLabel",m_piSTEntityObjectLabel);
		CHILD_MAP_ENTRY("EntitySample",m_piBTEntitySample);
		CHILD_MAP_ENTRY("EntityRemove",m_piBTEntityRemove);
		CHILD_MAP_ENTRY("EntityYaw",m_piSTEntityYaw);
		CHILD_MAP_ENTRY("EntityDecreaseYaw",m_piBTEntityDecreaseYaw);
		CHILD_MAP_ENTRY("EntityIncreaseYaw",m_piBTEntityIncreaseYaw);
		CHILD_MAP_ENTRY("EntityGroundConsiderAsTerrain",m_piBTEntityConsiderAsTerrain);
		CHILD_MAP_ENTRY("EntityLocked",m_piBTEntityLocked);

		CHILD_MAP_ENTRY("FormationPanel",m_piGRFormationPanel);
		CHILD_MAP_ENTRY("FormationName",m_piSTFormationName);
		CHILD_MAP_ENTRY("FormationSample",m_piBTFormationSample);
		CHILD_MAP_ENTRY("FormationObjectLabel",m_piSTFormationObjectLabel);
		CHILD_MAP_ENTRY("FormationBonusSample",m_piBTFormationBonusSample);
		CHILD_MAP_ENTRY("FormationBonusObjectLabel",m_piSTFormationBonusObjectLabel);
		CHILD_MAP_ENTRY("FormationRemove",m_piBTFormationRemove);

		CHILD_MAP_ENTRY("OptionsPanel",m_piGROptionsPanel);
		CHILD_MAP_ENTRY("OptionShowTextures",m_piBTOptionsTextures);
		CHILD_MAP_ENTRY("OptionSolid",m_piBTOptionsSolid);
		CHILD_MAP_ENTRY("OptionBlend",m_piBTOptionsBlend);
		CHILD_MAP_ENTRY("OptionSelectedLayerOnly",m_piBTOptionsLayerOnly);
		CHILD_MAP_ENTRY("OptionOriginalModel",m_piBTOptionsModelOriginal);
		CHILD_MAP_ENTRY("OptionGeneratedModel",m_piBTOptionsModelGenerated);

		CHILD_MAP_ENTRY("FilePanel",m_piGRFile);
		CHILD_MAP_ENTRY("FileNew",m_piBTFileNew);
		CHILD_MAP_ENTRY("FileOpen",m_piBTFileOpen);
		CHILD_MAP_ENTRY("FileSave",m_piBTFileSave);
		CHILD_MAP_ENTRY("FileSaveAs",m_piBTFileSaveAs);
		CHILD_MAP_ENTRY("FileCompile",m_piBTFileCompile);
		CHILD_MAP_ENTRY("FileExit",m_piBTFileExit);

		CHILD_MAP_ENTRY("GeneralPanel",m_piGRGeneralPanel);
		CHILD_MAP_ENTRY("GeneralChangeModel",m_piBTGeneralChangeModel);
		CHILD_MAP_ENTRY("GeneralChangeColorMap",m_piBTGeneralChangeColorMap);
		CHILD_MAP_ENTRY("GeneralModelName",m_piSTGeneralModelName);
		CHILD_MAP_ENTRY("GeneralColorMapName",m_piSTGeneralColorMapName);

		CHILD_MAP_ENTRY("WaterPanel",m_piGRWaterPanel);
		CHILD_MAP_ENTRY("WaterTextureName",m_piSTWaterName);
		CHILD_MAP_ENTRY("WaterSecondTextureName",m_piSTWaterSecondName);
		CHILD_MAP_ENTRY("WaterTextureSample",m_piBTWaterSample);
		CHILD_MAP_ENTRY("WaterSecondTextureSample",m_piBTWaterSecondSample);
		CHILD_MAP_ENTRY("WaterHeight",m_piSTWaterHeight);
		CHILD_MAP_ENTRY("WaterDecreaseHeight",m_piBTWaterDecreaseHeight);
		CHILD_MAP_ENTRY("WaterIncreaseHeight",m_piBTWaterIncreaseHeight);
		CHILD_MAP_ENTRY("WaterOpacity",m_piSTWaterOpacity);
		CHILD_MAP_ENTRY("WaterDecreaseOpacity",m_piBTWaterDecreaseOpacity);
		CHILD_MAP_ENTRY("WaterIncreaseOpacity",m_piBTWaterIncreaseOpacity);
		CHILD_MAP_ENTRY("WaterSpeed",m_piSTWaterSpeed);
		CHILD_MAP_ENTRY("WaterDecreaseSpeed",m_piBTWaterDecreaseSpeed);
		CHILD_MAP_ENTRY("WaterIncreaseSpeed",m_piBTWaterIncreaseSpeed);
		CHILD_MAP_ENTRY("WaterTextureHorzResolution",m_piSTWaterHorzResolution);
		CHILD_MAP_ENTRY("WaterDecreaseHorzResolution",m_piBTWaterDecreaseHorzResolution);
		CHILD_MAP_ENTRY("WaterIncreaseHorzResolution",m_piBTWaterIncreaseHorzResolution);
		CHILD_MAP_ENTRY("WaterTextureVertResolution",m_piSTWaterVertResolution);
		CHILD_MAP_ENTRY("WaterDecreaseVertResolution",m_piBTWaterDecreaseVertResolution);
		CHILD_MAP_ENTRY("WaterIncreaseVertResolution",m_piBTWaterIncreaseVertResolution);
		CHILD_MAP_ENTRY("WaterEnable",m_piBTWaterEnable);


		CHILD_MAP_ENTRY("FogPanel",m_piGRFogPanel);
		CHILD_MAP_ENTRY("FogStart",m_piSTFogStart);
		CHILD_MAP_ENTRY("FogDecreaseStart",m_piBTFogDecreaseStart);
		CHILD_MAP_ENTRY("FogIncreaseStart",m_piBTFogIncreaseStart);
		CHILD_MAP_ENTRY("FogEnd",m_piSTFogEnd);
		CHILD_MAP_ENTRY("FogDecreaseEnd",m_piBTFogDecreaseEnd);
		CHILD_MAP_ENTRY("FogIncreaseEnd",m_piBTFogIncreaseEnd);
		CHILD_MAP_ENTRY("FogColorSample",m_piBTFogColorSample);
		CHILD_MAP_ENTRY("FogEnable",m_piBTFogEnable);
		CHILD_MAP_ENTRY("FogFitWater",m_piBTFogFitWater);
		CHILD_MAP_ENTRY("FogFitSky",m_piBTFogFitSky);

		CHILD_MAP_ENTRY("SunPanel",m_piGRSunPanel);
		CHILD_MAP_ENTRY("SunDistance",m_piSTSunDistance);
		CHILD_MAP_ENTRY("SunDecreaseDistance",m_piBTSunDecreaseDistance);
		CHILD_MAP_ENTRY("SunIncreaseDistance",m_piBTSunIncreaseDistance);
		CHILD_MAP_ENTRY("SunElevation",m_piSTSunElevation);
		CHILD_MAP_ENTRY("SunDecreaseElevation",m_piBTSunDecreaseElevation);
		CHILD_MAP_ENTRY("SunIncreaseElevation",m_piBTSunIncreaseElevation);
		CHILD_MAP_ENTRY("SunAzimuth",m_piSTSunAzimuth);
		CHILD_MAP_ENTRY("SunDecreaseAzimuth",m_piBTSunDecreaseAzimuth);
		CHILD_MAP_ENTRY("SunIncreaseAzimuth",m_piBTSunIncreaseAzimuth);
		CHILD_MAP_ENTRY("SunColorSample",m_piBTSunColorSample);
		CHILD_MAP_ENTRY("AmbientColorSample",m_piBTAmbientColorSample);

		CHILD_MAP_ENTRY("SkyPanel",m_piGRSkyPanel);
		CHILD_MAP_ENTRY("SkyTextureName",m_piSTSkyName);
		CHILD_MAP_ENTRY("SkyTextureSample",m_piBTSkySample);
		CHILD_MAP_ENTRY("SkyOpacity",m_piSTSkyOpacity);
		CHILD_MAP_ENTRY("SkyDecreaseOpacity",m_piBTSkyDecreaseOpacity);
		CHILD_MAP_ENTRY("SkyIncreaseOpacity",m_piBTSkyIncreaseOpacity);
		CHILD_MAP_ENTRY("SkySpeed",m_piSTSkySpeed);
		CHILD_MAP_ENTRY("SkyDecreaseSpeed",m_piBTSkyDecreaseSpeed);
		CHILD_MAP_ENTRY("SkyIncreaseSpeed",m_piBTSkyIncreaseSpeed);
		CHILD_MAP_ENTRY("SkyTextureHorzResolution",m_piSTSkyHorzResolution);
		CHILD_MAP_ENTRY("SkyDecreaseHorzResolution",m_piBTSkyDecreaseHorzResolution);
		CHILD_MAP_ENTRY("SkyIncreaseHorzResolution",m_piBTSkyIncreaseHorzResolution);
		CHILD_MAP_ENTRY("SkyTextureVertResolution",m_piSTSkyVertResolution);
		CHILD_MAP_ENTRY("SkyDecreaseVertResolution",m_piBTSkyDecreaseVertResolution);
		CHILD_MAP_ENTRY("SkyIncreaseVertResolution",m_piBTSkyIncreaseVertResolution);
		CHILD_MAP_ENTRY("SkyEnable",m_piBTSkyEnable);

		CHILD_MAP_ENTRY("PlayAreaPanel",m_piGRPlayAreaPanel);
		CHILD_MAP_ENTRY("PlayAreaCameraDistance",m_piSTPlayAreaCameraDistance);
		CHILD_MAP_ENTRY("PlayAreaDecreaseCameraDistance",m_piBTPlayAreaDecreaseCameraDistance);
		CHILD_MAP_ENTRY("PlayAreaIncreaseCameraDistance",m_piBTPlayAreaIncreaseCameraDistance);
		CHILD_MAP_ENTRY("PlayAreaCameraSpeed",m_piSTPlayAreaCameraSpeed);
		CHILD_MAP_ENTRY("PlayAreaDecreaseCameraSpeed",m_piBTPlayAreaDecreaseCameraSpeed);
		CHILD_MAP_ENTRY("PlayAreaIncreaseCameraSpeed",m_piBTPlayAreaIncreaseCameraSpeed);
		CHILD_MAP_ENTRY("PlayAreaCameraViewAngle",m_piSTPlayAreaCameraViewAngle);
		CHILD_MAP_ENTRY("PlayAreaDecreaseCameraViewAngle",m_piBTPlayAreaDecreaseCameraViewAngle);
		CHILD_MAP_ENTRY("PlayAreaIncreaseCameraViewAngle",m_piBTPlayAreaIncreaseCameraViewAngle);
		CHILD_MAP_ENTRY("PlayAreaCameraAspectRatio",m_piSTPlayAreaCameraAspectRatio);
		CHILD_MAP_ENTRY("PlayAreaDecreaseCameraAspectRatio",m_piBTPlayAreaDecreaseCameraAspectRatio);
		CHILD_MAP_ENTRY("PlayAreaIncreaseCameraAspectRatio",m_piBTPlayAreaIncreaseCameraAspectRatio);
		CHILD_MAP_ENTRY("PlayAreaAirPlane",m_piSTPlayAreaAirPlane);
		CHILD_MAP_ENTRY("PlayAreaDecreaseAirPlane",m_piBTPlayAreaDecreaseAirPlane);
		CHILD_MAP_ENTRY("PlayAreaIncreaseAirPlane",m_piBTPlayAreaIncreaseAirPlane);
		CHILD_MAP_ENTRY("PlayAreaScroll",m_piSTPlayAreaScroll);
		CHILD_MAP_ENTRY("PlayAreaDecreaseScroll",m_piBTPlayAreaDecreaseScroll);
		CHILD_MAP_ENTRY("PlayAreaIncreaseScroll",m_piBTPlayAreaIncreaseScroll);
	
	END_CHILD_MAP()

public:

	CScenarioEditorColorSelectorWrapper m_ColorSelector;
	CScenarioEditorObjectSelectorWrapper m_ObjectSelector;

	bool m_bSimulationStarted;
	bool m_bInspectionMode;
	// Sobrecarga para cambiar el valor por defecto del sistema de referencia.

	BEGIN_PROP_MAP(CScenarioEditorMainWindow)
		PROP_CLASS_CHAIN(CGameWindowBase);
		PROP_VALUE_FLAGS(m_eReferenceSystem,"ReferenceSystem",eGameGUIReferenceSystem_Relative,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bAutoGenerateBSP,"GenerateBSP",1,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bAutoUpdateBSP,"UpdateBSP",0,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP()

	bool Unserialize(ISystemPersistencyNode *piNode);
	void DestroyWindow();

	void OnDraw(IGenericRender *piRender);
	void OnButtonClicked(IGameGUIButton *piControl);
	void OnKeyDown(int nKey,bool *pbProcessed);
	void OnMouseDown(int nButton,double x,double y);
	void OnMouseMove(double x,double y);
	void OnMouseUp(int nButton,double x,double y);

	void UpdateLayerPanel();
	void UpdateTexturization();
	void UpdateCaption();

	void CenterCamera();

	bool GetHeightAt(CVector vPoint,bool bIgnoreTerrainObjects, double *pdHeight);

	void SetupRenderOptions(IGenericRender *piRender);

	bool GetTerrainCoordinatesFromCursorPos(double x,double y,bool bIgnoreTerrainObjects, CVector *pTerrainPos);
	bool GetAirPlaneCoordinatesFromCursorPos(double x,double y,CVector *pAirPlanePos);

	double GetAirPlaneAbsoluteHeight();

	void StopGameSimulation();
	void StartGameSimulation();
	void UpdateColorLayerControls();
	void UpdateHeightLayerControls();
	void UpdateEntityLayerControls();
	void UpdateEntityControls();
	void UpdateFormationControls();
	void UpdateFormationsHeight();

	CScenarioEditorMainWindow(void);
	~CScenarioEditorMainWindow(void);
};


