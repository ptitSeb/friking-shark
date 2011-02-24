#include "./stdafx.h"
#include "GameGUIManager.h"
#include "GameMainWindow.h"
#include "GameGUILib.h"

CGameGUIManager::CGameGUIManager()
{
	m_bShowMouseCursor=true;
	m_piMainWindow=NULL;
	m_piFocusedWindow=NULL;
	m_piMouseCaptureWindow=NULL;
	m_bScreenSettingsChanged=false;
}

CGameGUIManager::~CGameGUIManager()
{
}

bool CGameGUIManager::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
	bool bResult=CSystemObjectBase::Init(sClass,sName,piSystem);
	if(!bResult){return false;}

	m_Render.Attach("GameGUI","Render");
	m_Viewport.Attach("GameGUI","Viewport");

	m_piMainWindow=new CGameMainWindow(this);
	m_piMainWindow->InitWindow(NULL,true);
	m_piFocusedWindow=m_piMainWindow;

	if(m_Viewport.m_piViewport)
	{
	  m_Viewport.m_piViewport->SetCallBack(this);
	}
	bool bOk=m_Viewport.m_piViewport->Create(0,0,800,600,false);
	m_Viewport.m_piViewport->ShowMouseCursor(false);
	return bOk;
}

void CGameGUIManager::Destroy()
{
	if(m_Viewport.m_piViewport)
	{
	  if(m_bScreenSettingsChanged)
	  {
		//  m_Viewport.m_piViewport->SetCurrentVideoMode(&m_sOldVideoMode);
		  m_bScreenSettingsChanged=false;
	  }
	  
	    m_Viewport.m_piViewport->ShowMouseCursor(true);
		m_Viewport.m_piViewport->SetCallBack(NULL);
	}
	if(m_piMainWindow)
	{
		m_piMainWindow->DestroyWindow();
	}

	while(m_vPopups.size())
	{
		IGameWindow *piWindow=m_vPopups[0];
		ADD(piWindow);
		piWindow->DestroyWindow();
		REL(piWindow);
	}

	REL(m_piMainWindow);
	m_Viewport.Detach();
	m_Render.Detach();
	m_piFocusedWindow=NULL;

	CSystemObjectBase::Destroy();
}

// Popup management.

void CGameGUIManager::GetPopups(std::vector<IGameWindow *> *pvPopups)
{
	std::vector<IGameWindow*>::iterator i;

	for(i=m_vPopups.begin();i!=m_vPopups.end();i++)
	{
		IGameWindow *piTempWindow=*i;
		ADD(piTempWindow);
		pvPopups->push_back(piTempWindow);
	}
}

void CGameGUIManager::AddPopup(IGameWindow *piWindow)
{
	ADD(piWindow);
	m_vPopups.push_back(piWindow);
}

void CGameGUIManager::RemovePopup(IGameWindow *piWindow)
{
	std::vector<IGameWindow*>::iterator i;

	for(i=m_vPopups.begin();i!=m_vPopups.end();i++)
	{
		IGameWindow *piTempWindow=*i;
		if(piTempWindow==piWindow)
		{
			m_vPopups.erase(i);
			REL(piTempWindow);
			break;
		}
	}
}

void CGameGUIManager::EnumeratePopups(IGameWindowEnumerationCallback *piCallback)
{
	bool bStopEnumerating=false;
	std::vector<IGameWindow*>::iterator i;

	for(i=m_vPopups.begin();i!=m_vPopups.end() && !bStopEnumerating;i++)
	{
		piCallback->ProcessEnumeratedWindow(*i,&bStopEnumerating);
	}
}

void CGameGUIManager::EnterGUILoop()
{
    if(m_Viewport.m_piViewport)
	{
	  m_Viewport.m_piViewport->EnterLoop();
	}
}

void CGameGUIManager::ExitGUILoop()
{
    if(m_Viewport.m_piViewport)
	{
	  m_Viewport.m_piViewport->ExitLoop();
	}
}

