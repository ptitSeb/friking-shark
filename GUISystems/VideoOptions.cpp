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
#include "GameRunTimeLib.h"
#include "GameGUILib.h"
#include "GUISystems.h"
#include "VideoOptions.h"

CVideoOptions::CVideoOptions(void)
{
	m_nCurrentRenderPath=0;
}

CVideoOptions::~CVideoOptions(void)
{
}

void CVideoOptions::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();

	if(m_piBTFullScreen)
	{
		m_piGUIManager->SetFocus(m_piBTFullScreen);
	}
	UpdateGUI();
}

void CVideoOptions::OnEndDialog()
{
	CGameDialogBase::OnEndDialog();
}

void CVideoOptions::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTOk)
	{
		m_piBTOk->PlayClickSound();
		EndDialog(DIALOG_OK);
	}
	if(piControl==m_piBTFullScreen)
	{
		SGameScreenProperties screen;
		m_piGUIManager->GetScreenProperties(&screen);
		screen.bFullScreen=!screen.bFullScreen;
		m_piGUIManager->SetScreenProperties(&screen);
		UpdateGUI();
	}
	if(piControl==m_piBTVerticalSync)
	{
		SGameScreenProperties screen;
		m_piGUIManager->GetScreenProperties(&screen);
		screen.bVerticalSync=!screen.bVerticalSync;
		m_piGUIManager->SetScreenProperties(&screen);
		UpdateGUI();
	}
	if(piControl==m_piBTShadows && m_Render.m_piRender)
	{
		bool bEnable=true;
		unsigned int nShadows=m_GameInterface.m_piInterfaceWindow->AreShadowsEnabled()?m_GameInterface.m_piInterfaceWindow->GetShadowQuality():eShadowQuality_Count;
		nShadows++;
		if(nShadows>eShadowQuality_Count){nShadows=eShadowQuality_Low;}
		if(nShadows==eShadowQuality_Count)
		{
			m_GameInterface.m_piInterfaceWindow->EnableShadows(false);
		}
		else
		{
			m_GameInterface.m_piInterfaceWindow->EnableShadows(true);
			m_GameInterface.m_piInterfaceWindow->SetShadowQuality((EShadowQuality)nShadows);
		}
		UpdateGUI();
	}
	if(piControl==m_piBTRender && m_Render.m_piRender)
	{
		for(int x=0;x<m_vRenderPaths.size();x++)
		{
			m_nCurrentRenderPath--;
			if(m_nCurrentRenderPath<0){m_nCurrentRenderPath=m_vRenderPaths.size()-1;}
			if(m_Render.m_piRender->SetCurrentRenderPath(m_vRenderPaths[m_nCurrentRenderPath]))
			{
				break;
			}
		}
		m_piBTRender->PlayClickSound();
		UpdateGUI();
	}
}

void CVideoOptions::OnDraw(IGenericRender *piRender)
{
	CGameWindowBase::OnDraw(piRender);

	if(m_piBTFullScreen && m_piBTVerticalSync)
	{
		SGameScreenProperties screen;
		m_piGUIManager->GetScreenProperties(&screen);
		m_piBTFullScreen->SetText(screen.bFullScreen?"Yes":"No");
		m_piBTVerticalSync->SetText(screen.bVerticalSync?"Yes":"No");
	}
}

void CVideoOptions::UpdateGUI()
{
	if(m_Render.m_piRender)
	{
		unsigned int x=0;
		std::string sCurrent=m_Render.m_piRender->GetCurrentRenderPath();
		m_nCurrentRenderPath=0;
		m_Render.m_piRender->GetRenderPaths(&m_vRenderPaths);
		for(x=0;x<m_vRenderPaths.size();x++)
		{
			if(sCurrent==m_vRenderPaths[x])
			{
				m_nCurrentRenderPath=x;
			}
		}
		m_piBTRender->SetText(sCurrent);
		if(m_GameInterface.m_piInterfaceWindow->AreShadowsEnabled())
		{
			switch(m_GameInterface.m_piInterfaceWindow->GetShadowQuality())
			{
			case eShadowQuality_Low:m_piBTShadows->SetText("Low");break;
			case eShadowQuality_Medium:m_piBTShadows->SetText("Medium");break;
			case eShadowQuality_High:m_piBTShadows->SetText("High");break;
			default:m_piBTShadows->SetText("Unknown");break;
			}
		}
		else
		{
			m_piBTShadows->SetText("Off");
		}
	}
}

