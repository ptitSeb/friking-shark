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

#include "FormationEditor.h"
#include "FormationEditorObjectSelector.h"

DECLARE_CUSTOM_WRAPPER1(CFormationEditorObjectSelectorWrapper,IFormationEditorObjectSelector,m_piObjectSelector)

#define KEY_FORWARD		1
#define KEY_BACK		2
#define KEY_RIGHT		3
#define KEY_LEFT		4
#define KEY_UP			5
#define KEY_DOWN		6
#define KEY_PAUSE		7
#define KEY_PROCESS_ONE_FRAME		8

struct SEntityControls
{
	CButtonWrapper					m_BTListRow;
	CObjectLabelWrapper				m_STEntity;
	unsigned long					m_nElementId;
	IEntityType						*m_piEntityType;
	IDesignObject					*m_piDesignObject;
	ISystemObject					*m_piObject;
	
	SEntityControls(){m_piEntityType=NULL;m_piDesignObject=NULL;m_piObject=NULL;m_nElementId=0;}
	~SEntityControls(){REL(m_piEntityType);REL(m_piDesignObject);REL(m_piObject);}
};

class CFormationEditorMainWindow: public CGameWindowBase, public IGameGUIButtonEvents
{
public:
	CConfigFile				m_GUIConfigFile;
	CViewportWrapper		m_Viewport;
	CRenderWrapper			m_Render;
	CGenericCameraWrapper	m_Camera;
	CFrameManagerWrapper	m_FrameManager;
	
	ISystem					*m_piGameSystem;
	CGameControllerWrapper   m_GameControllerWrapper;
	CPlayAreaManagerWrapper  m_PlayAreaManagerWrapper;
	CEntityManagerWrapper    m_EntityManagerWrapper;
	CSoundManagerWrapper	 m_SoundManagerWrapper;
	CWorldManagerWrapper	 m_WorldManagerWrapper;
	
	CFormationTypeWrapper    m_FormationType;
	CFormationWrapper		 m_Formation;
	unsigned int			 m_nFormationId;
	std::string 			 m_sFormationName;
	
	double					 m_d3DFontSize;
	std::string 			 m_sWorldModelFile;
	std::string 			 m_sWorldTextureFile;
	double					 m_dAutoAlignThreshold;
	bool					 m_bPauseOnNextFrame;
	
	std::vector<SEntityControls *>		m_vEntityControls;

	unsigned long		m_dwNexControlKey;
	bool				m_bSolid;
	bool				m_bTextures;
	bool				m_bColors;
	bool				m_bRenderPlayArea;
	bool				m_bRenderWorld;
	bool				m_bAutoAlign;

	
	bool				m_bShowFilePanel;
	bool				m_bShowEntitiesPanel;
	bool				m_bShowEntityPanel;
	bool				m_bShowOptionsPanel;
	bool				m_bShowPlayAreaPanel;
	
	bool				m_bMovingRoutePoint;
	int					m_nSelectedRoutePoint;
	int					m_nSelectedEntity;


	IGameGUILabel *m_piSTFps;
	IGameGUILabel *m_piSTVolume;
	IGameGUILabel *m_piBTDecreaseVolume;
	IGameGUILabel *m_piBTIncreaseVolume;

	IGameWindow	  *m_piGREntityPanel;
	IGameWindow	  *m_piGRPlayAreaPanel;
	IGameWindow	  *m_piGREntitiesPanel;
	IGameWindow	  *m_piGROptionsPanel;
	IGameWindow   *m_piGREntityList;

	IGameGUIButton *m_piBTShowOptionsPanel;
	IGameGUIButton *m_piBTShowEntitiesPanel;
	IGameGUIButton *m_piBTShowFilePanel;
	IGameGUIButton *m_piBTShowPlayAreaProperties;

	// Tools
	IGameGUIButton *m_piBTFlipV;
	IGameGUIButton *m_piBTFlipH;
	IGameGUIButton *m_piBTRotateRight;
	IGameGUIButton *m_piBTRotateLeft;
	IGameGUIButton *m_piBTMoveUp;
	IGameGUIButton *m_piBTMoveDown;
	IGameGUIButton *m_piBTMoveRight;
	IGameGUIButton *m_piBTMoveLeft;
	