IGameWindow *CGameGUIManager::GetWindowFromPos(IGameWindow *piWindow,SGamePos *pPosition,bool bOnlyActive)
{
	SGameRect windowRect;
	std::vector<IGameWindow*> vChildren;
	std::vector<IGameWindow*>::reverse_iterator iChildren;

	if(piWindow->IsVisible()==false)
	{
		return NULL;
	}

	if(bOnlyActive && !piWindow->IsActive())
	{
		return NULL;
	}


	piWindow->GetRealRect(&windowRect);

	if(windowRect.Contains(*pPosition))
	{
		piWindow->GetChildren(&vChildren);

		IGameWindow *piChildFound=NULL;
		for(iChildren=vChildren.rbegin();iChildren!=vChildren.rend();iChildren++)
		{
			IGameWindow *piChild=*iChildren;
			if(!piChildFound)
			{
				IGameWindow *piTempWindow=GetWindowFromPos(piChild,pPosition,bOnlyActive);
				if(piTempWindow)
				{
					piChildFound=piTempWindow;
				}
			}
			REL(piChild);
		}
		if(piChildFound)
		{
			return piChildFound;
		}
		ADD(piWindow);
		return piWindow;
	}

	return NULL;
}

IGameWindow *CGameGUIManager::GetWindowFromPos(SGamePos *pPosition,bool bOnlyActive)
{
	if(m_vPopups.size())
	{
		// El popup mas alto siempre bloquea la interaccion con los demas.
		return GetWindowFromPos(m_vPopups[m_vPopups.size()-1],pPosition,bOnlyActive);
	}
	return GetWindowFromPos(m_piMainWindow,pPosition,bOnlyActive);
}

void CGameGUIManager::RenderWindow(IGenericRender *piRender,IGameWindow *piWindow)
{
	SGameRect									rRect;
	std::vector<IGameWindow*> vChildren;
	std::vector<IGameWindow*>::iterator iChildren;
	IGameWindow *piParent=piWindow->GetParent();
	
	piWindow->GetRealRect(&rRect);
	m_Render.m_piRender->SetOrthographicProjection(rRect.w,rRect.h);
	m_Render.m_piRender->SetViewport(rRect.x,rRect.y,rRect.w,rRect.h);
	m_Render.m_piRender->SetCamera(CVector(rRect.w*0.5,rRect.h*0.5,200),90,0,0);
	m_Render.m_piRender->DeactivateDepth();

	if(!piWindow->IsPopup() && piParent)
	{
		SGameRect clipRect=rRect;
		SGameRect	rParentRect;

		piRender->ActivateClipping(true);

		piParent->GetRealRect(&rParentRect);
		clipRect.ClipToRect(&rParentRect);
		piRender->SetClipRect(clipRect.x,clipRect.y,clipRect.w,clipRect.h);
	}


	piWindow->OnDrawBackground(piRender);
	piWindow->OnDraw(piRender);

	if(!piWindow->IsPopup() && piParent)
	{
		piRender->ActivateClipping(false);
	}

	piWindow->GetChildren(&vChildren);

	for(iChildren=vChildren.begin();iChildren!=vChildren.end();iChildren++)
	{
		IGameWindow *piChild=*iChildren;
		if(piChild->IsVisible())
		{
			RenderWindow(piRender,piChild);
		}
		REL(piChild);
	}
	REL(piParent);
}

void CGameGUIManager::OnRender()
{
	SGameSize size;
	GetWindowSize(&size);

	RenderWindow(m_Render.m_piRender,m_piMainWindow);
	for(unsigned x=0;x<m_vPopups.size();x++)
	{
		if(m_vPopups[x]->IsVisible())
		{
			RenderWindow(m_Render.m_piRender,m_vPopups[x]);
		}
	}
	if(m_bShowMouseCursor)
	{
		m_Render.m_piRender->SetOrthographicProjection(size.w,size.h);
		m_Render.m_piRender->SetViewport(0,0,size.w,size.h);
		m_Render.m_piRender->SetCamera(CVector(size.w*0.5,size.h*0.5,200),90,0,0);

		int px=0,py=0;
		if(m_Viewport.m_piViewport){m_Viewport.m_piViewport->GetCursorPos(&px,&py);}

		bool bDrawed=false;
		SGamePos pos;
		pos.x=px;
		pos.y=size.h-py;
		IGameWindow *piWindow=m_piMouseCaptureWindow?ADD(m_piMouseCaptureWindow):GetWindowFromPos(m_piMainWindow,&pos,true);
		while(!bDrawed && piWindow)
		{
			piWindow->OnDrawMouseCursor(pos,m_Render.m_piRender,&bDrawed);
			IGameWindow *piParent=piWindow->GetParent();
			REL(piWindow);
			piWindow=piParent;
		}
		REL(piWindow);
	}
}

