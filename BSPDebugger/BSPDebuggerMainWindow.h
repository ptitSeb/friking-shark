#pragma once

#include "BSPDebugger.h"
#include "GameGUITranslationGizmo.h"
#include "GameGUIRotationGizmo.h"
#include "GameGUIBBoxGizmo.h"

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
	ePropertyPanel_Polygon,
	ePropertyPanel_Count
};

enum EBSPDebuggerView
{
	eBSPDebuggerView_Perspective,
	eBSPDebuggerView_Front,
	eBSPDebuggerView_Back,
	eBSPDebuggerView_Right,
	eBSPDebuggerView_Left,
	eBSPDebuggerView_Top,
	eBSPDebuggerView_Bottom
};

class CBSPDebuggerMainWindow: public CGameWindowBase, public IGameGUIButtonEvents, public IGameGUIListEvents, public IBSPDebuggerPropertyPanelEvents
{
public:
	CConfigFile				m_GUIConfigFile;
	CViewportWrapper		m_Viewport;
	CRenderWrapper			m_Render;
	CGenericCameraWrapper	m_Camera;
	CFrameManagerWrapper	m_FrameManager;

		
	CModelWrapper		 	 m_Model;
	std::string				 m_sModelName;
	
	IBSPDebuggerPropertyPanel *m_ppiPropertyPanels[ePropertyPanel_Count];

	double					 m_d3DFontSize;
		
	bool		m_bSolid;
	bool		m_bTextures;
	bool		m_bShowModel;
	
	bool		m_bShowFilePanel;
	bool		m_bShowOptionsPanel;
	
	CBSPNode 	*m_pBSP;
	CBSPNode 	*m_pCurrentBSPNode;
	std::vector<CBSPNode*> m_vNodeStack;
	std::vector<CBSPDrawNode *> m_vDrawNodes;
	
	bool		m_bTracing;
	CTraceInfo	m_TraceInfo;
	CLine		m_Trace;
	
	IGameGUILabel *m_piSTFps;

	IGameWindow	  *m_piGRFilePanel;
	IGameWindow	  *m_piGROptionsPanel;

	IGameGUIButton *m_piBTShowOptionsPanel;
	IGameGUIButton *m_piBTShowFilePanel;

	IGameGUIList   *m_piLSNodes;
	IGameGUIList   *m_piLSPolygons;
	
	// Options

	IGameGUIButton *m_piBTOptionsTextures;
	IGameGUIButton *m_piBTOptionsSolid;
	IGameGUIButton *m_piBTOptionsShowModel;
	
	// File
	IGameWindow	   *m_piGRFile;
	IGameGUIButton *m_piBTFileOpen;
	IGameGUIButton *m_piBTFileExit;

	void ProcessInput(double dTimeFraction,double dRealTimeFraction);
	void ProcessKey(unsigned short nKey,double dTimeFraction,double dRealTimeFraction);

	void ProcessFileOpen();
	void ProcessFileExit();
	
	void Reset();
	
	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("FPS",m_piSTFps);
		CHILD_MAP_ENTRY_EX("File",m_piBTShowFilePanel,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Options",m_piBTShowOptionsPanel,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("OptionsPanel",m_piGROptionsPanel);
		CHILD_MAP_ENTRY_EX("OptionShowTextures",m_piBTOptionsTextures,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionSolid",m_piBTOptionsSolid,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionShowModel",m_piBTOptionsShowModel,IGameGUIButtonEvents);
		
		CHILD_MAP_ENTRY_EX("NodeList",m_piLSNodes,IGameGUIListEvents);
		CHILD_MAP_ENTRY_EX("PolygonList",m_piLSPolygons,IGameGUIListEvents);
				
		CHILD_MAP_ENTRY("FilePanel",m_piGRFile);
		CHILD_MAP_ENTRY_EX("FileOpen",m_piBTFileOpen,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FileExit",m_piBTFileExit,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	BEGIN_PROP_MAP(CBSPDebuggerMainWindow)
		PROP_CLASS_CHAIN(CGameWindowBase)
		PROP_VALUE_FLAGS(m_d3DFontSize,"3DFontSize",0,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP();
public:
	
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

	void UpdateVisiblePanels();
	void UpdateNodeList();
	void UpdatePolygonList();
	void UpdateSelectedPolygon();
	void UpdateSelectedNode();
	void ShowPropertiesOf(ISystemObject *piObject);
		
	void UpdateCaption();
	void CenterCamera(EBSPDebuggerView eView=eBSPDebuggerView_Perspective);
	void SetupRenderOptions(IGenericRender *piRender,IGenericCamera *piCamera);

	void OnSelectionChanged(IGameGUIList *piControl,int nElement,std::string sElement);
	void OnSelectionDoubleCliked(IGameGUIList *piControl,int nElement,std::string sElement);

	// IBSPDebuggerPropertyPanelEvents
	void OnObjectChanged(IBSPDebuggerPropertyPanel *piPanel,ISystemObject *piObject);
	void OnObjectRemoved(IBSPDebuggerPropertyPanel *piPanel,ISystemObject *piObject);

	CBSPDebuggerMainWindow(void);
	~CBSPDebuggerMainWindow(void);
};


