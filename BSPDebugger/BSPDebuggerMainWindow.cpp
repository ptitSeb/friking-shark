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
#include "BSPDebuggerMainWindow.h"
#include "../GameRunTime/GameRunTime.h"

CBSPDebuggerMainWindow::CBSPDebuggerMainWindow(void)
{
	m_d3DFontSize=0;
	m_dNormalSize=2.0;
	
	m_bTracing=false;
	m_bShowOptionsPanel=false;
	m_bShowFilePanel=false;
	m_bShowModel=true;
	m_bShowNormals=false;
	
	m_bTextures=1;
	m_bSolid=1;
	m_bShowModel=1;
	
	m_pBSP=NULL;
	m_pCurrentBSPNode=NULL;
		
	InitializeChildren();
}

CBSPDebuggerMainWindow::~CBSPDebuggerMainWindow(void)
{
}

bool CBSPDebuggerMainWindow::InitWindow(IGameWindow *piParent,bool bPopup)
{
	bool bOk=CGameWindowBase::InitWindow(piParent,bPopup);
	if(!bOk){return false;}
	
	m_Viewport.Attach("GameGUI","Viewport");
	m_Render.Attach("GameGUI","Render");
	m_Viewport.m_piViewport->SetCaption("BSP Debugger");
	m_Camera.Create("GameGUI","Camera","");
	
	SGameRect sRect;
	sRect.x=0;
	sRect.y=0;
	sRect.w=1;
	sRect.h=1;
	m_eReferenceSystem=eGameGUIReferenceSystem_Relative;
	SetRect(&sRect);

	m_piGUIManager->SetFocus(this);
	m_FrameManager.Attach("GameGUI","FrameManager");
	UpdateVisiblePanels();
	UpdateNodeList();
	return bOk;
}

void CBSPDebuggerMainWindow::DestroyWindow()
{
	Reset();
	UpdateNodeList();
	
	m_FrameManager.Detach();
	m_Viewport.Detach();

	UNSUBSCRIBE_FROM_ALL();
	CGameWindowBase::DestroyWindow();
}

void CBSPDebuggerMainWindow::Reset()
{
	m_Model.Detach();

	delete m_pBSP;
	m_pBSP=NULL;
	m_pCurrentBSPNode=NULL;
	m_vNodeStack.clear();
	for(unsigned int x=0;x<m_vDrawNodes.size();x++)
	{
		delete m_vDrawNodes[x];
	}
	m_vDrawNodes.clear();
	m_bShowFilePanel=false;
	m_bShowOptionsPanel=false;
	m_sModelName="";

	UpdateCaption();
	UpdateNodeList();
}

void CBSPDebuggerMainWindow::ProcessInput(double dTimeFraction,double dRealTimeFraction)
{
	if(!m_piGUIManager->HasFocus(this)){return;}
	
	if(m_piGUIManager->IsKeyDown(GK_UP) || m_piGUIManager->IsKeyDown('W')){ProcessKey(KEY_FORWARD,dTimeFraction,dRealTimeFraction);}
	if(m_piGUIManager->IsKeyDown(GK_DOWN) || m_piGUIManager->IsKeyDown('S')){ProcessKey(KEY_BACK,dTimeFraction,dRealTimeFraction);}
	if(m_piGUIManager->IsKeyDown(GK_LEFT) || m_piGUIManager->IsKeyDown('A')){ProcessKey(KEY_LEFT,dTimeFraction,dRealTimeFraction);}
	if(m_piGUIManager->IsKeyDown(GK_RIGHT) || m_piGUIManager->IsKeyDown('D')){ProcessKey(KEY_RIGHT,dTimeFraction,dRealTimeFraction);}
	
	if(m_piGUIManager->IsKeyDown('R')){ProcessKey(KEY_UP,dTimeFraction,dRealTimeFraction);}
	if(m_piGUIManager->IsKeyDown('F')){ProcessKey(KEY_DOWN,dTimeFraction,dRealTimeFraction);}
}