void CGameGUIManager::GetWindowSize(SGameSize *pSize)
{
	unsigned w=0,h=0;
	m_Viewport.m_piViewport->GetClientSize(&w,&h);
	pSize->w=w;
	pSize->h=h;
}

void CGameGUIManager::GetMousePosition(IGameWindow *piWindow,SGamePos *pPos)
{
	SGameSize size;
	SGamePos  pos;

	int px=0,py=0;
	if(m_Viewport.m_piViewport){m_Viewport.m_piViewport->GetCursorPos(&px,&py);}
	GetWindowSize(&size);

	pPos->x=0;
	pPos->y=0;

	SGameRect rWindowRect;
	if(piWindow){piWindow->GetRealRect(&rWindowRect);}
	if(size.w)
	{
		pPos->x=px-rWindowRect.x;
	}
	if(size.h)
	{
		pPos->y=size.h-py-rWindowRect.y;
	}
}

void CGameGUIManager::SetMousePosition(IGameWindow *piWindow,SGamePos &pos)
{
	SGameSize size;
	SGameRect rWindowRect;
	if(piWindow){piWindow->GetRealRect(&rWindowRect);}
	GetWindowSize(&size);

	int pointX=0,pointY=0;
	if(size.w){pointX=(int)pos.x;}
	if(size.h){pointY=(int)(size.h-pos.y);}

	if(m_Viewport.m_piViewport){m_Viewport.m_piViewport->SetCursorPos(pointX,pointY);}
}

bool CGameGUIManager::IsKeyDown(int nKey)
{
	if(m_Viewport.m_piViewport==NULL || !m_Viewport.m_piViewport->IsActiveWindow())
	{
		return false;
	}
	return m_Viewport.m_piViewport->IsKeyDown(nKey);
}

bool CGameGUIManager::IsMouseDown(int nMouseButton)
{
	if(m_Viewport.m_piViewport==NULL || !m_Viewport.m_piViewport->IsActiveWindow())
	{
		return false;
	}
	return m_Viewport.m_piViewport->IsKeyDown(nMouseButton);
}

void CGameGUIManager::SetFocus(IGameWindow *piWindow)
{
	if(piWindow!=m_piFocusedWindow)
	{
		if(m_piFocusedWindow){m_piFocusedWindow->OnKillFocus(piWindow);}
		m_piFocusedWindow=piWindow;
		if(piWindow){piWindow->OnSetFocus();}
	}
}

IGameWindow *CGameGUIManager::GetFocus()
{
	return m_piFocusedWindow;
}

void CGameGUIManager::SetMouseCapture(IGameWindow *piWindow)
{
	ReleaseMouseCapture();
	if(piWindow!=m_piMouseCaptureWindow)
	{
		if(m_piMouseCaptureWindow)
		{
			m_piMouseCaptureWindow->OnReleaseMouseCapture();
			REL(m_piMouseCaptureWindow);
		}
		ADD(piWindow);
		m_piMouseCaptureWindow=piWindow;
		if(m_piMouseCaptureWindow)
		{
			m_Viewport.m_piViewport->SetMouseCapture();
		}
	}
}

void CGameGUIManager::ReleaseMouseCapture()
{
	if(m_piMouseCaptureWindow!=NULL)
	{
		m_piMouseCaptureWindow->OnReleaseMouseCapture();
		REL(m_piMouseCaptureWindow);
	}
	m_Viewport.m_piViewport->ReleaseMouseCapture();
}

