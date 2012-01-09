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
#include "GameGUIManager.h"
#include "GameMainWindow.h"
#include "GameGUILib.h"

CGameGUIManager::CGameGUIManager()
{
	m_bSetup=false;
	m_bShowMouseCursor=true;
	m_piFocusedWindow=NULL;
	m_piMouseCaptureWindow=NULL;
	m_piRootWindow=NULL;
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
	
	if(m_Viewport.m_piViewport){m_Viewport.m_piViewport->SetCallBack(this);}
	
	return true;
}

void CGameGUIManager::Destroy()
{
	if(m_Viewport.m_piViewport)
	{
	    m_Viewport.m_piViewport->ShowMouseCursor(true);
		m_Viewport.m_piViewport->SetCallBack(NULL);
	}
	if(m_piRootWindow)
	{
		m_piRootWindow->DestroyWindow();
	}
	
	while(m_vPopups.size())
	{
		IGameWindow *piWindow=m_vPopups[0];
		ADD(piWindow);
		piWindow->DestroyWindow();
		REL(piWindow);
	}

	m_MainWindow.Detach();
	REL(m_piFocusedWindow);
	REL(m_piRootWindow);
	m_Viewport.Detach();
	m_Render.Detach();

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
		
		if(!bOnlyActive || piWindow->IsActive())
		{
		  ADD(piWindow);
		  return piWindow;
		}
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
	return GetWindowFromPos(m_piRootWindow,pPosition,bOnlyActive);
}

void CGameGUIManager::RenderWindow(IGenericRender *piRender,IGameWindow *piWindow)
{
	std::vector<IGameWindow*> vChildren;
	std::vector<IGameWindow*>::iterator iChildren;
	IGameWindow *piParent=piWindow->GetParent();
		
	piWindow->OnDrawBackground(piRender);
	piWindow->OnDraw(piRender);

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

void CGameGUIManager::RestoreViewport()
{
	if(m_Render.m_piRender==NULL){return;}
	
	SGameSize size;
	GetWindowSize(&size);
	
	m_Render.m_piRender->SetOrthographicProjection(size.w,size.h);
	m_Render.m_piRender->SetViewport(0,0,size.w,size.h);
	m_Render.m_piRender->SetCamera(CVector(size.w*0.5,size.h*0.5,200),90,0,0);	
}

void CGameGUIManager::OnRender()
{
	if(!m_bSetup){return;}

	SGameSize size;
	GetWindowSize(&size);
	
	m_Render.m_piRender->StartFrame();
	m_Render.m_piRender->ActivateBlending();
	m_Render.m_piRender->DeactivateDepth();
	
	RestoreViewport();

	RenderWindow(m_Render.m_piRender,m_piRootWindow);
	for(unsigned x=0;x<m_vPopups.size();x++)
	{
		if(m_vPopups[x]->IsVisible())
		{
			RenderWindow(m_Render.m_piRender,m_vPopups[x]);
		}
	}
	if(m_bShowMouseCursor)
	{
		int px=0,py=0;
		if(m_Viewport.m_piViewport){m_Viewport.m_piViewport->GetCursorPos(&px,&py);}

		bool bDrawed=false;
		SGamePos pos;
		pos.x=px;
		pos.y=size.h-py;
		IGameWindow *piWindow=m_piMouseCaptureWindow?ADD(m_piMouseCaptureWindow):GetWindowFromPos(m_piRootWindow,&pos,true);
		while(!bDrawed && piWindow)
		{
			piWindow->OnDrawMouseCursor(pos,m_Render.m_piRender,&bDrawed);
			IGameWindow *piParent=piWindow->GetParent();
			REL(piWindow);
			piWindow=piParent;
		}
		REL(piWindow);
	}
	m_Render.m_piRender->EndFrame();
}

void CGameGUIManager::GetWindowSize(SGameSize *pSize)
{
	unsigned w=0,h=0;
	if(m_Viewport.m_piViewport){m_Viewport.m_piViewport->GetSize(&w,&h);}
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

bool CGameGUIManager::GetKeyName(int nKey,std::string *psKey)
{
	if(m_Viewport.m_piViewport==NULL)
	{
		return false;
	}
	return m_Viewport.m_piViewport->GetKeyName(nKey,psKey);
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
		REL(m_piFocusedWindow);
		
		bool bWantFocus=true;
		if(piWindow){piWindow->OnWantFocus(&bWantFocus);}
		if(bWantFocus)
		{
		  m_piFocusedWindow=ADD(piWindow);
		  if(m_piFocusedWindow){m_piFocusedWindow->OnSetFocus();}
		}
	}
}

bool CGameGUIManager::HasFocus(IGameWindow *piWindow)
{
	return m_piFocusedWindow==piWindow;
}

IGameWindow *CGameGUIManager::GetFocusedWindow()
{
	return ADD(m_piFocusedWindow);
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

bool CGameGUIManager::HasMouseCapture(IGameWindow *piWindow)
{
	return m_piMouseCaptureWindow==piWindow;
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
	return ADD(m_piRootWindow);
}

void CGameGUIManager::ProcessMouseActivation(IGameWindow *piWindow)
{
	if(!piWindow || !piWindow->IsActive()){return;}

	if(m_piFocusedWindow!=piWindow)
	{
		bool bWantFocus=true;
		if(piWindow){piWindow->OnWantFocus(&bWantFocus);}
		if(bWantFocus)
		{
		  if(m_piFocusedWindow)
		  {
			  m_piFocusedWindow->OnKillFocus(piWindow);
		  }
		  REL(m_piFocusedWindow);
		  m_piFocusedWindow=ADD(piWindow);
		  
		  if(m_piFocusedWindow)
		  {
			  m_piFocusedWindow->OnSetFocus();
		  }
		}
	}

	if(m_piFocusedWindow)
	{
		m_piFocusedWindow->BringToFront();
	}
}

void CGameGUIManager::OnLButtonDown(unsigned x,unsigned y)
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
		piWindow->OnMouseDown(GK_LBUTTON,pos.x-rRect.x,pos.y-rRect.y);
	}
	REL(piWindow);
}