bool CVideoOptions::Show(IGameWindow *piParent)
{
	m_Render.Attach("GameGUI","Render");
	m_GameInterface.Attach("GameGUI","GameInterface");
	Execute(piParent);
	m_Render.Detach();
	m_GameInterface.Detach();
	return true;
}

void CVideoOptions::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_RETURN){return;}
	if(nKey==GK_ESCAPE){m_piBTOk->PlayClickSound();}

	if((nKey==GK_LEFT || nKey==GK_RIGHT )&& m_piBTFullScreen && m_piGUIManager->HasFocus(m_piBTFullScreen))
	{
		m_piBTFullScreen->PlayClickSound();
		OnButtonClicked(m_piBTFullScreen);
		*pbProcessed=true;
		return;
	}
	if((nKey==GK_LEFT || nKey==GK_RIGHT )&& m_piBTVerticalSync && m_piGUIManager->HasFocus(m_piBTVerticalSync))
	{
		m_piBTVerticalSync->PlayClickSound();
		OnButtonClicked(m_piBTVerticalSync);
		*pbProcessed=true;
		return;
	}
	if(nKey==GK_LEFT && m_piBTShadows && m_GameInterface.m_piInterfaceWindow && m_piGUIManager->HasFocus(m_piBTShadows))
	{
		bool bEnable=true;
		int nShadows=m_GameInterface.m_piInterfaceWindow->AreShadowsEnabled()?m_GameInterface.m_piInterfaceWindow->GetShadowQuality():eShadowQuality_Count;
		nShadows--;
		if(nShadows<0){nShadows=eShadowQuality_Count;}
		if(nShadows==eShadowQuality_Count)
		{
			m_GameInterface.m_piInterfaceWindow->EnableShadows(false);
		}
		else
		{
			m_GameInterface.m_piInterfaceWindow->EnableShadows(true);
			m_GameInterface.m_piInterfaceWindow->SetShadowQuality((EShadowQuality)nShadows);
		}

		m_piBTShadows->PlayClickSound();
		UpdateGUI();

		*pbProcessed=true;
		return;
	}
	if(nKey==GK_RIGHT && m_piBTShadows && m_GameInterface.m_piInterfaceWindow && m_piGUIManager->HasFocus(m_piBTShadows))
	{
		bool bEnable=true;
		int nShadows=m_GameInterface.m_piInterfaceWindow->AreShadowsEnabled()?m_GameInterface.m_piInterfaceWindow->GetShadowQuality():eShadowQuality_Count;
		nShadows++;
		if(nShadows>eShadowQuality_Count){nShadows=eShadowQuality_Low;}
		if(nShadows==eShadowQuality_Count)
		{
			m_GameInterface.m_piInterfaceWindow->EnableShadows(false);
		}
		else
		{
			m_GameInterface.m_piInterfaceWindow->EnableShadows(true);
			m_GameInterface.m_piInterfaceWindow->SetShadowQuality((EShadowQuality)nShadows);
		}

		m_piBTShadows->PlayClickSound();
		UpdateGUI();

		*pbProcessed=true;
		return;
	}

	if(nKey==GK_LEFT && m_piBTRender && m_Render.m_piRender && m_piGUIManager->HasFocus(m_piBTRender))
	{
		for(unsigned int x=0;x<m_vRenderPaths.size();x++)
		{
			m_nCurrentRenderPath++;
			if(m_nCurrentRenderPath>=(int)m_vRenderPaths.size()){m_nCurrentRenderPath=0;}
			if(m_Render.m_piRender->SetCurrentRenderPath(m_vRenderPaths[m_nCurrentRenderPath]))
			{
				break;
			}
		}
		m_piBTRender->PlayClickSound();
		UpdateGUI();

		*pbProcessed=true;
		return;
	}
	if(nKey==GK_RIGHT && m_piBTRender && m_Render.m_piRender && m_piGUIManager->HasFocus(m_piBTRender))
	{
		for(unsigned int x=0;x<m_vRenderPaths.size();x++)
		{
			m_nCurrentRenderPath--;
			if(m_nCurrentRenderPath<0){m_nCurrentRenderPath=m_vRenderPaths.size()-1;}
			if(m_Render.m_piRender->SetCurrentRenderPath(m_vRenderPaths[m_nCurrentRenderPath]))
			{break;}
		}
		
		m_piBTRender->PlayClickSound();
		UpdateGUI();
		*pbProcessed=true;
		return;
	}
	CGameDialogBase::OnKeyDown(nKey,pbProcessed);
}
