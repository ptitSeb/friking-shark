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
	m_bClickInProgress=false;
	m_bHoverEnabled=true;
	m_dHoverBackgroundAlpha=1.0;
	m_vHoverBackgroundColor=CVector(0.5,0.5,1.0);
	m_dHoverTextAlpha=1.0;
	m_vHoverTextColor=CVector(0,0,0);
}

CGameGUIButton::~CGameGUIButton(void)
{
}

void CGameGUIButton::OnDrawBackground(IGenericRender *piRender)
{
	if(m_bActive==false)
	{
		if(m_DeactivatedTexture.m_piTexture)
		{
			piRender->ActivateBlending();
			piRender->SetColor(m_vDeactivatedBackgroundColor,m_dDeactivatedBackgroundAlpha);
			piRender->SelectTexture(m_DeactivatedTexture.m_piTexture,0);
			piRender->RenderTexture(CVector(m_rRealRect.w*0.5,m_rRealRect.h*0.5,0),m_rRealRect.w,m_rRealRect.h);
			piRender->UnselectTexture(0);
			piRender->DeactivateBlending();
		}
		else
		{
			if(m_dDeactivatedBackgroundAlpha!=0.0)
			{
				piRender->Clear(m_vDeactivatedBackgroundColor,m_dDeactivatedBackgroundAlpha);
			}
		}
	}
	else if(m_piGUIManager->HasMouseCapture(this) && m_bHoverEnabled)
	{
		if(m_HoverTexture.m_piTexture)
		{
			piRender->ActivateBlending();
			piRender->SetColor(m_vHoverBackgroundColor,m_dHoverBackgroundAlpha);
			piRender->SelectTexture(m_HoverTexture.m_piTexture,0);
			piRender->RenderTexture(CVector(m_rRealRect.w*0.5,m_rRealRect.h*0.5,0),m_rRealRect.w,m_rRealRect.h);
			piRender->UnselectTexture(0);
			piRender->DeactivateBlending();
		}
		else
		{
			if(m_dHoverBackgroundAlpha!=0.0)
			{
				piRender->Clear(m_vHoverBackgroundColor,m_dHoverBackgroundAlpha);
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
	else if(m_piGUIManager->HasMouseCapture(this) && m_bHoverEnabled)
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
	}
	else if(m_piGUIManager->HasMouseCapture(this) && !m_bClickInProgress)
	{
		if(x<0 || x>m_rRealRect.w || y<0 || y>m_rRealRect.h)
		{
			m_piGUIManager->ReleaseMouseCapture();
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
		m_piGUIManager->ReleaseMouseCapture();

		if(m_rRealRect.Contains(pos))
		{
			NOTIFY_EVENT(IGameGUIButtonEvents,OnButtonClicked(this))
		}
	}
}