void CGameGUIManager::OnRButtonDown(unsigned x,unsigned y)
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
		piWindow->OnMouseDown(GK_RBUTTON,pos.x-rRect.x,pos.y-rRect.y);
	}
	REL(piWindow);
}

void CGameGUIManager::OnLButtonDoubleClick(unsigned x,unsigned y)
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
		piWindow->OnMouseDoubleClick(GK_LBUTTON,pos.x-rRect.x,pos.y-rRect.y);
	}
	REL(piWindow);
}

void CGameGUIManager::OnRButtonDoubleClick(unsigned x,unsigned y)
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
		piWindow->OnMouseDoubleClick(GK_RBUTTON,pos.x-rRect.x,pos.y-rRect.y);
	}
	REL(piWindow);
}

void CGameGUIManager::OnLButtonUp(unsigned x,unsigned y)
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
		piWindow->OnMouseUp(GK_LBUTTON,pos.x-rRect.x,pos.y-rRect.y);
	}
	REL(piWindow);
}

void CGameGUIManager::OnRButtonUp(unsigned x,unsigned y)
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
		piWindow->OnMouseUp(GK_RBUTTON,pos.x-rRect.x,pos.y-rRect.y);
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

void CGameGUIManager::OnMouseWheelUp(unsigned x,unsigned y)
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
		piWindow->OnMouseWheelUp(pos.x-rRect.x,pos.y-rRect.y);
	}
	REL(piWindow);
}


void CGameGUIManager::OnMouseWheelDown(unsigned x,unsigned y)
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
		piWindow->OnMouseWheelDown(pos.x-rRect.x,pos.y-rRect.y);
	}
	REL(piWindow);
}

void CGameGUIManager::OnCharacter(unsigned short wCharacter)
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