	// Entity

	IGameGUIButton *m_piBTNewEntity;
	IGameGUILabel  *m_piSTEntityName;
	IFormationEditorObjectLabel *m_piSTEntityObjectLabel;
	IGameGUIButton *m_piBTEntitySample;
	IGameGUIButton *m_piBTEntityRemove;
	IGameGUILabel  *m_piSTEntityCount;
	IGameGUIButton *m_piBTEntityDecreaseCount;
	IGameGUIButton *m_piBTEntityIncreaseCount;
	IGameGUILabel  *m_piSTEntityDelay;
	IGameGUIButton *m_piBTEntityDecreaseDelay;
	IGameGUIButton *m_piBTEntityIncreaseDelay;
	IGameGUILabel  *m_piSTEntityInterval;
	IGameGUIButton *m_piBTEntityDecreaseInterval;
	IGameGUIButton *m_piBTEntityIncreaseInterval;
	IGameGUIButton *m_piBTEntityClearRoute;

	// Options

	IGameGUIButton *m_piBTOptionsTextures;
	IGameGUIButton *m_piBTOptionsSolid;
	IGameGUIButton *m_piBTOptionsShowWorld;
	IGameGUIButton *m_piBTOptionsAutoAlign;
	

	// File
	IGameWindow	   *m_piGRFormation;
	IGameGUIButton *m_piBTFormationNew;
	IGameGUIButton *m_piBTFormationOpen;
	IGameGUIButton *m_piBTFormationSave;
	IGameGUIButton *m_piBTFormationSaveAs;
	IGameGUIButton *m_piBTFormationCompile;
	IGameGUIButton *m_piBTFormationRemove;
	IGameGUIButton *m_piBTFormationExit;

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

	CVector WorldToFormation(CVector vWorldPoint);
	CVector FormationToWorld(CVector vFormationPoint);
	
	CVector AutoAlign(CVector vPoint,int nEntity,int nRoutePoint);
	
	
	void ProcessInput(double dTimeFraction,double dRealTimeFraction);
	void ProcessKey(unsigned short nKey,double dTimeFraction,double dRealTimeFraction);

	void ProcessFileNew();
	void ProcessFileOpen();

	void ProcessFileSave();
	void ProcessFileSaveAs();
	void ProcessFileRemove();
	void ProcessFileExit();
	
	void ProcessAddEntity();
	
