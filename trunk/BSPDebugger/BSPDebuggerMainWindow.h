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


BEGIN_STRUCT_PROPS(CLine)
	PROP(m_Points[0],"Point0")
	PROP(m_Points[1],"Point1")
END_STRUCT_PROPS()

struct SBSPDebuggerSession
{
	std::string sModelName;
	CLine sTrace;
	
	BEGIN_PROP_MAP(SBSPDebuggerSession)
		PROP(sModelName,"Model")
		PROP(sTrace,"Trace")
	END_PROP_MAP()
};


class CBSPDebuggerMainWindow: public CGameWindowBase, public IGameGUIButtonEvents, public IGameGUIListEvents
{
public:
	CConfigFile				m_GUIConfigFile;
	CViewportWrapper		m_Viewport;
	CRenderWrapper			m_Render;
	CGenericCameraWrapper	m_Camera;
	CFrameManagerWrapper	m_FrameManager;

		
	CModelWrapper		 	 m_Model;
	std::string				 m_sModelName;
	std::string				 m_sSessionName;
	
	double					 m_d3DFontSize;
		
	bool		m_bSolid;
	bool		m_bTextures;
	bool		m_bShowModel;
	bool		m_bShowNormals;
	double		m_dNormalSize;
	
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

	IGameGUIButton *m_piBTRunTrace;
	IGameGUIButton *m_piBTRunContent;
	IGameGUIList   *m_piLSNodes;
	IGameGUIList   *m_piLSPolygons;
	IGameGUIList   *m_piLSDiscardedPolygons;
	
	// Options

	IGameGUIButton *m_piBTOptionsTextures;
	IGameGUIButton *m_piBTOptionsSolid;
	IGameGUIButton *m_piBTOptionsShowModel;
	IGameGUIButton *m_piBTOptionsShowNormals;
	
	// File
	IGameWindow	   *m_piGRFile;
	IGameGUIButton *m_piBTFileOpen;
	IGameGUIButton *m_piBTFileNewSession;
	IGameGUIButton *m_piBTFileOpenSession;
	IGameGUIButton *m_piBTFileSaveSession;
	IGameGUIButton *m_piBTFileSaveSessionAs;
	IGameGUIButton *m_piBTFileExit;

	void RenderNormal(IGenericRender * piRender,CPolygon * pPolygon,CVector vColor);

	void ProcessInput(double dTimeFraction,double dRealTimeFraction);
	void ProcessKey(unsigned short nKey,double dTimeFraction,double dRealTimeFraction);

	void ProcessFileNewSession();
	void ProcessFileOpen();
	void ProcessFileOpenSession();
	void ProcessFileSaveSession();
	void ProcessFileSaveSessionAs();
	void ProcessFileExit();
	
	void RunTrace();
	void RunContent();
	
	
	void Reset();
	
	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("FPS",m_piSTFps);
		CHILD_MAP_ENTRY_EX("File",m_piBTShowFilePanel,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Options",m_piBTShowOptionsPanel,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("OptionsPanel",m_piGROptionsPanel);
		CHILD_MAP_ENTRY_EX("OptionShowTextures",m_piBTOptionsTextures,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionSolid",m_piBTOptionsSolid,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionShowModel",m_piBTOptionsShowModel,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("OptionShowNormals",m_piBTOptionsShowNormals,IGameGUIButtonEvents);
		
		CHILD_MAP_ENTRY_EX("RunTrace",m_piBTRunTrace,IGameGUIListEvents);
		CHILD_MAP_ENTRY_EX("RunContent",m_piBTRunContent,IGameGUIListEvents);
		CHILD_MAP_ENTRY_EX("NodeList",m_piLSNodes,IGameGUIListEvents);
		CHILD_MAP_ENTRY_EX("PolygonList",m_piLSPolygons,IGameGUIListEvents);
		CHILD_MAP_ENTRY_EX("DiscardedPolygonList",m_piLSDiscardedPolygons,IGameGUIListEvents);
				
		CHILD_MAP_ENTRY("FilePanel",m_piGRFile);
		CHILD_MAP_ENTRY_EX("FileOpen",m_piBTFileOpen,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FileNewSession",m_piBTFileNewSession,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FileOpenSession",m_piBTFileOpenSession,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FileSaveSession",m_piBTFileSaveSession,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("FileSaveSessionAs",m_piBTFileSaveSessionAs,IGameGUIButtonEvents);
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
		
	void UpdateCaption();
	void CenterCamera(EBSPDebuggerView eView=eBSPDebuggerView_Perspective);
	void SetupRenderOptions(IGenericRender *piRender,IGenericCamera *piCamera);

	void OnSelectionChanged(IGameGUIList *piControl,int nElement,std::string sElement);
	void OnSelectionDoubleCliked(IGameGUIList *piControl,int nElement,std::string sElement);

	CBSPDebuggerMainWindow(void);
	~CBSPDebuggerMainWindow(void);
};