void CBSPDebuggerMainWindow::ProcessKey(unsigned short nKey,double dTimeFraction,double dRealTimeFraction)
{
	double dCameraRotationSpeed=90.0;
	double dCameraForwardSpeed=20.0*dRealTimeFraction*3.0;
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
void CBSPDebuggerMainWindow::SetupRenderOptions(IGenericRender *piRender,IGenericCamera *piCamera)
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

void CBSPDebuggerMainWindow::RenderNormal(IGenericRender * piRender,CPolygon * pPolygon,CVector vColor) 
{
	CVector vMiddle;
	for(unsigned int x=0;x<pPolygon->m_nVertexes;x++){vMiddle+=pPolygon->m_pVertexes[x];}
	vMiddle/=(double)pPolygon->m_nVertexes;
	piRender->RenderLine(vMiddle,vMiddle+pPolygon->m_Plane*m_dNormalSize,vColor,0xFFFF);
	piRender->RenderPoint(vMiddle+pPolygon->m_Plane*m_dNormalSize,5,vColor,1);
}

void CBSPDebuggerMainWindow::OnDraw(IGenericRender *piRender)
{
	if(!m_FrameManager.m_piFrameManager)
	{
		return;
	}
	
	m_Camera.m_piCamera->SetAspectRatio(m_rRealRect.w/m_rRealRect.h);

	m_FrameManager.m_piFrameManager->ProcessFrame();
	
	ProcessInput(m_FrameManager.m_piFrameManager->GetTimeFraction(),m_FrameManager.m_piFrameManager->GetRealTimeFraction());
	
	piRender->ActivateDepth();
	piRender->PushOptions();
	piRender->PushState();

	m_bTextures?piRender->EnableTextures():piRender->DisableTextures();
	m_bSolid?piRender->EnableSolid():piRender->DisableSolid();
	piRender->EnableBlending();
	piRender->EnableShadows();
	piRender->DisableLighting();
	piRender->DisableHeightFog();
	
	SetupRenderOptions(piRender,m_Camera.m_piCamera);
	
	//piRender->StartStagedRendering();
	
	if(m_pCurrentBSPNode && m_pCurrentBSPNode->m_pDrawNode)
	{
		int nIndex=0;
		std::map<CPolygon	*,int>::iterator i;
		for(i=m_pCurrentBSPNode->m_pDrawNode->m_mPolygons.begin();i!=m_pCurrentBSPNode->m_pDrawNode->m_mPolygons.end();i++,nIndex++)
		{
			CPolygon *pPolygon=i->first;
			if(pPolygon->m_nVertexes>1)
			{
				CVector vColor=i->second?CVector(1,0,0):CVector(0,1,0);
				if(nIndex==m_piLSPolygons->GetSelectedElement())
				{
					piRender->PushState();
					piRender->ActivateSolid();
					piRender->SetColor(vColor,1);
					piRender->RenderPolygon(pPolygon->m_nVertexes,pPolygon->m_pVertexes,NULL);
					piRender->DeactivateSolid();
					piRender->PopState();
				}
				else
				{
					piRender->RenderLineStrip(pPolygon->m_nVertexes,pPolygon->m_pVertexes,vColor,0xFFFF);
				}
				if(m_bShowNormals){RenderNormal(piRender,pPolygon,vColor*0.5);}
			}
		}
		nIndex=0;
		for(i=m_pCurrentBSPNode->m_pDrawNode->m_mDiscardedPolygons.begin();i!=m_pCurrentBSPNode->m_pDrawNode->m_mDiscardedPolygons.end();i++,nIndex++)
		{
			CPolygon *pPolygon=i->first;
			if(pPolygon->m_nVertexes>1)
			{
				CVector vColor=CVector(1,0,1);
				if(nIndex==m_piLSDiscardedPolygons->GetSelectedElement())
				{
					piRender->PushState();
					piRender->ActivateSolid();
					piRender->SetColor(vColor,1);
					piRender->RenderPolygon(pPolygon->m_nVertexes,pPolygon->m_pVertexes,NULL);
					piRender->DeactivateSolid();
					piRender->PopState();
				}
				else
				{
					piRender->RenderLineStrip(pPolygon->m_nVertexes,pPolygon->m_pVertexes,vColor,0xFFFF);
				}
			}
		}
		
		CBSPNode *pNode=m_pCurrentBSPNode;
		CBSPNode *pParent=m_pCurrentBSPNode->pParent;
		double dColor=0.8;
		while(pParent)
		{
			int nSide=pParent->pChild[0]==pNode?1:0;
			for(i=pParent->m_pDrawNode->m_mPolygons.begin();i!=pParent->m_pDrawNode->m_mPolygons.end();i++)
			{
				CPolygon *pPolygon=i->first;
				if(i->second==nSide && pPolygon->m_nVertexes>1)
				{
					piRender->RenderLineStrip(pPolygon->m_nVertexes,pPolygon->m_pVertexes,CVector(dColor,dColor,dColor),0xFFFF);
				}
			}
			dColor*=0.6;
			
			pNode=pParent;
			pParent=pParent->pParent;	
		}
	}
	if(m_bShowModel && m_Model.m_piModel)
	{
		piRender->RenderModel(Origin,Origin,m_Model.m_piModel);
	}
	//piRender->EndStagedRendering();
	
	if(m_pCurrentBSPNode && m_pCurrentBSPNode->m_pDrawNode)
	{
		CVector *pInverted=new CVector[m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_nVertexes];
		for(int y=0,x=m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_nVertexes-1;x>=0;x--,y++){pInverted[y]=m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_pVertexes[x];}

		CVector vColor=CVector(0,0,1);
		piRender->PushOptions();
		piRender->ActivateSolid();
		piRender->SetColor(vColor,1);
		piRender->RenderPolygon(m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_nVertexes,m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_pVertexes,NULL);
		piRender->RenderPolygon(m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_nVertexes,pInverted,NULL);
		if(m_bShowNormals){RenderNormal(piRender,m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon,vColor*0.5);}
		piRender->PopOptions();

		delete [] pInverted;
	
	}
	if(m_TraceInfo.m_bTraceHit)
	{
		piRender->RenderPoint(m_TraceInfo.m_vTracePos,10,CVector(1,1,0),1);
		piRender->RenderLine(m_Trace.m_Points[0],m_TraceInfo.m_vTracePos,CVector(1,1,0),0x8888);
		piRender->DeactivateDepth();
		
		if(m_pCurrentBSPNode && m_TraceInfo.m_bTraceHit)
		{
			double dSide=m_pCurrentBSPNode->plane.GetSide(m_TraceInfo.m_vTracePos);
			CVector vPlaneProj=m_TraceInfo.m_vTracePos-m_pCurrentBSPNode->plane*dSide;
			
			piRender->RenderPoint(vPlaneProj,10,dSide>=0?CVector(0,0,1):CVector(1,0,0),1);
			piRender->RenderLine(vPlaneProj,m_TraceInfo.m_vTracePos,dSide>=0?CVector(0,0,1):CVector(1,0,0),0x8888);
			
			double dFontSize=0;
			IGenericFont *piFont=NULL;
			GetFont(&piFont,&dFontSize);
			if(m_d3DFontSize>0){dFontSize=m_d3DFontSize;}
			if(piFont && dFontSize>0)
			{
				char sDescr[1024];
				sprintf(sDescr,"%s: %f",dSide>=0?"O":"I",dSide);
				piRender->SetColor(CVector(1,1,1),1);
				piFont->RenderText(piRender,dFontSize,vPlaneProj,sDescr);
			}
			REL(piFont);
		}
		piRender->ActivateDepth();
	}
	
	piRender->PopOptions();
	piRender->PopState();

	if(m_piSTFps)
	{
		char A[200];
		sprintf(A,"Fps: %.02f",m_FrameManager.m_piFrameManager->GetCurrentFps());
		
		m_piSTFps->SetText(A);
	}
}

void CBSPDebuggerMainWindow::ProcessFileOpen()
{
	std::string sBaseModel="./Models/";

	if(OpenFileDialog("Select Model...",".ase;.ASE",&sBaseModel))
	{
		Reset();
		
		if(m_Model.Create("GameGUI","Model",sBaseModel))
		{
			if(m_Model.m_piModel->Load(sBaseModel))
			{
				m_sModelName=sBaseModel;
				std::vector<CPolygon*> vGeometry;
				m_Model.m_piModel->GetGeometry(&vGeometry);
				m_pBSP=BSPFromPolygonVector(NULL,1,&vGeometry,CONTENT_NODE,&m_vDrawNodes,true);
				m_pCurrentBSPNode=m_pBSP;
				if(m_pBSP){m_vNodeStack.push_back(m_pCurrentBSPNode);}
				
				UpdateCaption();
				UpdateNodeList();
				CenterCamera();
			}
			else
			{
				Reset();
				MessageDialog("Failed to load model","BSP Debugger",eMessageDialogType_Error);
			}
		}
		else
		{
			MessageDialog("Failed to create model instance","BSP Debugger",eMessageDialogType_Error);
		}
		
	}
}



void CBSPDebuggerMainWindow::ProcessFileExit()
{
	if(ConfirmDialog("Do you really want to exit?","BSP Debugger",eMessageDialogType_Question))
	{
		m_piGUIManager->ExitGUILoop();
	}
}

void CBSPDebuggerMainWindow::OnButtonClicked(IGameGUIButton *piControl)
{
	if(m_piBTFileNewSession==piControl)
	{
		ProcessFileNewSession();
		m_bShowFilePanel=false;
	}
	if(m_piBTFileOpenSession==piControl)
	{
		ProcessFileOpenSession();
		m_bShowFilePanel=false;
	}
	if(m_piBTFileSaveSession==piControl)
	{
		ProcessFileSaveSession();
		m_bShowFilePanel=false;
	}
	if(m_piBTFileSaveSessionAs==piControl)
	{
		ProcessFileSaveSessionAs();
		m_bShowFilePanel=false;
	}
	if(m_piBTFileOpen==piControl)
	{
		ProcessFileOpen();
		m_bShowFilePanel=false;
	}
	if(m_piBTFileExit==piControl)
	{
		ProcessFileExit();
		m_bShowFilePanel=false;
	}
	if(m_piBTShowOptionsPanel==piControl){m_bShowOptionsPanel=!m_bShowOptionsPanel;}
	if(m_piBTShowFilePanel==piControl){m_bShowFilePanel=!m_bShowFilePanel;}
	if(m_piBTOptionsTextures==piControl){m_bTextures=!m_bTextures;}
	if(m_piBTOptionsSolid==piControl){m_bSolid=!m_bSolid;}
	if(m_piBTOptionsShowModel==piControl){m_bShowModel=!m_bShowModel;}
	if(m_piBTOptionsShowNormals==piControl){m_bShowNormals=!m_bShowNormals;}
	if(m_piBTRunTrace==piControl){RunTrace();}
	if(m_piBTRunContent==piControl){RunContent();}
	UpdateVisiblePanels();
}

void CBSPDebuggerMainWindow::RunTrace()
{
	m_vNodeStack.clear();
	if(m_pBSP){m_TraceInfo=m_pBSP->GetTrace(m_Trace.m_Points[0],m_Trace.m_Points[1],m_Trace.m_Points[0],m_Trace.m_Points[1],&m_vNodeStack);}
	UpdateNodeList();
}

void CBSPDebuggerMainWindow::RunContent()
{
	m_vNodeStack.clear();
	if(m_pBSP){m_pBSP->GetContent(m_TraceInfo.m_vTracePos,&m_vNodeStack);}
	UpdateNodeList();
}

void CBSPDebuggerMainWindow::UpdateCaption()
{
	if(m_Viewport.m_piViewport)
	{
		std::string sCaption="BSP Debugger";
		if(m_sSessionName!=""){sCaption+=" - "+m_sSessionName;}
		if(m_sModelName!=""){sCaption+=" - "+m_sModelName;}
		m_Viewport.m_piViewport->SetCaption(sCaption);
	}
}

void CBSPDebuggerMainWindow::OnCharacter( int nKey,bool *pbProcessed )
{
	if     (nKey=='T' || nKey=='t'){m_bTextures=!m_bTextures;*pbProcessed=true;}
	else if(nKey=='M' || nKey=='m'){m_bShowModel=!m_bShowModel;*pbProcessed=true;}
	else if(nKey=='N' || nKey=='n'){m_bShowNormals=!m_bShowNormals;*pbProcessed=true;}
	else if(nKey=='O' || nKey=='o'){m_bSolid=!m_bSolid;*pbProcessed=true;}
	
	
	if(nKey=='I' || nKey=='i')
	{
		int nElem=m_piLSNodes->GetSelectedElement();
		if(nElem!=-1 && nElem>0)
		{
			m_piLSNodes->SetSelectedElement(nElem-1);
			m_pCurrentBSPNode=m_vNodeStack[nElem-1];
		}
		UpdatePolygonList();
	}
	if(nKey=='K' || nKey=='k')
	{
		int nElem=m_piLSNodes->GetSelectedElement();
		if(nElem!=-1 && nElem<(int)(m_piLSNodes->GetElementCount()-1))
		{
			m_piLSNodes->SetSelectedElement(nElem+1);
			m_pCurrentBSPNode=m_vNodeStack[nElem+1];
		}
		
		UpdatePolygonList();
	}
	if(nKey=='L' || nKey=='l')
	{
		int nElem=m_piLSNodes->GetSelectedElement();
		if(nElem!=-1 && m_pCurrentBSPNode && m_pCurrentBSPNode->pChild[0]&& m_pCurrentBSPNode->pChild[0]->m_pDrawNode)
		{
			m_pCurrentBSPNode=m_pCurrentBSPNode->pChild[0];
			while((int)m_piLSNodes->GetElementCount()>nElem+1){m_piLSNodes->RemoveElement(nElem);}
			char sName[256];
			sprintf(sName,"O: %d: %f,%f,%f:%f",m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_nVertexes,m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_Plane.c[0],m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_Plane.c[1],m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_Plane.c[2],m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_Plane.d);
			m_piLSNodes->AddElement(sName);
			m_piLSNodes->SetSelectedElement(nElem+1);
			m_vNodeStack.push_back(m_pCurrentBSPNode);
		}
		UpdatePolygonList();
	}
	if(nKey=='J' || nKey=='j')
	{
		int nElem=m_piLSNodes->GetSelectedElement();
		if(nElem!=-1 && m_pCurrentBSPNode && m_pCurrentBSPNode->pChild[1]&& m_pCurrentBSPNode->pChild[1]->m_pDrawNode)
		{
			m_pCurrentBSPNode=m_pCurrentBSPNode->pChild[1];
			while((int)m_piLSNodes->GetElementCount()>nElem+1){m_piLSNodes->RemoveElement(nElem);}
			char sName[256];
			sprintf(sName,"I: %d: %f,%f,%f:%f",m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_nVertexes,m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_Plane.c[0],m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_Plane.c[1],m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_Plane.c[2],m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_Plane.d);
			m_piLSNodes->AddElement(sName);
			m_piLSNodes->SetSelectedElement(nElem+1);
			m_vNodeStack.push_back(m_pCurrentBSPNode);
		}
		UpdatePolygonList();
	}
}

void CBSPDebuggerMainWindow::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_F1){ProcessFileOpen();UpdateVisiblePanels();*pbProcessed=true;}
	else if(nKey==GK_F2){ProcessFileSaveSession();UpdateVisiblePanels();*pbProcessed=true;}
	else if(nKey==GK_F3){ProcessFileOpenSession();UpdateVisiblePanels();*pbProcessed=true;}
	else if(nKey==GK_F5){RunTrace();}
	else if(nKey==GK_F6){RunContent();}
	else if(nKey==GK_PAUSE){m_FrameManager.m_piFrameManager->TogglePauseOnNextFrame();*pbProcessed=true;}
	else if(nKey==GK_HOME){CenterCamera();*pbProcessed=true;}
	else if(nKey==GK_NUMPAD5){CenterCamera(eBSPDebuggerView_Top);*pbProcessed=true;}
	else if(nKey==GK_NUMPAD4){CenterCamera(eBSPDebuggerView_Left);*pbProcessed=true;}
	else if(nKey==GK_NUMPAD8){CenterCamera(eBSPDebuggerView_Front);*pbProcessed=true;}
	else if(nKey==GK_NUMPAD2){CenterCamera(eBSPDebuggerView_Back);*pbProcessed=true;}
	else if(nKey==GK_NUMPAD6){CenterCamera(eBSPDebuggerView_Right);*pbProcessed=true;}
	else if(nKey==GK_ESCAPE)
	{
		ProcessFileExit();
		*pbProcessed=true;
	}
}