IGameWindow *CGameGUIManager::GetMouseCapture()
{
	return ADD(m_piMouseCaptureWindow);
}

void CGameGUIManager::ShowMouseCursor(bool bShow)
{
	m_bShowMouseCursor=bShow;
}

bool CGameGUIManager::IsMouseCursorVisible()
{
	return m_bShowMouseCursor;
}

void CGameGUIManager::RedrawAll()
{
}

IGameWindow *CGameGUIManager::GetMainWindow()
{
	ADD(m_piMainWindow);
	return m_piMainWindow;
}

void CGameGUIManager::ProcessMouseActivation(IGameWindow *piWindow)
{
	if(!piWindow || !piWindow->IsActive()){return;}

	if(m_piFocusedWindow!=piWindow)
	{
		if(m_piFocusedWindow)
		{
			m_piFocusedWindow->OnKillFocus(piWindow);
		}

		m_piFocusedWindow=piWindow;
		if(m_piFocusedWindow)
		{
			m_piFocusedWindow->OnSetFocus();
		}
	}

	if(m_piFocusedWindow)
	{
		m_piFocusedWindow->BringToFront();
	}
}

void CGameGUIManager::OnLButtonDown(WORD wKeyState,unsigned x,unsigned y)
{
	SGameSize size;
	SGamePos pos;
	GetWindowSize(&size);
	pos.x=x;
	pos.y=size.h-(double)y;
	IGameWindow *piWindow=m_piMouseCaptureWindow && m_piMouseCaptureWindow->IsActive()?ADD(m_piMouseCaptureWindow):GetWindowFromPos(&pos,true);
	if(piWindow)
	{
		SGameRect rRect;
		piWindow->GetRealRect(&rRect);
		ProcessMouseActivation(piWindow);
		piWindow->OnMouseDown(MK_LBUTTON,pos.x-rRect.x,pos.y-rRect.y);
	}
	REL(piWindow);
}

void CGameGUIManager::OnRButtonDown(WORD wKeyState,unsigned x,unsigned y)
{
	SGameSize size;
	SGamePos pos;
	GetWindowSize(&size);
	pos.x=x;
	pos.y=size.h-(double)y;
	IGameWindow *piWindow=m_piMouseCaptureWindow?ADD(m_piMouseCaptureWindow):GetWindowFromPos(&pos,true);
	if(piWindow)
	{
		SGameRect rRect;
		piWindow->GetRealRect(&rRect);
		ProcessMouseActivation(piWindow);
		piWindow->OnMouseDown(MK_RBUTTON,pos.x-rRect.x,pos.y-rRect.y);
	}
	REL(piWindow);
}

void CGameGUIManager::OnLButtonUp(WORD wKeyState,unsigned x,unsigned y)
{
	SGameSize size;
	SGamePos pos;
	GetWindowSize(&size);
	pos.x=x;
	pos.y=size.h-(double)y;
	IGameWindow *piWindow=m_piMouseCaptureWindow?ADD(m_piMouseCaptureWindow):GetWindowFromPos(&pos,true);
	if(piWindow)
	{
		SGameRect rRect;
		piWindow->GetRealRect(&rRect);
		piWindow->OnMouseUp(MK_LBUTTON,pos.x-rRect.x,pos.y-rRect.y);
	}
	REL(piWindow);
}

void CGameGUIManager::OnRButtonUp(WORD wKeyState,unsigned x,unsigned y)
{
	SGameSize size;
	SGamePos pos;
	GetWindowSize(&size);
	pos.x=x;
	pos.y=size.h-(double)y;
	IGameWindow *piWindow=m_piMouseCaptureWindow?ADD(m_piMouseCaptureWindow):GetWindowFromPos(&pos,true);
	if(piWindow)
	{
		SGameRect rRect;
		piWindow->GetRealRect(&rRect);
		piWindow->OnMouseUp(MK_RBUTTON,pos.x-rRect.x,pos.y-rRect.y);
	}
	REL(piWindow);
}