void CGameGUIManager::OnKeyDown(unsigned short wKey)
{
	if(wKey==GK_RETURN && IsKeyDown(GK_LMENU) && IsKeyDown(GK_LCONTROL))
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
			piWindow->OnKeyDown(wKey,&bProcessed);
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

void CGameGUIManager::OnKeyUp(unsigned short wKey)
{
	if(m_piFocusedWindow)
	{
		bool				 bProcessed=false;
		IGameWindow *piWindow=m_piFocusedWindow;
		ADD(piWindow);

		while(piWindow)
		{
			piWindow->OnKeyUp(wKey,&bProcessed);
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


	if(m_piRootWindow){m_piRootWindow->UpdateRealRect();}
	for(unsigned int x=0;x<m_vPopups.size();x++)
	{
		IGameWindow *piWindow=m_vPopups[x];
		piWindow->UpdateRealRect();
	}
}

void CGameGUIManager::OnMove(unsigned x,unsigned y)
{
	if(!m_sScreenProperties.bFullScreen)
	{
		if(m_sScreenProperties.eWindowReferenceSystem==eGameGUIReferenceSystem_Absolute)
		{
			m_sScreenProperties.rWindowRect.x=x;
			m_sScreenProperties.rWindowRect.y=y;
		}
		else
		{
			SVideoMode sVideoMode;
			m_Viewport.m_piViewport->GetCurrentVideoMode(&sVideoMode);
			m_sScreenProperties.rWindowRect.x=(double)x/(double)sVideoMode.w;
			m_sScreenProperties.rWindowRect.y=(double)y/(double)sVideoMode.h;
		}
	}

}

void CGameGUIManager::GetScreenProperties(SGameScreenProperties *pProperties)
{
	*pProperties=m_sScreenProperties;
}

void CGameGUIManager::SetScreenProperties(SGameScreenProperties *pProperties)
{
	m_sScreenProperties=*pProperties;
	if(m_bSetup){UpdateScreenPlacement();}
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
			rFinalRect.x=m_sScreenProperties.rWindowRect.x*m_sWindowedResolution.w;
			rFinalRect.y=m_sScreenProperties.rWindowRect.y*m_sWindowedResolution.h;
			rFinalRect.w=m_sScreenProperties.rWindowRect.w*m_sWindowedResolution.w;
			rFinalRect.h=m_sScreenProperties.rWindowRect.h*m_sWindowedResolution.h;
		}
		m_Viewport.m_piViewport->SetWindowed((unsigned)rFinalRect.x,(unsigned)rFinalRect.y,(unsigned)rFinalRect.w,(unsigned)rFinalRect.h);
	}
 	m_Viewport.m_piViewport->SetVSync(m_sScreenProperties.bVerticalSync);
}

bool CGameGUIManager::Setup()
{
	bool bOk=true;
	if(bOk && m_Viewport.m_piViewport)
	{
		SVideoMode sVideoMode;
		m_Viewport.m_piViewport->GetCurrentVideoMode(&sVideoMode);
		
		SGameRect rRelativeFullScreenRect;
		rRelativeFullScreenRect.x=((double)sVideoMode.fullscreenX)/(double)sVideoMode.w;
		rRelativeFullScreenRect.y=((double)sVideoMode.fullscreenY)/(double)sVideoMode.h;
		rRelativeFullScreenRect.w=((double)sVideoMode.fullscreenW)/(double)sVideoMode.w;
		rRelativeFullScreenRect.h=((double)sVideoMode.fullscreenH)/(double)sVideoMode.h;
		
		m_sWindowedResolution.w=sVideoMode.w;
		m_sWindowedResolution.h=sVideoMode.h;
		
		// Esto es un apa�o de un problema en la persistencia por el que no se pueden especificar valores por defecto
		// para SGameRect ni SGameSize, ademas no hacerlo supondria un problema para obtener los valores por defecto para
		// los bpp y el refreshrate.
		
		if(m_sScreenProperties.rWindowRect.x==0 && m_sScreenProperties.rWindowRect.y==0 && m_sScreenProperties.rWindowRect.w==0 && m_sScreenProperties.rWindowRect.h==0)
		{
			m_sScreenProperties.eWindowReferenceSystem=eGameGUIReferenceSystem_Relative;
			m_sScreenProperties.rWindowRect.x=rRelativeFullScreenRect.x+rRelativeFullScreenRect.w*0.125;
			m_sScreenProperties.rWindowRect.y=rRelativeFullScreenRect.y+rRelativeFullScreenRect.h*0.125;
			m_sScreenProperties.rWindowRect.w=rRelativeFullScreenRect.w*0.75;
			m_sScreenProperties.rWindowRect.h=rRelativeFullScreenRect.h*0.75;
		}
		if(m_sScreenProperties.sFullScreenResolution.w==0 || m_sScreenProperties.sFullScreenResolution.h==0)
		{
			m_sScreenProperties.sFullScreenResolution.w=sVideoMode.w;
			m_sScreenProperties.sFullScreenResolution.h=sVideoMode.h;
		}
		if(m_sScreenProperties.dFullScreenRefreshRate==0)
		{
			m_sScreenProperties.dFullScreenRefreshRate=sVideoMode.rate;
		}
		if(m_sScreenProperties.dFullScreenRefreshBitsPerPixel==0)
		{
			m_sScreenProperties.dFullScreenRefreshBitsPerPixel=sVideoMode.bpp;
		}
		
		// Se crea la ventana.
		
		SGameRect rWindowRect;
		if(m_sScreenProperties.eWindowReferenceSystem==eGameGUIReferenceSystem_Relative)
		{
			rWindowRect.x=m_sScreenProperties.rWindowRect.x*m_sWindowedResolution.w;
			rWindowRect.y=m_sScreenProperties.rWindowRect.y*m_sWindowedResolution.h;
			rWindowRect.w=m_sScreenProperties.rWindowRect.w*m_sWindowedResolution.w;
			rWindowRect.h=m_sScreenProperties.rWindowRect.h*m_sWindowedResolution.h;
		}
		else
		{
			rWindowRect=m_sScreenProperties.rWindowRect;
		}
		if(m_sScreenProperties.bWindowCentered)
		{
			SGameRect sScreenRect;
			sScreenRect.x=sVideoMode.fullscreenX;
			sScreenRect.y=sVideoMode.fullscreenY;
			sScreenRect.w=sVideoMode.fullscreenW;
			sScreenRect.h=sVideoMode.fullscreenH;
			rWindowRect.CenterOnRect(&sScreenRect);
			
			if(m_sScreenProperties.eWindowReferenceSystem==eGameGUIReferenceSystem_Relative)
			{
				m_sScreenProperties.rWindowRect.x=rWindowRect.x/m_sWindowedResolution.w;
				m_sScreenProperties.rWindowRect.y=rWindowRect.y/m_sWindowedResolution.h;
			}
			else
			{
				m_sScreenProperties.rWindowRect.x=rWindowRect.x;
				m_sScreenProperties.rWindowRect.y=rWindowRect.y;
			}
		}
		if(m_sScreenProperties.bFullScreen)
		{
			bOk=m_Viewport.m_piViewport->CreateFullScreen(m_sScreenProperties.sFullScreenResolution.w,m_sScreenProperties.sFullScreenResolution.h,m_sScreenProperties.dFullScreenRefreshBitsPerPixel,m_sScreenProperties.dFullScreenRefreshRate);
			if(!bOk){RTTRACE("CGameGUIManager::Setup -> Failed create full screen viewport");}
		}
		else
		{
			bOk=m_Viewport.m_piViewport->CreateWindowed((unsigned int)rWindowRect.x,(unsigned int)rWindowRect.y,(unsigned int)rWindowRect.w,(unsigned int)rWindowRect.h);
			if(!bOk){RTTRACE("CGameGUIManager::Setup -> Failed create windowed viewport");}
		}
		if(bOk){m_Viewport.m_piViewport->ShowMouseCursor(false);}
		if(bOk){m_Viewport.m_piViewport->SetVSync(m_sScreenProperties.bVerticalSync);}
	}
	if(!bOk)
	{
		m_Render.Detach();
		m_Viewport.Detach();
	}
	
	if(bOk)
	{
		bOk=m_Render.m_piRender->Setup(m_Viewport.m_piViewport);
		if(!bOk){RTTRACE("CGameGUIManager::Setup -> Failed to setup render");}
	}

	if(bOk)
	{
		bOk=(m_MainWindow.Attach("GameGUI","MainWindow"));
		if(!bOk){RTTRACE("CGameGUIManager::Setup -> Main window not found");}
	}
	if(bOk)
	{
		if(m_piRootWindow==NULL){m_piRootWindow=new CGameMainWindow(this);}
		bOk=m_piRootWindow->InitWindow(m_piRootWindow,true);
		if(bOk){bOk=m_MainWindow.m_piWindow->InitWindow(m_piRootWindow,false);}
		if(bOk){m_piFocusedWindow=ADD(m_MainWindow.m_piWindow);}
	}	
	if(bOk)
	{
		if(m_Viewport.m_piViewport->GetCurrentJoystick()=="")
		{
			// Set default device (if any)
			std::vector<std::string> vJoysticks;
			m_Viewport.m_piViewport->GetJoysticks(&vJoysticks);
			for(int x=0;x<(int)vJoysticks.size();x++)
			{
				if(m_Viewport.m_piViewport->SetCurrentJoystick(vJoysticks[x]))
				{
					break;
				}
			}
		}
	}
	m_bSetup=bOk;
	
	return bOk;
}

bool CGameGUIManager::DetectDrag(double dx,double dy)
{
	if(!m_Viewport.m_piViewport){return false;}
	return m_Viewport.m_piViewport->DetectDrag(dx,dy);
}
	
bool CGameGUIManager::IsNavigationControl(EGameGUINavigationControl eNavigationControl,int nKey)
{
	if(nKey==0){return false;}
	switch(eNavigationControl)
	{
		case eGameGUINavigationControl_Up:return nKey==GK_UP || nKey==GK_JOYUP || nKey==m_AdditionalNavigationControls.nKeyboardUp;
		case eGameGUINavigationControl_Down:return nKey==GK_DOWN || nKey==GK_JOYDOWN || nKey==m_AdditionalNavigationControls.nKeyboardDown;
		case eGameGUINavigationControl_Left:return nKey==GK_LEFT || nKey==GK_JOYLEFT || nKey==m_AdditionalNavigationControls.nKeyboardLeft;
		case eGameGUINavigationControl_Right:return nKey==GK_RIGHT || nKey==GK_JOYRIGHT || nKey==m_AdditionalNavigationControls.nKeyboardRight;
		case eGameGUINavigationControl_Accept:return nKey==GK_RETURN || nKey==m_AdditionalNavigationControls.nKeyboardAccept || nKey==m_AdditionalNavigationControls.nJoystickAccept;
		case eGameGUINavigationControl_Cancel:return nKey==GK_ESCAPE || nKey==m_AdditionalNavigationControls.nKeyboardCancel || nKey==m_AdditionalNavigationControls.nJoystickCancel;
	}
	return false;
}

void CGameGUIManager::GetAdditionalNavigationControls(SGameGUIAdditionalNavigationControls *psControls)
{
	*psControls=m_AdditionalNavigationControls;
}

void CGameGUIManager::SetAdditionalNavigationControls(SGameGUIAdditionalNavigationControls *psControls)
{
	m_AdditionalNavigationControls=*psControls;
}

void CGameGUIManager::GetJoysticks(std::vector<std::string> *psJoysticks)
{
	psJoysticks->clear();
	if(m_Viewport.m_piViewport){m_Viewport.m_piViewport->GetJoysticks(psJoysticks);}
}

std::string CGameGUIManager::GetCurrentJoystick()
{
	if(m_Viewport.m_piViewport){return m_Viewport.m_piViewport->GetCurrentJoystick();}
	return "";
}

bool CGameGUIManager::SetCurrentJoystick(std::string sJoystick)
{
	if(m_Viewport.m_piViewport){return m_Viewport.m_piViewport->SetCurrentJoystick(sJoystick);}
	return false;
}

void CGameGUIManager::SetJoystickDeadZone(double dDeadZone)
{
	if(m_Viewport.m_piViewport){m_Viewport.m_piViewport->SetJoystickDeadZone(dDeadZone);}
}

double CGameGUIManager::GetJoystickDeadZone()
{
	return m_Viewport.m_piViewport?m_Viewport.m_piViewport->GetJoystickDeadZone():0;
}