void CBSPDebuggerMainWindow::OnMouseDown( int nButton,double dx,double dy )
{
	if(DetectDrag(dx,dy))
	{
		m_Trace=GetMouseRay(dx,dy,10000.0,m_Camera.m_piCamera);
		RunTrace();
		m_bTracing=true;
	}
}

void CBSPDebuggerMainWindow::OnMouseMove( double x,double y )
{
	if(m_bTracing)
	{
		m_Trace=GetMouseRay(x,y,10000.0,m_Camera.m_piCamera);
		RunTrace();
	}
}

void CBSPDebuggerMainWindow::OnMouseUp( int nButton,double x,double y )
{
	m_bTracing=false;
}

void CBSPDebuggerMainWindow::UpdateVisiblePanels()
{
	if(m_piGROptionsPanel){m_piGROptionsPanel->Show(m_bShowOptionsPanel);}
	if(m_piGRFile){m_piGRFile->Show(m_bShowFilePanel);}
}

void CBSPDebuggerMainWindow::UpdateNodeList()
{
	if(m_piLSNodes==NULL){return;}
	m_piLSNodes->Clear();
	for(unsigned int x=0;x<m_vNodeStack.size();x++)
	{
		CBSPNode *pNode=m_vNodeStack[x];
		CBSPNode *pParent=pNode->pParent;
		if(!pParent)
		{
			char sName[256];
			sprintf(sName,"R: %d: %f,%f,%f:%f",pNode->m_pDrawNode->m_pNodePolygon->m_nVertexes,pNode->m_pDrawNode->m_pNodePolygon->m_Plane.c[0],pNode->m_pDrawNode->m_pNodePolygon->m_Plane.c[1],pNode->m_pDrawNode->m_pNodePolygon->m_Plane.c[2],pNode->m_pDrawNode->m_pNodePolygon->m_Plane.d);
			m_piLSNodes->AddElement(sName);
		}
		else        
		{
			char sName[256];
			sprintf(sName,"%s: %d: %f,%f,%f:%f",pParent->pChild[1]==pNode?"I":"O",pNode->m_pDrawNode->m_pNodePolygon->m_nVertexes,pNode->m_pDrawNode->m_pNodePolygon->m_Plane.c[0],pNode->m_pDrawNode->m_pNodePolygon->m_Plane.c[1],pNode->m_pDrawNode->m_pNodePolygon->m_Plane.c[2],pNode->m_pDrawNode->m_pNodePolygon->m_Plane.d);
			m_piLSNodes->AddElement(sName);
		}
		if(pNode==m_pCurrentBSPNode){m_piLSNodes->SetSelectedElement(x);}
	}
	UpdatePolygonList();
}