void CGameGUIManager::OnMouseMove(unsigned x,unsigned y)
{
	SGameSize size;
	SGamePos pos;
	GetWindowSize(&size);
	pos.x=x;
	pos.y=size.h-(double)y;
	IGameWindow *piWindow=m_piMouseCaptureWindow?ADD(m_piMouseCaptureWindow):GetWindowFromPos(&pos,true);
	if(piWindow)
	{
		SGameRect rRect;
		piWindow->GetRealRect(&rRect);
		piWindow->OnMouseMove(pos.x-rRect.x,pos.y-rRect.y);
	}
	REL(piWindow);
}

void CGameGUIManager::OnCharacter(WORD wCharacter)
{
	if(m_piFocusedWindow)
	{
		bool				 bProcessed=false;
		IGameWindow *piWindow=m_piFocusedWindow;
		ADD(piWindow);

		while(piWindow)
		{
			piWindow->OnCharacter(wCharacter,&bProcessed);
			if(piWindow->IsPopup() || bProcessed)
			{
				REL(piWindow);
				break;
			}
			else
			{
				IGameWindow *piParent=piWindow->GetParent();
				REL(piWindow);
				piWindow=piParent;
			}
		}
	}
}

void CGameGUIManager::OnKeyDown(WORD wKeyState)
{
	if(wKeyState==VK_RETURN && IsKeyDown(VK_LMENU) && IsKeyDown(VK_LCONTROL))
	{
		m_sScreenProperties.bFullScreen=!m_sScreenProperties.bFullScreen;
		UpdateScreenPlacement();
		return;
	}
	if(m_piFocusedWindow)
	{
		bool				 bProcessed=false;
		IGameWindow *piWindow=m_piFocusedWindow;
		ADD(piWindow);

		while(piWindow)
		{
			piWindow->OnKeyDown(wKeyState,&bProcessed);
			if(piWindow->IsPopup() || bProcessed)
			{
				REL(piWindow);
				break;
			}
			else
			{
				IGameWindow *piParent=piWindow->GetParent();
				REL(piWindow);
				piWindow=piParent;
			}
		}
	}
}

void CGameGUIManager::OnKeyUp(WORD wKeyState)
{
	if(m_piFocusedWindow)
	{
		bool				 bProcessed=false;
		IGameWindow *piWindow=m_piFocusedWindow;
		ADD(piWindow);

		while(piWindow)
		{
			piWindow->OnKeyUp(wKeyState,&bProcessed);
			if(piWindow->IsPopup() || bProcessed)
			{
				REL(piWindow);
				break;
			}
			else
			{
				IGameWindow *piParent=piWindow->GetParent();
				REL(piWindow);
				piWindow=piParent;
			}
		}
	}
}

void CGameGUIManager::OnSize(unsigned w,unsigned h)
{
  	if(!m_sScreenProperties.bFullScreen)
	{
	  if(m_sScreenProperties.eWindowReferenceSystem==eGameGUIReferenceSystem_Absolute)
	  {
		m_sScreenProperties.rWindowRect.w=w;
		m_sScreenProperties.rWindowRect.h=h;
	  }
	  else
	  {
		SVideoMode sVideoMode;
		m_Viewport.m_piViewport->GetCurrentVideoMode(&sVideoMode);
		m_sScreenProperties.rWindowRect.w=(double)w/(double)sVideoMode.w;
		m_sScreenProperties.rWindowRect.h=(double)h/(double)sVideoMode.h;
	  }
	}
	OnMove();
}

void CGameGUIManager::OnMove()
{
	if(m_piMainWindow){m_piMainWindow->UpdateRealRect();}
	
	for(unsigned int x=0;x<m_vPopups.size();x++)
	{
		IGameWindow *piWindow=m_vPopups[x];
		piWindow->UpdateRealRect();
	}
}

void CGameGUIManager::GetScreenProperties(SGameScreenProperties *pProperties)
{
	*pProperties=m_sScreenProperties;
}

void CGameGUIManager::SetScreenProperties(SGameScreenProperties *pProperties)
{
	m_sScreenProperties=*pProperties;
	UpdateScreenPlacement();
}

