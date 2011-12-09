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


#pragma once

#include "ScenarioEditor.h"
#include "ScenarioEditorColorSelector.h"
#include "ScenarioEditorObjectSelector.h"
#include "ScenarioEditorObjectListSelector.h"

DECLARE_CUSTOM_WRAPPER1(CScenarioEditorColorSelectorWrapper,IScenarioEditorColorSelector,m_piColorSelector)
DECLARE_CUSTOM_WRAPPER1(CScenarioEditorObjectSelectorWrapper,IScenarioEditorObjectSelector,m_piObjectSelector)
DECLARE_CUSTOM_WRAPPER1(CScenarioEditorObjectListSelectorWrapper,IScenarioEditorObjectListSelector,m_piObjectListSelector)

#define KEY_FORWARD		1
#define KEY_BACK		2
#define KEY_RIGHT		3
#define KEY_LEFT		4
#define KEY_UP			5
#define KEY_DOWN		6
#define KEY_PAUSE		7
#define KEY_PROCESS_ONE_FRAME		8

enum EColorCallBack
{
	eColorCallBackSun,
	eColorCallBackAmbient,
	eColorCallBackFog
};

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
	unsigned long					m_nPlayAreaElementId;
	IPlayAreaEntity					*m_piPlayAreaEntity;
	IEntityType						*m_piEntityType;
	IDesignObject					*m_piDesignObject;
	IDesignObject					*m_piBonusDesignObject;
	ISystemObject					*m_piObject;
	
	SEntityControls(){m_piBonusDesignObject=NULL;m_piPlayAreaEntity=NULL;m_piEntityType=NULL;m_piDesignObject=NULL;m_piObject=NULL;m_nPlayAreaElementId=0;}
	~SEntityControls(){REL(m_piBonusDesignObject);REL(m_piPlayAreaEntity);REL(m_piEntityType);REL(m_piDesignObject);REL(m_piObject);}
};

struct SCheckPoint
{
	unsigned long					m_nPlayAreaElementId;
	IPlayAreaCheckPoint				*m_piPlayAreaCheckPoint;
	
	SCheckPoint(){m_piPlayAreaCheckPoint=NULL;m_nPlayAreaElementId=0;}
	~SCheckPoint(){REL(m_piPlayAreaCheckPoint);}
};

struct SFormationControls
{
	unsigned long					m_nPlayAreaElementId;
	IPlayAreaFormation				*m_piPlayAreaFormation;
	IFormationType					*m_piFormationType;
	IDesignObject					*m_piDesignObject;
	IDesignObject					*m_piAlternativeDesignObject;
	ISystemObject					*m_piObject;
	IDesignObject					*m_piBonusDesignObject;
	IDesignObject					*m_piAlternativeBonusDesignObject;

	SFormationControls(){m_piAlternativeDesignObject=NULL;m_piAlternativeBonusDesignObject=NULL;m_piPlayAreaFormation=NULL;m_piFormationType=NULL;m_piDesignObject=NULL;m_piObject=NULL;m_piBonusDesignObject=NULL;m_nPlayAreaElementId=0;}
	~SFormationControls(){REL(m_piAlternativeDesignObject);REL(m_piAlternativeBonusDesignObject);REL(m_piPlayAreaFormation);REL(m_piFormationType);REL(m_piDesignObject);REL(m_piObject);REL(m_piBonusDesignObject);}
};

class CScenarioEditorMainWindow: virtual public CGameWindowBase, virtual public IGameGUIButtonEvents,virtual public IGameGUIListEvents,virtual public IGameGUIColorDialogCallback
{
public:
	CConfigFile				m_GUIConfigFile;
	CViewportWrapper		m_Viewport;
	CRenderWrapper			m_Render;
	CGenericCameraWrapper	m_Camera;
	CFrameManagerWrapper	m_FrameManager;
	CPlayerProfileWrapper	m_PlayerProfile;

	std::vector<SHeightLayerControls *> m_vHeightLayerControls;
	std::vector<SColorLayerControls *>	m_vColorLayerControls;
	std::vector<SEntityLayerControls *> m_vEntityLayerControls;
	std::vector<SEntityControls *>		m_vEntityControls;
	std::vector<SFormationControls *>	m_vFormationControls;
	std::vector<SCheckPoint *>			m_vCheckPoints;
	std::string							m_sFile;

	unsigned long		m_dwNexControlKey;
	double				m_d3DFontSize;
	bool				m_bBlend;
	bool				m_bSolid;
	bool				m_bTextures;
	bool				m_bFog;
	bool				m_bShadows;
	bool				m_bLighting;
	bool				m_bNormalMaps;
	bool				m_bSkyShadow;
	bool				m_bStats;
	EShadingModel 		m_eShadingModel;
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
	bool				m_bRenderPlayArea;
	bool				m_bMovingCameraPosition;
	CVector 			m_vPlayMovementPosition;
	bool				m_bGodMode;
	bool				m_bUseAmmo;
	double				m_dDifficulty;
	