void CBSPDebuggerMainWindow::UpdatePolygonList()
{
	m_piLSPolygons->Clear();
	m_piLSDiscardedPolygons->Clear();
	if(m_pCurrentBSPNode==NULL){return;}

	std::map<CPolygon	*,int>::iterator i;
	for(i=m_pCurrentBSPNode->m_pDrawNode->m_mPolygons.begin();i!=m_pCurrentBSPNode->m_pDrawNode->m_mPolygons.end();i++)
	{
		CPolygon *pPolygon=i->first;
		char sName[256];
		sprintf(sName,"%s: %d: %f,%f,%f:%f",i->second?"I":"O",pPolygon->m_nVertexes,pPolygon->m_Plane.c[0],pPolygon->m_Plane.c[1],pPolygon->m_Plane.c[2],pPolygon->m_Plane.d);
		m_piLSPolygons->AddElement(sName);
	}
	for(i=m_pCurrentBSPNode->m_pDrawNode->m_mDiscardedPolygons.begin();i!=m_pCurrentBSPNode->m_pDrawNode->m_mDiscardedPolygons.end();i++)
	{
		CPolygon *pPolygon=i->first;
		const char *pDiscardCode="UNK";

		if     (i->second==BSP_DISCARD_COPLANAR){pDiscardCode="COP";}
		else if(i->second==BSP_DISCARD_INVALID_PLANE){pDiscardCode="INV";}
		else if(i->second==BSP_DISCARD_SMALL_SURFACE){pDiscardCode="SUR";}
		else if(i->second==BSP_DISCARD_VERTEX_COUNT){pDiscardCode="VER";}

		char sName[256];
		sprintf(sName,"%s: %d: %f,%f,%f:%f",pDiscardCode,pPolygon->m_nVertexes,pPolygon->m_Plane.c[0],pPolygon->m_Plane.c[1],pPolygon->m_Plane.c[2],pPolygon->m_Plane.d);
		m_piLSDiscardedPolygons->AddElement(sName);
	}
}