void CGameGUIManager::UpdateScreenPlacement()
{

	if(m_sScreenProperties.bFullScreen)
	{
		m_Viewport.m_piViewport->SetFullScreen((unsigned int)m_sScreenProperties.sFullScreenResolution.w,(unsigned int)m_sScreenProperties.sFullScreenResolution.h,(unsigned int)m_sScreenProperties.dFullScreenRefreshBitsPerPixel,(unsigned int)m_sScreenProperties.dFullScreenRefreshRate);
	}
	else
	{
		SGameRect rFinalRect;
		if(m_sScreenProperties.eWindowReferenceSystem==eGameGUIReferenceSystem_Absolute)
		{
			rFinalRect=m_sScreenProperties.rWindowRect;
		}
		else
		{
			rFinalRect.x=m_sScreenProperties.rWindowRect.x*m_sOldVideoMode.w;
			rFinalRect.y=m_sScreenProperties.rWindowRect.y*m_sOldVideoMode.h;
			rFinalRect.w=m_sScreenProperties.rWindowRect.w*m_sOldVideoMode.w;
			rFinalRect.h=m_sScreenProperties.rWindowRect.h*m_sOldVideoMode.h;
		}
		if(m_sScreenProperties.bWindowCentered)
		{
			SGameRect rScreenRect;
			rScreenRect.w=m_sOldVideoMode.w;
			rScreenRect.h=m_sOldVideoMode.h;

			rFinalRect.CenterOnRect(&rScreenRect);
		}
		m_Viewport.m_piViewport->SetWindowed((unsigned)rFinalRect.x,(unsigned)rFinalRect.y,(unsigned)rFinalRect.w,(unsigned)rFinalRect.h);
	}
// 	m_Viewport.m_piViewport->SetVSync(m_sScreenProperties.bVerticalSync);
}

bool CGameGUIManager::Unserialize(ISystemPersistencyNode *piNode)
{
	bool bOk=CSystemObjectBase::Unserialize(piNode);

	if(bOk && m_Viewport.m_piViewport)
	{
		// Esto es un apaño de un problema en la persistencia por el que no se pueden especificar valores por defecto
		// para SGameRect ni SGameSize, ademas no hacerlo supondria un problema para obtener los valores por defecto para
		// los bpp y el refreshrate.

		m_Viewport.m_piViewport->GetCurrentVideoMode(&m_sOldVideoMode);

		if(m_sScreenProperties.rWindowRect.x==0 && m_sScreenProperties.rWindowRect.y==0 && m_sScreenProperties.rWindowRect.w==0 && m_sScreenProperties.rWindowRect.h==0)
		{
			m_sScreenProperties.eWindowReferenceSystem=eGameGUIReferenceSystem_Relative;
			m_sScreenProperties.rWindowRect.x=0.125;
			m_sScreenProperties.rWindowRect.y=0.125;
			m_sScreenProperties.rWindowRect.w=0.75;
			m_sScreenProperties.rWindowRect.h=0.75;
		}
		if(m_sScreenProperties.sFullScreenResolution.w==0 || m_sScreenProperties.sFullScreenResolution.h==0)
		{
			m_sScreenProperties.sFullScreenResolution.w=m_sOldVideoMode.w;
			m_sScreenProperties.sFullScreenResolution.h=m_sOldVideoMode.h;
		}
		if(m_sScreenProperties.dFullScreenRefreshRate==0)
		{
			m_sScreenProperties.dFullScreenRefreshRate=m_sOldVideoMode.rate;
		}
		if(m_sScreenProperties.dFullScreenRefreshBitsPerPixel==0)
		{
			m_sScreenProperties.dFullScreenRefreshBitsPerPixel=m_sOldVideoMode.bpp;
		}

		UpdateScreenPlacement();
	}
	return bOk;
}

bool CGameGUIManager::DetectDrag(double dx,double dy)
{
	if(!m_Viewport.m_piViewport){return false;}
	return m_Viewport.m_piViewport->DetectDrag(dx,dy);
}
	