	unsigned int        m_nLastStatsTime;

	int					m_nSelectedRoutePoint;
	int					m_nSelectedLandingRoutePoint;
	int					m_nSelectedTakeOffRoutePoint;
	
	bool				m_bMovingObject;
	bool				m_bMovingRoutePoint;
	bool				m_bMovingPlayerRoutePoint;
	bool				m_bMovingCheckPoint;
	CVector				m_vObjectOriginalPosition;
	CVector				m_vCursorOriginalPosition;

	int					m_nSelectedHeightLayer;
	int					m_nSelectedColorLayer;
	int					m_nSelectedEntityLayer;
	int					m_nSelectedEntity;
	int					m_nSelectedFormation;
	int					m_nSelectedCheckPoint;
	
	bool				m_bAutoGenerateBSP;
	bool				m_bAutoUpdateBSP;

	bool				m_bPauseOnNextFrame;
	
	double 				m_dMouseTraceDistance;
	
	unsigned int		m_nStartingWeapon;
	
	EColorCallBack		m_eColorCallBack;
	
	IGameGUILabel *m_piSTFps;
	IGameGUILabel *m_piSTGameTime;
	
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
	IGameWindow	  *m_piGRRoutePointPanel;
	
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
	IGameGUIButton *m_piBTHeightLayerNormalMapSample;
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
	IGameGUIButton *m_piBTColorLayerNormalMapSample;
	IGameGUIButton *m_piBTColorLayerColorSample;
	IGameGUIButton *m_piBTColorLayerAbruptEnd;
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
	IScenarioEditorObjectLabel *m_piSTEntityBonusObjectLabel;
	IGameGUIButton *m_piBTEntityBonusSample;
	IGameGUIButton *m_piBTEntityRemove;
	IGameGUILabel  *m_piSTEntityYaw;
	IGameGUIButton *m_piBTEntityDecreaseYaw;
	IGameGUIButton *m_piBTEntityIncreaseYaw;
	IGameGUIButton *m_piBTEntityClearRoute;
	IGameGUILabel  *m_piSTEntityCount;
	IGameGUIButton *m_piBTEntityDecreaseCount;
	IGameGUIButton *m_piBTEntityIncreaseCount;
	IGameGUILabel  *m_piSTEntityDelay;
	IGameGUIButton *m_piBTEntityDecreaseDelay;
	IGameGUIButton *m_piBTEntityIncreaseDelay;
	IGameGUILabel  *m_piSTEntityBonusOnChild;
	IGameGUIButton *m_piBTEntityDecreaseBonusOnChild;
	IGameGUIButton *m_piBTEntityIncreaseBonusOnChild;
	IGameGUILabel  *m_piSTEntityRouteDelay;
	IGameGUIButton *m_piBTEntityDecreaseRouteDelay;
	IGameGUIButton *m_piBTEntityIncreaseRouteDelay;
	IGameGUILabel  *m_piSTEntityInterval;
	IGameGUIButton *m_piBTEntityDecreaseInterval;
	IGameGUIButton *m_piBTEntityIncreaseInterval;
	
	// Formation

	IGameGUIButton *m_piBTNewFormation;
	IGameGUIButton *m_piBTFormationSample;
	IGameGUIButton *m_piBTFormationBonusSample;
	IScenarioEditorObjectLabel *m_piSTFormationObjectLabel;
	IScenarioEditorObjectLabel *m_piSTFormationBonusObjectLabel;
	IGameGUIButton *m_piBTFormationAlternativeSample;
	IGameGUIButton *m_piBTFormationAlternativeBonusSample;
	IScenarioEditorObjectLabel *m_piSTFormationAlternativeObjectLabel;
	IScenarioEditorObjectLabel *m_piSTFormationAlternativeBonusObjectLabel;
	IGameGUILabel  *m_piSTFormationCondition;
	IGameGUIButton *m_piBTFormationDecreaseCondition;
	IGameGUIButton *m_piBTFormationIncreaseCondition;
	IGameGUIButton *m_piBTFormationRemove;

	// Entity Layers

	IGameWindow *m_piGREntityLayerList;
	IGameGUIList *m_piLSEntityList;
	IGameGUIList *m_piLSFormationList;
	
	// Options