void CBSPDebuggerMainWindow::OnSelectionChanged(IGameGUIList *piControl,int nElement,std::string sElement)
{
	if(piControl==m_piLSNodes)
	{
		m_pCurrentBSPNode=(nElement==-1)?NULL:m_vNodeStack[nElement];
		UpdatePolygonList();
	}
}

void CBSPDebuggerMainWindow::OnSelectionDoubleCliked(IGameGUIList *piControl,int nElement,std::string sElement){}
void CBSPDebuggerMainWindow::OnWantFocus(bool *pbWant){*pbWant=true;}

void CBSPDebuggerMainWindow::CenterCamera(EBSPDebuggerView eView)
{
	if(m_Model.m_piModel==NULL){return;}

	bool bCenter=false;
	CVector vCenter,vSize;
	CVector vMins,vMaxs;
	m_Model.m_piModel->GetFrameBBox(0,0,&vMins,&vMaxs);
	bCenter=true;
	vCenter+=(vMaxs+vMins)*0.5;
	vSize=(vMaxs-vMins);

	double dNearPlane=0,dFarPlane=0;
	m_Camera.m_piCamera->GetClippingPlanes(dNearPlane,dFarPlane);
	double dAspect=m_Camera.m_piCamera->GetAspectRatio();
	
	CVector vPos;
	if(eView==eBSPDebuggerView_Perspective)
	{
		vPos.c[0]=vCenter.c[0];
		vPos.c[1]=vCenter.c[1]+fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5))*(vSize.c[0]*0.5))+dNearPlane;
		vPos.c[2]=vCenter.c[2]+fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5))*(vSize.c[0]*0.5))*m_Camera.m_piCamera->GetAspectRatio()+dNearPlane;
	}
	else if(eView==eBSPDebuggerView_Left)
	{
		double dDistance=vSize.c[0]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))*dAspect);
		dDistance=std::max(dDistance,vSize.c[1]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))));
		dDistance=std::max(dDistance,dNearPlane);
		vPos.c[0]=vCenter.c[0];
		vPos.c[1]=vCenter.c[1];
		vPos.c[2]=vMins.c[2]-dDistance;
	}
	else if(eView==eBSPDebuggerView_Top)
	{
		double dDistance=vSize.c[2]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))*dAspect);
		dDistance=std::max(dDistance,vSize.c[0]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))));
		dDistance=std::max(dDistance,dNearPlane);
		vPos.c[0]=vCenter.c[0];
		vPos.c[1]=vMaxs.c[1]+dDistance;
		vPos.c[2]=vCenter.c[2];
	}
	else if(eView==eBSPDebuggerView_Bottom)
	{
		double dDistance=vSize.c[2]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))*dAspect);
		dDistance=std::max(dDistance,vSize.c[0]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))));
		dDistance=std::max(dDistance,dNearPlane);
		vPos.c[0]=vCenter.c[0];
		vPos.c[1]=vMins.c[1]-dDistance;
		vPos.c[2]=vCenter.c[2];
	}
	else if(eView==eBSPDebuggerView_Right)
	{
		double dDistance=vSize.c[0]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))*dAspect);
		dDistance=std::max(dDistance,vSize.c[1]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))));
		dDistance=std::max(dDistance,dNearPlane);
		vPos.c[0]=vCenter.c[0];
		vPos.c[1]=vCenter.c[1];
		vPos.c[2]=vMaxs.c[2]+dDistance;
	}
	else if(eView==eBSPDebuggerView_Front)
	{
		double dDistance=vSize.c[2]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))*dAspect);
		dDistance=std::max(dDistance,vSize.c[1]*0.5/(fabs(tan(DegreesToRadians(m_Camera.m_piCamera->GetViewAngle()*0.5)))));
		dDistance=std::max(dDistance,dNearPlane);
		vPos.c[0]=vMaxs.c[0]+dDistance;
		vPos.c[1]=vCenter.c[1];
		vPos.c[2]=vCenter.c[2];
	}
	else if(eView==eBSPDebuggerView_Back)
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
	m_Camera.m_piCamera->SetPosition(vPos);
	m_Camera.m_piCamera->SetAngles(vAngles);
}

