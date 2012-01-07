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
#include "GameGUILib.h"
#include "GameGUILabel.h"
#include "GameGUIButton.h"

CGameGUIButton::CGameGUIButton(void)
{
  	m_bActive=true;
	m_bWantFocus=true;
	m_bFocusOnHover=false;
	m_bClickInProgress=false;
	m_bHoverEnabled=true;
	m_dHoverBackgroundAlpha=1.0;
	m_vHoverBackgroundColor=CVector(0.5,0.5,1.0);
	m_dHoverTextAlpha=1.0;
	m_vHoverTextColor=CVector(0,0,0);
	m_piClickSound=NULL;
	m_piHoverSound=NULL;
	m_bSoundsEnabled=true;
}

CGameGUIButton::~CGameGUIButton(void)
{
}

bool CGameGUIButton::InitWindow(IGameWindow *piParent,bool bPopup)
{
	bool bOk=CGameGUILabel::InitWindow(piParent,bPopup);
	if(bOk)
	{
		SGameRect rEmpty;
		if(m_rDeactivatedTextureCoords==rEmpty){m_rDeactivatedTextureCoords=SGameRect(0,0,1,1);}
		if(m_rHoverTextureCoords==rEmpty){m_rHoverTextureCoords=SGameRect(0,0,1,1);}
	}
	return bOk;
}

bool CGameGUIButton::Unserialize(ISystemPersistencyNode *piNode)
{
	bool bOk=CGameWindowBase::Unserialize(piNode);
	if(bOk && m_HoverSound.m_piSoundType){m_piHoverSound=m_HoverSound.m_piSoundType->CreateInstance();}
	if(bOk && m_ClickSound.m_piSoundType){m_piClickSound=m_ClickSound.m_piSoundType->CreateInstance();}
	return bOk;
}

void CGameGUIButton::Destroy()
{
	delete m_piClickSound;
	delete m_piHoverSound;
	m_piClickSound=NULL;
	m_piHoverSound=NULL;
	m_HoverSound.Detach();
	m_ClickSound.Detach();
	CGameWindowBase::Destroy();
}

void CGameGUIButton::EnableSounds(){m_bSoundsEnabled=true;}
void CGameGUIButton::DisableSounds()
{
	m_bSoundsEnabled=false;
	if(m_piClickSound){m_piClickSound->Stop();}
	if(m_piHoverSound){m_piHoverSound->Stop();}
}
bool CGameGUIButton::AreSoundsEnabled(){return m_bSoundsEnabled;}

void CGameGUIButton::PlayClickSound()
{
	if(m_bSoundsEnabled && m_piClickSound){m_piClickSound->Play();}
}

void CGameGUIButton::PlayHoverSound()
{
	if(m_bSoundsEnabled && m_piHoverSound){m_piHoverSound->Play();}
}

void CGameGUIButton::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(m_bWantFocus && m_piGUIManager->IsNavigationControl(eGameGUINavigationControl_Accept,nKey))
	{
		*pbProcessed=true;
		if(m_bSoundsEnabled && m_piClickSound){m_piClickSound->Play();}
		NOTIFY_EVENT(IGameGUIButtonEvents,OnButtonClicked(this))
	}
}

void CGameGUIButton::OnWantFocus(bool *pbWant)
{
	*pbWant=m_bWantFocus;
}

void CGameGUIButton::OnSetFocus()
{
	if(m_bHoverEnabled && m_bFocusOnHover)
	{
		m_piGUIManager->SetMouseCapture(this);
		if(m_bSoundsEnabled && m_piHoverSound){m_piHoverSound->Play();}
	}
}

void CGameGUIButton::OnKillFocus(IGameWindow *piFocusedWindow)
{
	if(m_piGUIManager->HasMouseCapture(this))
	{
		m_piGUIManager->ReleaseMouseCapture();
	}
}