	IGameGUIButton *m_piBTOptionsTextures;
	IGameGUIButton *m_piBTOptionsSolid;
	IGameGUIButton *m_piBTOptionsBlend;
	IGameGUIButton *m_piBTOptionsShadows;
	IGameGUIButton *m_piBTOptionsLighting;
	IGameGUIButton *m_piBTOptionsFog;
	IGameGUIButton *m_piBTOptionsGod;
	IGameGUIButton *m_piBTOptionsAmmo;
	IGameGUILabel  *m_piSTOptionsVolume;
	IGameGUIButton *m_piBTOptionsDecreaseVolume;
	IGameGUIButton *m_piBTOptionsIncreaseVolume;
	IGameGUILabel  *m_piSTOptionsSoundFxVolume;
	IGameGUIButton *m_piBTOptionsDecreaseSoundFxVolume;
	IGameGUIButton *m_piBTOptionsIncreaseSoundFxVolume;
	IGameGUILabel  *m_piSTOptionsMusicVolume;
	IGameGUIButton *m_piBTOptionsDecreaseMusicVolume;
	IGameGUIButton *m_piBTOptionsIncreaseMusicVolume;
	IGameGUILabel  *m_piSTOptionsDifficulty;
	IGameGUIButton *m_piBTOptionsDecreaseDifficulty;
	IGameGUIButton *m_piBTOptionsIncreaseDifficulty;
	IGameGUILabel  *m_piSTOptionsMouseTraceDistance;
	IGameGUIButton *m_piBTOptionsDecreaseMouseTraceDistance;
	IGameGUIButton *m_piBTOptionsIncreaseMouseTraceDistance;
	
	// File
	IGameWindow	   *m_piGRFile;
	IGameGUIButton *m_piBTFileNew;
	IGameGUIButton *m_piBTFileOpen;
	IGameGUIButton *m_piBTFileSave;
	IGameGUIButton *m_piBTFileSaveAs;
	IGameGUIButton *m_piBTFileExit;

	// General
	IGameGUILabel  *m_piSTGeneralModelName;
	IGameGUILabel  *m_piSTGeneralIntroMusicName;
	IGameGUILabel  *m_piSTGeneralMusicName;
	IGameGUILabel  *m_piSTGeneralColorMapName;
	IGameGUIButton *m_piBTGeneralChangeModel;
	IGameGUIButton *m_piBTGeneralChangeIntroMusic;
	IGameGUIButton *m_piBTGeneralChangeMusic;
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
	IGameGUIButton *m_piBTWaterAuto;
	

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
	IGameGUILabel  *m_piSTPlayAreaDifficulty;
	IGameGUIButton *m_piBTPlayAreaDecreaseDifficulty;
	IGameGUIButton *m_piBTPlayAreaIncreaseDifficulty;
	IGameGUIButton *m_piBTPlayAreaEnable;
	IGameGUIButton *m_piBTPlayAreaAutoTakeOff;
	IGameGUIButton *m_piBTPlayAreaAutoLanding;
	
	// Route Point
	
	IGameGUILabel  *m_piSTRoutePointCaption;
	IGameGUILabel  *m_piSTRoutePointPause;
	IGameGUIButton *m_piBTRoutePointDecreasePause;
	IGameGUIButton *m_piBTRoutePointIncreasePause;
	IGameGUILabel  *m_piSTRoutePointSpeed;
	IGameGUIButton *m_piBTRoutePointDecreaseSpeed;
	IGameGUIButton *m_piBTRoutePointIncreaseSpeed;
	
	
	ISystem					*m_piGameSystem;
	CGameControllerWrapper   m_GameControllerWrapper;
	CPlayerManagerWrapper    m_PlayerManagerWrapper;
	CMusicManagerWrapper     m_MusicManagerWrapper;
	CPlayAreaManagerWrapper  m_PlayAreaManagerWrapper;
	CWorldManagerWrapper	 m_WorldManagerWrapper;
	CEntityManagerWrapper	 m_EntityManagerWrapper;
	CSoundManagerWrapper	 m_SoundManagerWrapper;

	static void RenderEntity(IEntity *piEntity,void *pParam1,void *pParam2);
	
	void ProcessInput(double dTimeFraction,double dRealTimeFraction);
	void ProcessKey(unsigned short nKey,double dTimeFraction,double dRealTimeFraction);

	void ProcessFileNew();
	void ProcessFileOpen();

	void OpenScenario( std::string sScenario );
	void ProcessFileSave();
	void ProcessFileSaveAs();
	void ProcessFileExit();

	void Reset();

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("FPS",m_piSTFps);
		CHILD_MAP_ENTRY("GameTime",m_piSTGameTime);
		