void CBSPDebuggerMainWindow::ProcessFileNewSession()
{
	m_sSessionName="";
	Reset();
}

void CBSPDebuggerMainWindow::ProcessFileOpenSession()
{
	std::string sSessionName="./";
	if(OpenFileDialog("Select Session...",".bspd;.BSPD",&sSessionName))
	{
		Reset();
		CConfigFile file;
		if(file.Open(sSessionName))
		{
			SBSPDebuggerSession sessionConfig;
			sessionConfig.PersistencyLoad(file.GetRoot());
			if(m_Model.Create("GameGUI","Model",sessionConfig.sModelName))
			{
				if(m_Model.m_piModel->Load(sessionConfig.sModelName))
				{
					m_sSessionName=sSessionName;
					m_sModelName=sessionConfig.sModelName;
					m_Trace=sessionConfig.sTrace;

					std::vector<CPolygon*> vGeometry;
					m_Model.m_piModel->GetGeometry(&vGeometry);
					m_pBSP=BSPFromPolygonVector(NULL,1,&vGeometry,CONTENT_NODE,&m_vDrawNodes,true);
					m_pCurrentBSPNode=m_pBSP;
					if(m_pBSP)
					{
						m_TraceInfo=m_pBSP->GetTrace(m_Trace.m_Points[0],m_Trace.m_Points[1],m_Trace.m_Points[0],m_Trace.m_Points[1],&m_vNodeStack);
					}

					UpdateCaption();
					UpdateNodeList();
					CenterCamera();
				}
				else
				{
					Reset();
					MessageDialog("Failed to load model","BSP Debugger",eMessageDialogType_Error);
				}
			}
			else
			{
				MessageDialog("Failed to create model instance","BSP Debugger",eMessageDialogType_Error);
			}
		}
		else
		{
			MessageDialog("Failed to open session file","BSP Debugger",eMessageDialogType_Error);
		}
	}
}

