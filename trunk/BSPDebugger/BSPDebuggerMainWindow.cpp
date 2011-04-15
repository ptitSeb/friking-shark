#include "./stdafx.h"
#include "resource.h"
#include "GameRunTimeLib.h"
#include "GameGUILib.h"
#include "BSPDebuggerMainWindow.h"
#include "../GameRunTime/GameRunTime.h"

CBSPDebuggerMainWindow::CBSPDebuggerMainWindow(void)
{
	m_d3DFontSize=0;
	
	m_bTracing=false;
	m_bShowOptionsPanel=false;
	m_bShowFilePanel=false;
	m_bShowModel=true;
	
	m_bTextures=1;
	m_bSolid=1;
	m_bShowModel=1;
	
	m_pBSP=NULL;
	m_pCurrentBSPNode=NULL;
		
	for(unsigned int x=0;x<ePropertyPanel_Count;x++){m_ppiPropertyPanels[x]=NULL;}
	
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

void CBSPDebuggerMainWindow::OnDraw(IGenericRender *piRender)
{
	if(!m_FrameManager.m_piFrameManager)
	{
		return;
	}
	
	m_Camera.m_piCamera->SetAspectRatio(m_rRealRect.w/m_rRealRect.h);

	m_FrameManager.m_piFrameManager->ProcessFrame();
	
	ProcessInput(m_FrameManager.m_piFrameManager->GetTimeFraction(),m_FrameManager.m_piFrameManager->GetRealTimeFraction());
	
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
	
	if(m_pCurrentBSPNode && m_pCurrentBSPNode->m_pDrawNode)
	{
		std::map<CPolygon	*,int>::iterator i;
		for(i=m_pCurrentBSPNode->m_pDrawNode->m_mPolygons.begin();i!=m_pCurrentBSPNode->m_pDrawNode->m_mPolygons.end();i++)
		{
			CPolygon *pPolygon=i->first;
			if(pPolygon->m_nVertexes>1)
			{
				piRender->RenderLineStrip(pPolygon->m_nVertexes,pPolygon->m_pVertexes,i->second?CVector(1,0,0):CVector(0,1,0),0xFFFF);
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
	piRender->EndStagedRendering();
	
	if(m_pCurrentBSPNode && m_pCurrentBSPNode->m_pDrawNode)
	{
		m_Render.m_piRender->PushOptions();
		m_Render.m_piRender->ActivateSolid();
		m_Render.m_piRender->SetColor(CVector(0,0,1),1);
		m_Render.m_piRender->RenderPolygon(m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_nVertexes,m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_pVertexes,NULL);
		CVector *pInverted=new CVector[m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_nVertexes];
		for(int y=0,x=m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_nVertexes-1;x>=0;x--,y++)
		{
			pInverted[y]=m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_pVertexes[x];
		}
		m_Render.m_piRender->RenderPolygon(m_pCurrentBSPNode->m_pDrawNode->m_pNodePolygon->m_nVertexes,pInverted,NULL);
		delete [] pInverted;
		m_Render.m_piRender->PopOptions();
	
	}
	if(m_TraceInfo.m_bTraceHit)
	{
		m_Render.m_piRender->RenderPoint(m_TraceInfo.m_vTracePos,10,CVector(1,1,0),1);
		m_Render.m_piRender->RenderLine(m_Trace.m_Points[0],m_TraceInfo.m_vTracePos,CVector(1,1,0),0x8888);
	}
	

	m_Render.m_piRender->PopOptions();
	m_Render.m_piRender->PopState();

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
				m_pBSP=BSPFromPolygonVector(NULL,1,&vGeometry,CONTENT_NODE,&m_vDrawNodes,false);
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
	if(m_piBTOptionsShowModel==piControl){m_bSolid=!m_bShowModel;}
	
	
	UpdateVisiblePanels();
}

void CBSPDebuggerMainWindow::UpdateCaption()
{
	if(m_Viewport.m_piViewport)
	{
		std::string sCaption="BSP Debugger";
		if(m_sModelName!=""){sCaption+=" - "+m_sModelName;}
		m_Viewport.m_piViewport->SetCaption(sCaption);
	}
}

void CBSPDebuggerMainWindow::OnCharacter( int nKey,bool *pbProcessed )
{
	if     (nKey=='T' || nKey=='t'){m_bTextures=!m_bTextures;*pbProcessed=true;}
	else if(nKey=='M' || nKey=='m'){m_bShowModel=!m_bShowModel;*pbProcessed=true;}
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
			m_piLSNodes->AddElement("Outside");
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
			m_piLSNodes->AddElement("Inside");
			m_piLSNodes->SetSelectedElement(nElem+1);
			m_vNodeStack.push_back(m_pCurrentBSPNode);
		}
		UpdatePolygonList();
	}	
//	else if(nKey=='E' || nKey=='e'){ShowPropertiesOf(m_EntityType.m_piObject);*pbProcessed=true;}
	/*	if(nKey=='I' && m_pEntity){m_pEntity->GetPhysicInfo()->vAngles.c[PITCH]+=5;if(m_pEntity->GetPhysicInfo()->vAngles.c[PITCH]>360){m_pEntity->GetPhysicInfo()->vAngles.c[PITCH]=90;}}
	 i f*(nKey=='K' && m_pEntity){m_pEntity->GetPhysicInfo()->vAngles.c[PITCH]-=5;if(m_pEntity->GetPhysicInfo()->vAngles.c[PITCH]<-0){m_pEntity->GetPhysicInfo()->vAngles.c[PITCH]=-0;}}
	 if(nKey=='J' && m_pEntity){m_pEntity->GetPhysicInfo()->vAngles.c[YAW]+=5;if(m_pEntity->GetPhysicInfo()->vAngles.c[YAW]>360){m_pEntity->GetPhysicInfo()->vAngles.c[YAW]-=360;}}
	 if(nKey=='L' && m_pEntity){m_pEntity->GetPhysicInfo()->vAngles.c[YAW]-=5;if(m_pEntity->GetPhysicInfo()->vAngles.c[YAW]<0){m_pEntity->GetPhysicInfo()->vAngles.c[YAW]+=360;}}
	 if(nKey=='N' && m_pEntity){m_pEntity->GetPhysicInfo()->vAngles.c[ROLL]+=5;if(m_pEntity->GetPhysicInfo()->vAngles.c[ROLL]>360){m_pEntity->GetPhysicInfo()->vAngles.c[ROLL]-=360;}}
	 if(nKey=='M' && m_pEntity){m_pEntity->GetPhysicInfo()->vAngles.c[ROLL]-=5;if(m_pEntity->GetPhysicInfo()->vAngles.c[ROLL]<0){m_pEntity->GetPhysicInfo()->vAngles.c[ROLL]+=360;}}
	 */	
}

void CBSPDebuggerMainWindow::OnKeyDown(int nKey,bool *pbProcessed)
{
	
	if(nKey==GK_F3){ProcessFileOpen();UpdateVisiblePanels();*pbProcessed=true;}
	else if(nKey==GK_PAUSE){m_FrameManager.m_piFrameManager->TogglePauseOnNextFrame();*pbProcessed=true;}
	else if(nKey==GK_HOME){CenterCamera();*pbProcessed=true;}
	else if(nKey==GK_NUMPAD5){CenterCamera(eBSPDebuggerView_Top);*pbProcessed=true;}
	else if(nKey==GK_NUMPAD4){CenterCamera(eBSPDebuggerView_Left);*pbProcessed=true;}
	else if(nKey==GK_NUMPAD8){CenterCamera(eBSPDebuggerView_Front);*pbProcessed=true;}
	else if(nKey==GK_NUMPAD2){CenterCamera(eBSPDebuggerView_Back);*pbProcessed=true;}
	else if(nKey==GK_NUMPAD6){CenterCamera(eBSPDebuggerView_Right);*pbProcessed=true;}
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

void CBSPDebuggerMainWindow::OnMouseDown( int nButton,double dx,double dy )
{
	m_Trace=GetMouseRay(dx,dy,10000.0,m_Camera.m_piCamera);
	if(m_pBSP)
	{
		m_vNodeStack.clear();
		m_TraceInfo=m_pBSP->GetTrace(m_Trace.m_Points[0],m_Trace.m_Points[1],m_Trace.m_Points[0],m_Trace.m_Points[1],&m_vNodeStack);
		UpdateNodeList();
	}
	if(DetectDrag(dx,dy))
	{
		m_bTracing=true;
	}
}

void CBSPDebuggerMainWindow::OnMouseMove( double x,double y )
{
	if(m_bTracing)
	{
		m_Trace=GetMouseRay(x,y,10000.0,m_Camera.m_piCamera);
		if(m_pBSP)
		{
			m_vNodeStack.clear();
			m_TraceInfo=m_pBSP->GetTrace(m_Trace.m_Points[0],m_Trace.m_Points[1],m_Trace.m_Points[0],m_Trace.m_Points[1],&m_vNodeStack);
			UpdateNodeList();
		}
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
		if(!pParent){m_piLSNodes->AddElement("Root");}
		else        {m_piLSNodes->AddElement(pParent->pChild[0]==pNode?"Outside":"Inside");}
		if(pNode==m_pCurrentBSPNode){m_piLSNodes->SetSelectedElement(x);}
	}
	UpdatePolygonList();
}

void CBSPDebuggerMainWindow::UpdatePolygonList()
{/*
	if(m_piLSNodes==NULL){return;}
	if(m_piLSPolygons==NULL){return;}
	m_piLSPolygons->Clear();
	m_vPolygons.clear();
	
	int nNode=m_piLSNodes->GetSelectedElement();
	
	if(nNode>=0 && m_EntityType.m_piEntityType)
	{
		for(unsigned int x=0;x<m_EntityType.m_piEntityTypeDesign->GetNodePolygonCount(nNode);x++)
		{
			IPolygonType *piPolygon=NULL;
			m_EntityType.m_piEntityTypeDesign->GetNodePolygon(nNode,x,&piPolygon);
			
			CPolygonDesignTypeWrapper wrapper;
			if(piPolygon && wrapper.Attach(piPolygon))
			{
				m_vPolygons.push_back(wrapper);
				char sName[128];
				sprintf(sName,"Polygon %d",x);
				m_piLSPolygons->AddElement(sName);
			}
			REL(piPolygon);
		}
	}
	if(m_piLSPolygons->GetElementCount())
	{
		m_piLSPolygons->SetSelectedElement(0);
	}
	UpdateSelectedPolygon();*/
}

void CBSPDebuggerMainWindow::ShowPropertiesOf(ISystemObject *piObject)
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
}

void CBSPDebuggerMainWindow::OnSelectionChanged(IGameGUIList *piControl,int nElement,std::string sElement)
{
	if(piControl==m_piLSNodes)
	{
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


void CBSPDebuggerMainWindow::OnObjectChanged(IBSPDebuggerPropertyPanel *piPanel,ISystemObject *piObject)
{
}

void CBSPDebuggerMainWindow::OnObjectRemoved(IBSPDebuggerPropertyPanel *piPanel,ISystemObject *piObject)
{
}