	void Reset();

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("FPS",m_piSTFps);
		CHILD_MAP_ENTRY("Volume",m_piSTVolume);
		CHILD_MAP_ENTRY_EX("DecreaseVolume",m_piBTDecreaseVolume,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("IncreaseVolume",m_piBTIncreaseVolume,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY_EX("PlayAreaProperties",m_piBTShowPlayAreaProperties,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Options",m_piBTShowOptionsPanel,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Entities",m_piBTShowEntitiesPanel,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Formation",m_piBTShowFilePanel,IGameGUIButtonEvents);
		
		
		CHILD_MAP_ENTRY_EX("FlipV",m_piBTFlipV,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FlipH",m_piBTFlipH,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("RotateRight",m_piBTRotateRight,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("RotateLeft",m_piBTRotateLeft,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY_EX("MoveUp",m_piBTMoveUp,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("MoveDown",m_piBTMoveDown,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("MoveRight",m_piBTMoveRight,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("MoveLeft",m_piBTMoveLeft,IGameGUIButtonEvents);
		
		CHILD_MAP_ENTRY("EntitiesPanel",m_piGREntitiesPanel);
		CHILD_MAP_ENTRY("EntityList",m_piGREntityList);
		CHILD_MAP_ENTRY_EX("NewEntity",m_piBTNewEntity,IGameGUIButtonEvents);
	
		CHILD_MAP_ENTRY("EntityPanel",m_piGREntityPanel);
		CHILD_MAP_ENTRY("EntityName",m_piSTEntityName);
		CHILD_MAP_ENTRY("EntityObjectLabel",m_piSTEntityObjectLabel);
		CHILD_MAP_ENTRY("EntityCount",m_piSTEntityCount);
		CHILD_MAP_ENTRY("EntityDelay",m_piSTEntityDelay);
		CHILD_MAP_ENTRY("EntityInterval",m_piSTEntityInterval);
		CHILD_MAP_ENTRY_EX("EntityDecreaseCount",m_piBTEntityDecreaseCount,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityIncreaseCount",m_piBTEntityIncreaseCount,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityDecreaseDelay",m_piBTEntityDecreaseDelay,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityIncreaseDelay",m_piBTEntityIncreaseDelay,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityDecreaseInterval",m_piBTEntityDecreaseInterval,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityIncreaseInterval",m_piBTEntityIncreaseInterval,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntitySample",m_piBTEntitySample,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityRemove",m_piBTEntityRemove,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityClearRoute",m_piBTEntityClearRoute,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("OptionsPanel",m_piGROptionsPanel);
		CHILD_MAP_ENTRY_EX("OptionShowTextures",m_piBTOptionsTextures,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionSolid",m_piBTOptionsSolid,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionShowWorld",m_piBTOptionsShowWorld,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionAutoAlign",m_piBTOptionsAutoAlign,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("FormationPanel",m_piGRFormation);
		CHILD_MAP_ENTRY_EX("FormationNew",m_piBTFormationNew,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FormationOpen",m_piBTFormationOpen,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FormationSave",m_piBTFormationSave,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FormationSaveAs",m_piBTFormationSaveAs,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FormationRemove",m_piBTFormationRemove,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FormationExit",m_piBTFormationExit,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("PlayAreaPanel",m_piGRPlayAreaPanel);
		CHILD_MAP_ENTRY("PlayAreaCameraDistance",m_piSTPlayAreaCameraDistance);
		CHILD_MAP_ENTRY("PlayAreaCameraSpeed",m_piSTPlayAreaCameraSpeed);
		CHILD_MAP_ENTRY("PlayAreaCameraViewAngle",m_piSTPlayAreaCameraViewAngle);
		CHILD_MAP_ENTRY("PlayAreaCameraAspectRatio",m_piSTPlayAreaCameraAspectRatio);
		CHILD_MAP_ENTRY("PlayAreaAirPlane",m_piSTPlayAreaAirPlane);
		CHILD_MAP_ENTRY("PlayAreaScroll",m_piSTPlayAreaScroll);
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
	END_CHILD_MAP()
	
	BEGIN_PROP_MAP(CFormationEditorMainWindow)
		PROP_CLASS_CHAIN(CGameWindowBase)
		PROP_VALUE_FLAGS(m_sWorldModelFile,"WorldModelFile","",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_sWorldTextureFile,"WorldTextureFile","",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_d3DFontSize,"3DFontSize",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dAutoAlignThreshold,"AutoAlignThreshold",3,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP();
public:

	CFormationEditorObjectSelectorWrapper m_ObjectSelector;

	bool m_bSimulationStarted;
	bool m_bInspectionMode;

	
	bool InitWindow(IGameWindow *piParent,bool bPopup);
	void DestroyWindow();

	void OnWantFocus(bool *pbWant);
	void OnDraw(IGenericRender *piRender);
	void OnButtonClicked(IGameGUIButton *piControl);
	void OnCharacter( int nKey,bool *pbProcessed );
	void OnKeyDown(int nKey,bool *pbProcessed);
	void OnMouseDown(int nButton,double x,double y);
	void OnMouseMove(double x,double y);
	void OnMouseUp(int nButton,double x,double y);

	void UpdateLayerPanel();
	void UpdateCaption();

	void CenterCamera(bool bForceWholeFormation=false);

	void SetupRenderOptions(IGenericRender *piRender,IGenericCamera *piCamera);

	bool GetAirPlaneCoordinatesFromCursorPos(double x,double y,CVector *pAirPlanePos);
	double GetAirPlaneAbsoluteHeight();

	void StopGameSimulation();
	void StartGameSimulation();
	void UpdateEntityControls();

	CFormationEditorMainWindow(void);
	~CFormationEditorMainWindow(void);
	
	void RenderRoute( IGenericRender * piRender,int nEntity, int nSelectedEntity, int nSelectedRoutePoint );
};