void CBSPDebuggerMainWindow::ProcessFileSaveSession()
{
	std::string sSessionName=m_sSessionName;
	if(m_Model.m_piModel==NULL)
	{
		MessageDialog("Cannot save session without loading a model","BSP Debugger",eMessageDialogType_Error);
		return;
	}
	bool bSave=true;
	if(sSessionName.length()==0)
	{
		bSave=SaveFileDialog("Save session...",".bspd;.BSPD",&sSessionName,true);
	}
	if(bSave)
	{
		m_sSessionName=sSessionName;

		CConfigFile file;
		SBSPDebuggerSession sessionConfig;
		sessionConfig.sTrace=m_Trace;
		sessionConfig.sModelName=m_sModelName;
		sessionConfig.PersistencySave(file.GetRoot());

		file.Save(sSessionName);
		UpdateCaption();
	}
}

void CBSPDebuggerMainWindow::ProcessFileSaveSessionAs()
{
	std::string sSessionName=m_sSessionName;
	if(m_Model.m_piModel==NULL)
	{
		MessageDialog("Cannot save session without loading a model","BSP Debugger",eMessageDialogType_Error);
		return;
	}

	if(SaveFileDialog("Save session as...",".bspd;.BSPD",&sSessionName,true))
	{
		m_sSessionName=sSessionName;

		CConfigFile file;
		SBSPDebuggerSession sessionConfig;
		sessionConfig.PersistencySave(file.GetRoot());

		file.Save(sSessionName);
		UpdateCaption();
	}
}
