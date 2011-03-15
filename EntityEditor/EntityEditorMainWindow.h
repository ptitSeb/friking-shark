#pragma once

#include "EntityEditor.h"
#include "EntityEditorObjectSelector.h"

DECLARE_CUSTOM_WRAPPER1(CEntityEditorObjectSelectorWrapper,IEntityEditorObjectSelector,m_piObjectSelector)
DECLARE_CUSTOM_WRAPPER1(CEntityEditorNamedObjectSelectorWrapper,IEntityEditorNamedObjectSelector,m_piObjectSelector)
DECLARE_CUSTOM_WRAPPER1(CEntityEditorClassSelectorWrapper,IEntityEditorClassSelector,m_piClassSelector)

#define KEY_FORWARD		1
#define KEY_BACK		2
#define KEY_RIGHT		3
#define KEY_LEFT		4
#define KEY_UP			5
#define KEY_DOWN		6
#define KEY_PAUSE		7
#define KEY_PROCESS_ONE_FRAME		8

enum EPropertyPanel
{
	ePropertyPanel_None,
	ePropertyPanel_Animation,
	ePropertyPanel_Sound,
	ePropertyPanel_Model,
	ePropertyPanel_Event,
	ePropertyPanel_ParticleSystem,
	ePropertyPanel_Count
};

class CFakeEntity: virtual public CEntityBase
{
public:
	
	CFakeEntity(){}
	~CFakeEntity(){}	
};

class CEntityEditorMainWindow: public CGameWindowBase, public IGameGUIButtonEvents, public IGameGUIListEvents, public IEntityEditorPropertyPanelEvents
{
public:
	CConfigFile				m_GUIConfigFile;
	CViewportWrapper		m_Viewport;
	CRenderWrapper			m_Render;
	CGenericCameraWrapper	m_Camera;
	CFrameManagerWrapper	m_FrameManager;
	CPhysicManagerWrapper	m_PhysicManagerWrapper;
	
	ISystem					*m_piGameSystem;
	CGameControllerWrapper   m_GameControllerWrapper;
	CSoundManagerWrapper	 m_SoundManagerWrapper;
	
	std::vector<CAnimationDesignTypeWrapper> m_vAnimations;
	IAnimation 				*m_piAnimation;
	CEntityTypeWrapper		 m_EntityType;
	std::string				 m_sEntityName;
	
	IEntityEditorPropertyPanel *m_ppiPropertyPanels[ePropertyPanel_Count];

	double					 m_d3DFontSize;
	
	bool		m_bSolid;
	bool		m_bTextures;
	
	bool		m_bShowFilePanel;
	bool		m_bShowEntitiesPanel;
	bool		m_bShowEntityPanel;
	bool		m_bShowOptionsPanel;
	bool		m_bShowPlayAreaPanel;
	
	bool		m_bShowModelPanel;
	bool		m_bShowSoundPanel;
	bool		m_bShowEventPanel;
	bool		m_bShowParticleSystemPanel;
	
	CFakeEntity	*m_pEntity;
	
	IGameGUILabel *m_piSTFps;
	IGameGUILabel *m_piSTVolume;
	IGameGUILabel *m_piBTDecreaseVolume;
	IGameGUILabel *m_piBTIncreaseVolume;

	IGameWindow	  *m_piGREntityPanel;
	IGameWindow	  *m_piGREntitiesPanel;
	IGameWindow	  *m_piGROptionsPanel;
	IGameWindow   *m_piGREntityList;

	IGameGUIButton *m_piBTShowOptionsPanel;
	IGameGUIButton *m_piBTShowFilePanel;

	IGameGUIList   *m_piLSStates;
	IGameGUIList   *m_piLSAnimations;
	IGameGUIButton *m_piBTNewAnimation;
	
	IGameWindow     *m_piGRObjectPanel;
	IGameGUIList    *m_piLSObjects;

	IGameGUIButton  *m_piBTNewModel;
	IGameGUIButton  *m_piBTNewSound;
	IGameGUIButton  *m_piBTNewEvent;
	IGameGUIButton  *m_piBTNewParticleSystem;
	
	// Options

	IGameGUIButton *m_piBTOptionsTextures;
	IGameGUIButton *m_piBTOptionsSolid;
	IGameGUIButton *m_piBTOptionsShowWorld;
	IGameGUIButton *m_piBTOptionsAutoAlign;
	

	// File
	IGameWindow	   *m_piGREntity;
	IGameGUIButton *m_piBTEntityNew;
	IGameGUIButton *m_piBTEntityOpen;
	IGameGUIButton *m_piBTEntitySave;
	IGameGUIButton *m_piBTEntitySaveAs;
	IGameGUIButton *m_piBTEntityCompile;
	IGameGUIButton *m_piBTEntityRemove;
	IGameGUIButton *m_piBTEntityExit;

	void ProcessInput(double dTimeFraction,double dRealTimeFraction);
	void ProcessKey(unsigned short nKey,double dTimeFraction,double dRealTimeFraction);

	void ProcessFileNew();
	void ProcessFileOpen();

	void ProcessFileSave();
	void ProcessFileSaveAs();
	void ProcessFileRemove();
	void ProcessFileExit();
	