		CHILD_MAP_ENTRY_EX("Options",m_piBTShowOptionsPanel,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Terrain",m_piBTShowTerrainPanel,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Entities",m_piBTShowEntitiesPanel,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Formations",m_piBTShowFormationsPanel,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("File",m_piBTShowFilePanel,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("EntitiesPanel",m_piGREntitiesPanel);
		CHILD_MAP_ENTRY("EntityLayerList",m_piGREntityLayerList);
		CHILD_MAP_ENTRY_EX("NewEntityLayer",m_piBTNewEntityLayer,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("NewEntity",m_piBTNewEntity,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityList",m_piLSEntityList,IGameGUIListEvents);
		

		CHILD_MAP_ENTRY("FormationsPanel",m_piGRFormationsPanel);
		CHILD_MAP_ENTRY_EX("NewFormation",m_piBTNewFormation,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FormationList",m_piLSFormationList,IGameGUIListEvents);
		
		CHILD_MAP_ENTRY("TerrainPanel",m_piGRTerrainPanel);
		CHILD_MAP_ENTRY("LayerList",m_piGRLayerList);
		CHILD_MAP_ENTRY_EX("GeneralProperties",m_piBTShowGeneralProperties,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("WaterProperties",m_piBTShowWaterProperties,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FogProperties",m_piBTShowFogProperties,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("SunProperties",m_piBTShowSunProperties,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("SkyProperties",m_piBTShowSkyProperties,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("PlayAreaProperties",m_piBTShowPlayAreaProperties,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("NewHeightLayer",m_piBTNewHeightLayer,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("NewColorLayer",m_piBTNewColorLayer,IGameGUIButtonEvents);
		
		CHILD_MAP_ENTRY("HeightLayerPanel",m_piGRHeightLayerPanel);
		CHILD_MAP_ENTRY("HeightLayerTextureName",m_piSTHeightLayerName);
		CHILD_MAP_ENTRY("HeightLayerMaxHeight",m_piSTHeightLayerMaxHeight);
		CHILD_MAP_ENTRY("HeightLayerMinHeight",m_piSTHeightLayerMinHeight);
		CHILD_MAP_ENTRY("HeightLayerMargin",m_piSTHeightLayerMargin);
		CHILD_MAP_ENTRY("HeightLayerTextureHorzResolution",m_piSTHeightLayerHorzResolution);
		CHILD_MAP_ENTRY("HeightLayerTextureVertResolution",m_piSTHeightLayerVertResolution);
		CHILD_MAP_ENTRY_EX("HeightLayerTextureSample",m_piBTHeightLayerSample,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("HeightLayerNormalMapSample",m_piBTHeightLayerNormalMapSample,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("HeightLayerDecreaseMaxHeight",m_piBTHeightLayerDecreaseMaxHeight,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("HeightLayerIncreaseMaxHeight",m_piBTHeightLayerIncreaseMaxHeight,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("HeightLayerDecreaseMinHeight",m_piBTHeightLayerDecreaseMinHeight,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("HeightLayerIncreaseMinHeight",m_piBTHeightLayerIncreaseMinHeight,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("HeightLayerDecreaseMargin",m_piBTHeightLayerDecreaseMargin,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("HeightLayerIncreaseMargin",m_piBTHeightLayerIncreaseMargin,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("HeightLayerDecreaseHorzResolution",m_piBTHeightLayerDecreaseHorzResolution,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("HeightLayerIncreaseHorzResolution",m_piBTHeightLayerIncreaseHorzResolution,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("HeightLayerDecreaseVertResolution",m_piBTHeightLayerDecreaseVertResolution,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("HeightLayerIncreaseVertResolution",m_piBTHeightLayerIncreaseVertResolution,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("HeightLayerRemove",m_piBTHeightLayerRemove,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("HeightLayerMoveUp",m_piBTHeightLayerMoveUp,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("HeightLayerMoveDown",m_piBTHeightLayerMoveDown,IGameGUIButtonEvents);
	
		CHILD_MAP_ENTRY("ColorLayerPanel",m_piGRColorLayerPanel);
		CHILD_MAP_ENTRY("ColorLayerTextureName",m_piSTColorLayerName);
		CHILD_MAP_ENTRY("ColorLayerTextureHorzResolution",m_piSTColorLayerHorzResolution);
		CHILD_MAP_ENTRY("ColorLayerTextureVertResolution",m_piSTColorLayerVertResolution);
		CHILD_MAP_ENTRY_EX("ColorLayerAbruptEnd",m_piBTColorLayerAbruptEnd,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ColorLayerTextureSample",m_piBTColorLayerSample,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ColorLayerNormalMapSample",m_piBTColorLayerNormalMapSample,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ColorLayerColorSample",m_piBTColorLayerColorSample,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ColorLayerDecreaseHorzResolution",m_piBTColorLayerDecreaseHorzResolution,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ColorLayerIncreaseHorzResolution",m_piBTColorLayerIncreaseHorzResolution,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ColorLayerDecreaseVertResolution",m_piBTColorLayerDecreaseVertResolution,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ColorLayerIncreaseVertResolution",m_piBTColorLayerIncreaseVertResolution,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ColorLayerRemove",m_piBTColorLayerRemove,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ColorLayerMoveUp",m_piBTColorLayerMoveUp,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ColorLayerMoveDown",m_piBTColorLayerMoveDown,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("EntityLayerPanel",m_piGREntityLayerPanel);
		CHILD_MAP_ENTRY("EntityLayerModelName",m_piSTEntityLayerName);
		CHILD_MAP_ENTRY("EntityLayerObjectLabel",m_piSTEntityLayerObjectLabel);
		CHILD_MAP_ENTRY("EntityLayerSeparation",m_piSTEntityLayerSeparation);
		CHILD_MAP_ENTRY("EntityLayerPositionJitter",m_piSTEntityLayerPositionJitter);
		CHILD_MAP_ENTRY("EntityLayerYawJitter",m_piSTEntityLayerYawJitter);
		CHILD_MAP_ENTRY("EntityBonusObjectLabel",m_piSTEntityBonusObjectLabel);
		CHILD_MAP_ENTRY_EX("EntityBonusSample",m_piBTEntityBonusSample,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityLayerModelSample",m_piBTEntityLayerSample,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityLayerColorSample",m_piBTEntityLayerColorSample,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityLayerRemove",m_piBTEntityLayerRemove,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityLayerDecreaseSeparation",m_piBTEntityLayerDecreaseSeparation,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityLayerIncreaseSeparation",m_piBTEntityLayerIncreaseSeparation,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityLayerDecreasePositionJitter",m_piBTEntityLayerDecreasePositionJitter,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityLayerIncreasePositionJitter",m_piBTEntityLayerIncreasePositionJitter,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityLayerDecreaseYawJitter",m_piBTEntityLayerDecreaseYawJitter,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityLayerIncreaseYawJitter",m_piBTEntityLayerIncreaseYawJitter,IGameGUIButtonEvents);
		

		CHILD_MAP_ENTRY("EntityPanel",m_piGREntityPanel);
		CHILD_MAP_ENTRY("EntityName",m_piSTEntityName);
		CHILD_MAP_ENTRY("EntityObjectLabel",m_piSTEntityObjectLabel);
		CHILD_MAP_ENTRY("EntityYaw",m_piSTEntityYaw);
		CHILD_MAP_ENTRY("EntityCount",m_piSTEntityCount);
		CHILD_MAP_ENTRY("EntityDelay",m_piSTEntityDelay);
		CHILD_MAP_ENTRY("EntityBonusOnChild",m_piSTEntityBonusOnChild);
		CHILD_MAP_ENTRY("EntityRouteDelay",m_piSTEntityRouteDelay);
		CHILD_MAP_ENTRY("EntityInterval",m_piSTEntityInterval);
		CHILD_MAP_ENTRY_EX("EntitySample",m_piBTEntitySample,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityRemove",m_piBTEntityRemove,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityDecreaseYaw",m_piBTEntityDecreaseYaw,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityIncreaseYaw",m_piBTEntityIncreaseYaw,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityDecreaseCount",m_piBTEntityDecreaseCount,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityIncreaseCount",m_piBTEntityIncreaseCount,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityDecreaseDelay",m_piBTEntityDecreaseDelay,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityIncreaseDelay",m_piBTEntityIncreaseDelay,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityDecreaseBonusOnChild",m_piBTEntityDecreaseBonusOnChild,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityIncreaseBonusOnChild",m_piBTEntityIncreaseBonusOnChild,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityDecreaseRouteDelay",m_piBTEntityDecreaseRouteDelay,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityIncreaseRouteDelay",m_piBTEntityIncreaseRouteDelay,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityDecreaseInterval",m_piBTEntityDecreaseInterval,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityIncreaseInterval",m_piBTEntityIncreaseInterval,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityClearRoute",m_piBTEntityClearRoute,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("FormationPanel",m_piGRFormationPanel);
		CHILD_MAP_ENTRY("FormationObjectLabel",m_piSTFormationObjectLabel);
		CHILD_MAP_ENTRY("FormationBonusObjectLabel",m_piSTFormationBonusObjectLabel);
		CHILD_MAP_ENTRY_EX("FormationSample",m_piBTFormationSample,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FormationBonusSample",m_piBTFormationBonusSample,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY("FormationAlternativeObjectLabel",m_piSTFormationAlternativeObjectLabel);
		CHILD_MAP_ENTRY("FormationAlternativeBonusObjectLabel",m_piSTFormationAlternativeBonusObjectLabel);
		CHILD_MAP_ENTRY_EX("FormationAlternativeSample",m_piBTFormationAlternativeSample,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FormationAlternativeBonusSample",m_piBTFormationAlternativeBonusSample,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY("FormationCondition",m_piSTFormationCondition);
		CHILD_MAP_ENTRY_EX("FormationDecreaseCondition",m_piBTFormationDecreaseCondition,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FormationIncreaseCondition",m_piBTFormationIncreaseCondition,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY_EX("FormationRemove",m_piBTFormationRemove,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("OptionsPanel",m_piGROptionsPanel);
		CHILD_MAP_ENTRY_EX("OptionShowTextures",m_piBTOptionsTextures,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionSolid",m_piBTOptionsSolid,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionBlend",m_piBTOptionsBlend,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionShadows",m_piBTOptionsShadows,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionFog",m_piBTOptionsFog,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionLighting",m_piBTOptionsLighting,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionGod",m_piBTOptionsGod,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionAmmo",m_piBTOptionsAmmo,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY("OptionVolume",m_piSTOptionsVolume);
		CHILD_MAP_ENTRY_EX("OptionDecreaseVolume",m_piBTOptionsDecreaseVolume,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionIncreaseVolume",m_piBTOptionsIncreaseVolume,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY("OptionSoundFxVolume",m_piSTOptionsSoundFxVolume);
		CHILD_MAP_ENTRY_EX("OptionDecreaseSoundFxVolume",m_piBTOptionsDecreaseSoundFxVolume,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionIncreaseSoundFxVolume",m_piBTOptionsIncreaseSoundFxVolume,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY("OptionMusicVolume",m_piSTOptionsMusicVolume);
		CHILD_MAP_ENTRY_EX("OptionDecreaseMusicVolume",m_piBTOptionsDecreaseMusicVolume,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionIncreaseMusicVolume",m_piBTOptionsIncreaseMusicVolume,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY("OptionDifficulty",m_piSTOptionsDifficulty);
		CHILD_MAP_ENTRY_EX("OptionDecreaseDifficulty",m_piBTOptionsDecreaseDifficulty,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionIncreaseDifficulty",m_piBTOptionsIncreaseDifficulty,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY("OptionMouseTraceDistance",m_piSTOptionsMouseTraceDistance);
		CHILD_MAP_ENTRY_EX("OptionDecreaseMouseTraceDistance",m_piBTOptionsDecreaseMouseTraceDistance,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionIncreaseMouseTraceDistance",m_piBTOptionsIncreaseMouseTraceDistance,IGameGUIButtonEvents);
		
		CHILD_MAP_ENTRY("FilePanel",m_piGRFile);
		CHILD_MAP_ENTRY_EX("FileNew",m_piBTFileNew,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FileOpen",m_piBTFileOpen,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FileSave",m_piBTFileSave,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FileSaveAs",m_piBTFileSaveAs,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FileExit",m_piBTFileExit,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("GeneralModelName",m_piSTGeneralModelName);
		CHILD_MAP_ENTRY("GeneralMusicName",m_piSTGeneralMusicName);
		CHILD_MAP_ENTRY("GeneralIntroMusicName",m_piSTGeneralIntroMusicName);
		CHILD_MAP_ENTRY("GeneralColorMapName",m_piSTGeneralColorMapName);
		CHILD_MAP_ENTRY_EX("GeneralPanel",m_piGRGeneralPanel,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("GeneralChangeModel",m_piBTGeneralChangeModel,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("GeneralChangeMusic",m_piBTGeneralChangeMusic,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("GeneralChangeIntroMusic",m_piBTGeneralChangeIntroMusic,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("GeneralChangeColorMap",m_piBTGeneralChangeColorMap,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("WaterPanel",m_piGRWaterPanel);
		CHILD_MAP_ENTRY("WaterTextureName",m_piSTWaterName);
		CHILD_MAP_ENTRY("WaterSecondTextureName",m_piSTWaterSecondName);
		CHILD_MAP_ENTRY("WaterHeight",m_piSTWaterHeight);
		CHILD_MAP_ENTRY("WaterOpacity",m_piSTWaterOpacity);
		CHILD_MAP_ENTRY("WaterSpeed",m_piSTWaterSpeed);
		CHILD_MAP_ENTRY("WaterTextureHorzResolution",m_piSTWaterHorzResolution);
		CHILD_MAP_ENTRY("WaterTextureVertResolution",m_piSTWaterVertResolution);
		CHILD_MAP_ENTRY_EX("WaterTextureSample",m_piBTWaterSample,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("WaterSecondTextureSample",m_piBTWaterSecondSample,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("WaterDecreaseHeight",m_piBTWaterDecreaseHeight,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("WaterIncreaseHeight",m_piBTWaterIncreaseHeight,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("WaterDecreaseOpacity",m_piBTWaterDecreaseOpacity,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("WaterIncreaseOpacity",m_piBTWaterIncreaseOpacity,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("WaterDecreaseSpeed",m_piBTWaterDecreaseSpeed,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("WaterIncreaseSpeed",m_piBTWaterIncreaseSpeed,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("WaterDecreaseHorzResolution",m_piBTWaterDecreaseHorzResolution,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("WaterIncreaseHorzResolution",m_piBTWaterIncreaseHorzResolution,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("WaterDecreaseVertResolution",m_piBTWaterDecreaseVertResolution,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("WaterIncreaseVertResolution",m_piBTWaterIncreaseVertResolution,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("WaterEnable",m_piBTWaterEnable,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("WaterAuto",m_piBTWaterAuto,IGameGUIButtonEvents);
		
		CHILD_MAP_ENTRY("FogPanel",m_piGRFogPanel);
		CHILD_MAP_ENTRY("FogStart",m_piSTFogStart);
		CHILD_MAP_ENTRY("FogEnd",m_piSTFogEnd);
		CHILD_MAP_ENTRY_EX("FogDecreaseStart",m_piBTFogDecreaseStart,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FogIncreaseStart",m_piBTFogIncreaseStart,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FogDecreaseEnd",m_piBTFogDecreaseEnd,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FogIncreaseEnd",m_piBTFogIncreaseEnd,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FogColorSample",m_piBTFogColorSample,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FogEnable",m_piBTFogEnable,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FogFitWater",m_piBTFogFitWater,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FogFitSky",m_piBTFogFitSky,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("RoutePointPanel",m_piGRRoutePointPanel);
		CHILD_MAP_ENTRY("RoutePointCaption",m_piSTRoutePointCaption);
		CHILD_MAP_ENTRY("RoutePointPause",m_piSTRoutePointPause);
		CHILD_MAP_ENTRY("RoutePointSpeed",m_piSTRoutePointSpeed);
		CHILD_MAP_ENTRY_EX("RoutePointDecreasePause",m_piBTRoutePointDecreasePause,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("RoutePointIncreasePause",m_piBTRoutePointIncreasePause,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("RoutePointDecreaseSpeed",m_piBTRoutePointDecreaseSpeed,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("RoutePointIncreaseSpeed",m_piBTRoutePointIncreaseSpeed,IGameGUIButtonEvents);
		
		CHILD_MAP_ENTRY("SunPanel",m_piGRSunPanel);
		CHILD_MAP_ENTRY("SunDistance",m_piSTSunDistance);
		CHILD_MAP_ENTRY("SunElevation",m_piSTSunElevation);
		CHILD_MAP_ENTRY("SunAzimuth",m_piSTSunAzimuth);
		CHILD_MAP_ENTRY_EX("SunDecreaseDistance",m_piBTSunDecreaseDistance,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("SunIncreaseDistance",m_piBTSunIncreaseDistance,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("SunDecreaseElevation",m_piBTSunDecreaseElevation,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("SunIncreaseElevation",m_piBTSunIncreaseElevation,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("SunDecreaseAzimuth",m_piBTSunDecreaseAzimuth,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("SunIncreaseAzimuth",m_piBTSunIncreaseAzimuth,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("SunColorSample",m_piBTSunColorSample,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("AmbientColorSample",m_piBTAmbientColorSample,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("SkyPanel",m_piGRSkyPanel);
		CHILD_MAP_ENTRY("SkyTextureName",m_piSTSkyName);
		CHILD_MAP_ENTRY("SkyOpacity",m_piSTSkyOpacity);
		CHILD_MAP_ENTRY("SkySpeed",m_piSTSkySpeed);
		CHILD_MAP_ENTRY("SkyTextureHorzResolution",m_piSTSkyHorzResolution);
		CHILD_MAP_ENTRY("SkyTextureVertResolution",m_piSTSkyVertResolution);
		CHILD_MAP_ENTRY_EX("SkyTextureSample",m_piBTSkySample,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("SkyDecreaseOpacity",m_piBTSkyDecreaseOpacity,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("SkyIncreaseOpacity",m_piBTSkyIncreaseOpacity,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("SkyDecreaseSpeed",m_piBTSkyDecreaseSpeed,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("SkyIncreaseSpeed",m_piBTSkyIncreaseSpeed,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("SkyDecreaseHorzResolution",m_piBTSkyDecreaseHorzResolution,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("SkyIncreaseHorzResolution",m_piBTSkyIncreaseHorzResolution,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("SkyDecreaseVertResolution",m_piBTSkyDecreaseVertResolution,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("SkyIncreaseVertResolution",m_piBTSkyIncreaseVertResolution,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("SkyEnable",m_piBTSkyEnable,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("PlayAreaPanel",m_piGRPlayAreaPanel);
		CHILD_MAP_ENTRY("PlayAreaCameraDistance",m_piSTPlayAreaCameraDistance);
		CHILD_MAP_ENTRY("PlayAreaCameraSpeed",m_piSTPlayAreaCameraSpeed);
		CHILD_MAP_ENTRY("PlayAreaCameraViewAngle",m_piSTPlayAreaCameraViewAngle);
		CHILD_MAP_ENTRY("PlayAreaCameraAspectRatio",m_piSTPlayAreaCameraAspectRatio);
		CHILD_MAP_ENTRY("PlayAreaAirPlane",m_piSTPlayAreaAirPlane);
		CHILD_MAP_ENTRY("PlayAreaScroll",m_piSTPlayAreaScroll);
		CHILD_MAP_ENTRY("PlayAreaDifficulty",m_piSTPlayAreaDifficulty);
		CHILD_MAP_ENTRY_EX("PlayAreaDecreaseCameraDistance",m_piBTPlayAreaDecreaseCameraDistance,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("PlayAreaIncreaseCameraDistance",m_piBTPlayAreaIncreaseCameraDistance,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("PlayAreaDecreaseCameraSpeed",m_piBTPlayAreaDecreaseCameraSpeed,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("PlayAreaIncreaseCameraSpeed",m_piBTPlayAreaIncreaseCameraSpeed,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("PlayAreaDecreaseCameraViewAngle",m_piBTPlayAreaDecreaseCameraViewAngle,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("PlayAreaIncreaseCameraViewAngle",m_piBTPlayAreaIncreaseCameraViewAngle,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("PlayAreaDecreaseCameraAspectRatio",m_piBTPlayAreaDecreaseCameraAspectRatio,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("PlayAreaIncreaseCameraAspectRatio",m_piBTPlayAreaIncreaseCameraAspectRatio,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("PlayAreaDecreaseAirPlane",m_piBTPlayAreaDecreaseAirPlane,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("PlayAreaIncreaseAirPlane",m_piBTPlayAreaIncreaseAirPlane,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("PlayAreaDecreaseScroll",m_piBTPlayAreaDecreaseScroll,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("PlayAreaIncreaseScroll",m_piBTPlayAreaIncreaseScroll,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("PlayAreaDecreaseDifficulty",m_piBTPlayAreaDecreaseDifficulty,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("PlayAreaIncreaseDifficulty",m_piBTPlayAreaIncreaseDifficulty,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("PlayAreaAutoTakeOff",m_piBTPlayAreaAutoTakeOff,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("PlayAreaAutoLanding",m_piBTPlayAreaAutoLanding,IGameGUIButtonEvents);
	END_CHILD_MAP()

public:

	CScenarioEditorColorSelectorWrapper m_ColorSelector;
	CScenarioEditorObjectSelectorWrapper m_ObjectSelector;
	CScenarioEditorObjectListSelectorWrapper m_ObjectListSelector;

	bool m_bSimulationStarted;
	bool m_bInspectionMode;
	// Sobrecarga para cambiar el valor por defecto del sistema de referencia.

	BEGIN_PROP_MAP(CScenarioEditorMainWindow)
		PROP_CLASS_CHAIN(CGameWindowBase);
		PROP_VALUE_FLAGS(m_eReferenceSystem,"ReferenceSystem",eGameGUIReferenceSystem_Relative,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bAutoGenerateBSP,"GenerateBSP",1,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bAutoUpdateBSP,"UpdateBSP",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_d3DFontSize,"3DFontSize",0,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP()

	bool InitWindow(IGameWindow *piParent,bool bPopup);
	void DestroyWindow();

	void OnWantFocus(bool *pbWant);
	void OnDraw(IGenericRender *piRender);
	void OnDrawBackground(IGenericRender *piRender);
	void OnButtonClicked(IGameGUIButton *piControl);
	void OnSelectionChanged(IGameGUIList *piControl,int nElement,std::string sElement);
	void OnSelectionDoubleCliked(IGameGUIList *piControl,int nElement,std::string sElement);
	
	void OnCharacter( int nKey,bool *pbProcessed );
	void OnKeyDown(int nKey,bool *pbProcessed);
	void OnMouseDown(int nButton,double x,double y);
	void OnMouseMove(double x,double y);
	void OnMouseDoubleClick(int nButton,double x,double y);	
	void OnMouseUp(int nButton,double x,double y);

	void OnColorChanged(CVector vColor);
	
	void UpdateLayerPanel();
	void UpdateTexturization();
	void UpdateCaption();

	void CenterCamera();

	bool GetHeightAt(CVector vPoint,bool bIgnoreTerrainObjects, double *pdHeight);

	void SetupRenderOptions(IGenericRender *piRender,IGenericCamera *piCamera);

	bool GetTerrainCoordinatesFromLine(CLine line,bool bIgnoreTerrainObjects, CVector *pTerrainPos);
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
	void UpdateCheckPoints();
	
	void RenderRoute( IGenericRender * piRender, int nSelectedEntity, int nSelectedRoutePoint );
	void RenderPlayArea(bool bSelectionRender);
	void RenderPlayerRoutes(bool bSelectionRender);
	void RenderCheckPoints(bool bSelectionRender);
	
	CScenarioEditorMainWindow(void);
	~CScenarioEditorMainWindow(void);
};