void CGameGUIButton::OnDrawBackground(IGenericRender *piRender)
{
	if(m_bActive==false)
	{
		if(m_DeactivatedTexture.m_piTexture)
		{
			piRender->SelectTexture(m_DeactivatedTexture.m_piTexture,0);
			piRender->RenderTexture(CVector(m_rRealRect.x+m_rRealRect.w*0.5,m_rRealRect.y+m_rRealRect.h*0.5,0),m_rRealRect.w,m_rRealRect.h,m_rDeactivatedTextureCoords.x,m_rDeactivatedTextureCoords.y,m_rDeactivatedTextureCoords.w,m_rDeactivatedTextureCoords.h,m_vDeactivatedBackgroundColor,m_dDeactivatedBackgroundAlpha);
			piRender->UnselectTexture(0);
		}
		else
		{
			if(m_dDeactivatedBackgroundAlpha!=0.0)
			{
				piRender->RenderRect(m_rRealRect.x,m_rRealRect.y,m_rRealRect.w,m_rRealRect.h,m_vDeactivatedBackgroundColor,m_dDeactivatedBackgroundAlpha);
			}
		}
	}
	else if((m_piGUIManager->HasMouseCapture(this) && m_bHoverEnabled) ||
			(m_bWantFocus && m_piGUIManager->HasFocus(this)))
	{
		if(m_HoverTexture.m_piTexture)
		{
			piRender->SelectTexture(m_HoverTexture.m_piTexture,0);
			piRender->RenderTexture(CVector(m_rRealRect.x+m_rRealRect.w*0.5,m_rRealRect.y+m_rRealRect.h*0.5,0),m_rRealRect.w,m_rRealRect.h,m_rHoverTextureCoords.x,m_rHoverTextureCoords.y,m_rHoverTextureCoords.w,m_rHoverTextureCoords.h,m_vHoverBackgroundColor,m_dHoverBackgroundAlpha);
			piRender->UnselectTexture(0);
		}
		else
		{
			if(m_dHoverBackgroundAlpha!=0.0)
			{
				piRender->RenderRect(m_rRealRect.x,m_rRealRect.y,m_rRealRect.w,m_rRealRect.h,m_vHoverBackgroundColor,m_dHoverBackgroundAlpha);
			}
		}
	}
	else
	{
		CGameGUILabel::OnDrawBackground(piRender);	
	}
}

void CGameGUIButton::OnDraw(IGenericRender *piRender)
{
	if(m_bActive==false)
	{
		if(m_DeactivatedFont.m_piFont)
		{
			DrawText(m_DeactivatedFont.m_piFont,m_dFontSize,piRender,m_vDeactivatedTextColor,m_dDeactivatedTextAlpha);
		}
		else
		{
			DrawText(piRender,m_vDeactivatedTextColor,m_dDeactivatedTextAlpha);
		}
	}
	else if((m_piGUIManager->HasMouseCapture(this) && m_bHoverEnabled) ||
			(m_bWantFocus && m_piGUIManager->HasFocus(this)))
	{
		if(m_HoverFont.m_piFont)
		{
			DrawText(m_HoverFont.m_piFont,m_dFontSize,piRender,m_vHoverTextColor,m_dHoverTextAlpha);
		}
		else
		{
			DrawText(piRender,m_vHoverTextColor,m_dHoverTextAlpha);
		}
	}
	else
	{
		CGameGUILabel::OnDraw(piRender);	
	}
}

void CGameGUIButton::OnMouseMove(double x,double y)
{
	if(m_piGUIManager->HasMouseCapture(NULL))
	{
		m_piGUIManager->SetMouseCapture(this);
		if(m_bSoundsEnabled && m_piHoverSound){m_piHoverSound->Stop();m_piHoverSound->Play();}
		if(m_bFocusOnHover){m_piGUIManager->SetFocus(this);}
	}
	else if(m_piGUIManager->HasMouseCapture(this) && !m_bClickInProgress)
	{
		if(x<0 || x>m_rRealRect.w || y<0 || y>m_rRealRect.h)
		{
			m_piGUIManager->ReleaseMouseCapture();
			if(m_bFocusOnHover && m_piGUIManager->HasFocus(this))
			{
				IGameWindow *piAncestor=GetFocusableAncestor();
				m_piGUIManager->SetFocus(piAncestor);
				REL(piAncestor);
			}
		}
	}
}

void CGameGUIButton::OnMouseDown(int nButton,double x,double y)
{
	if(nButton!=GK_LBUTTON){return;}
	m_piGUIManager->SetMouseCapture(this);
	m_bClickInProgress=true;
}

void CGameGUIButton::OnMouseUp(int nButton,double x,double y)
{
	if(nButton!=GK_LBUTTON){return;}

	m_bClickInProgress=false;

	if(m_piGUIManager->HasMouseCapture(this))
	{
		SGamePos pos;
		pos.x=x+m_rRealRect.x;
		pos.y=y+m_rRealRect.y;

		if(m_rRealRect.Contains(pos))
		{
			if(m_bSoundsEnabled && m_piClickSound){m_piClickSound->Play();}
			NOTIFY_EVENT(IGameGUIButtonEvents,OnButtonClicked(this))
		}
		else
		{
			m_piGUIManager->ReleaseMouseCapture();
			if(m_bFocusOnHover && m_piGUIManager->HasFocus(this))
			{
				IGameWindow *piAncestor=GetFocusableAncestor();
				m_piGUIManager->SetFocus(piAncestor);
				REL(piAncestor);
			}
		}
	}
}