	void ProcessNewModel();
	void ProcessNewSound();
	void ProcessNewEvent();
	void ProcessNewParticleSystem();
	void ProcessNewAnimation();
	
	void Reset();

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("FPS",m_piSTFps);
		CHILD_MAP_ENTRY("Volume",m_piSTVolume);
		CHILD_MAP_ENTRY_EX("DecreaseVolume",m_piBTDecreaseVolume,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("IncreaseVolume",m_piBTIncreaseVolume,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY_EX("Options",m_piBTShowOptionsPanel,IGameGUIButtonEvents);
		//CHILD_MAP_ENTRY_EX("Entities",m_piBTShowEntitiesPanel,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Entity",m_piBTShowFilePanel,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("OptionsPanel",m_piGROptionsPanel);
		CHILD_MAP_ENTRY_EX("OptionShowTextures",m_piBTOptionsTextures,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionSolid",m_piBTOptionsSolid,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY_EX("StateList",m_piLSStates,IGameGUIListEvents);
		CHILD_MAP_ENTRY_EX("AnimationList",m_piLSAnimations,IGameGUIListEvents);
		CHILD_MAP_ENTRY_EX("NewAnimation",m_piBTNewAnimation,IGameGUIButtonEvents);
		
		CHILD_MAP_ENTRY("ObjectPanel",m_piGRObjectPanel);
		CHILD_MAP_ENTRY_EX("ObjectList",m_piLSObjects,IGameGUIListEvents);
		CHILD_MAP_ENTRY_EX("NewModel",m_piBTNewModel,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("NewSound",m_piBTNewSound,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("NewEvent",m_piBTNewEvent,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("NewParticleSystem",m_piBTNewParticleSystem,IGameGUIButtonEvents);
		
		CHILD_MAP_ENTRY("EntityPanel",m_piGREntity);
		CHILD_MAP_ENTRY_EX("EntityNew",m_piBTEntityNew,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityOpen",m_piBTEntityOpen,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntitySave",m_piBTEntitySave,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntitySaveAs",m_piBTEntitySaveAs,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityRemove",m_piBTEntityRemove,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityExit",m_piBTEntityExit,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY("EntityPanel",m_piGREntity);
		CHILD_MAP_ENTRY_EX("AnimationPropertyPanel",m_ppiPropertyPanels[ePropertyPanel_Animation],IEntityEditorPropertyPanelEvents);
		CHILD_MAP_ENTRY_EX("ModelPropertyPanel",m_ppiPropertyPanels[ePropertyPanel_Model],IEntityEditorPropertyPanelEvents);
		CHILD_MAP_ENTRY_EX("EventPropertyPanel",m_ppiPropertyPanels[ePropertyPanel_Event],IEntityEditorPropertyPanelEvents);
		CHILD_MAP_ENTRY_EX("SoundPropertyPanel",m_ppiPropertyPanels[ePropertyPanel_Sound],IEntityEditorPropertyPanelEvents);
		CHILD_MAP_ENTRY_EX("ParticleSystemPropertyPanel",m_ppiPropertyPanels[ePropertyPanel_ParticleSystem],IEntityEditorPropertyPanelEvents);
		
	END_CHILD_MAP()
	
	BEGIN_PROP_MAP(CEntityEditorMainWindow)
		PROP_CLASS_CHAIN(CGameWindowBase)
		PROP_VALUE_FLAGS(m_d3DFontSize,"3DFontSize",0,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP();
public:
	

	CEntityEditorClassSelectorWrapper  m_ClassSelector;
	CEntityEditorObjectSelectorWrapper m_ObjectSelector;
	CEntityEditorNamedObjectSelectorWrapper m_NamedObjectSelector;
	
	bool m_bSimulationStarted;
	bool m_bInspectionMode;

	
	bool InitWindow(IGameWindow *piParent,bool bPopup);
	void DestroyWindow();

	void OnWantFocus(bool *pbWant);
	void OnDraw(IGenericRender *piRender);
	void OnButtonClicked(IGameGUIButton *piControl);
	void OnKeyDown(int nKey,bool *pbProcessed);
	void OnMouseDown(int nButton,double x,double y);
	void OnMouseMove(double x,double y);
	void OnMouseUp(int nButton,double x,double y);

	void UpdateVisiblePanels();
	void UpdateStateList();
	void UpdateAnimationList();
	void UpdateObjectList();
	void UpdateSelectedAnimation();
	void UpdateSelectedObject();
	
	void UpdateRunningAnimation();
	

	void UpdateCaption();
	void CenterCamera();
	void SetupRenderOptions(IGenericRender *piRender,IGenericCamera *piCamera);
	void StopGameSimulation();
	void StartGameSimulation();
	
	void OnSelectionChanged(IGameGUIList *piControl,int nElement,std::string sElement);
	void OnSelectionDoubleCliked(IGameGUIList *piControl,int nElement,std::string sElement);

	// IEntityEditorPropertyPanelEvents
	void OnObjectChanged(IEntityEditorPropertyPanel *piPanel,ISystemObject *piObject);
	void OnObjectRemoved(IEntityEditorPropertyPanel *piPanel,ISystemObject *piObject);

	CEntityEditorMainWindow(void);
	~CEntityEditorMainWindow(void);
};


