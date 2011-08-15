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
#include "GameGUISlider.h"

CGameGUISlider::CGameGUISlider(void)
{
	m_dValue=0.5;
	m_dStep=0.1;
}

CGameGUISlider::~CGameGUISlider(void)
{
}

void CGameGUISlider::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(m_bWantFocus && nKey==GK_LEFT)
	{
		m_dValue-=m_dStep;
		if(m_dValue<0){m_dValue=0;}
		if(m_dValue>1){m_dValue=1;}		
		NOTIFY_EVENT(IGameGUISliderEvents,OnSliderValueChanged(this,m_dValue))
		if(m_bSoundsEnabled && m_piClickSound){m_piClickSound->Play();}
	}
	if(m_bWantFocus && nKey==GK_RIGHT)
	{
		m_dValue+=m_dStep;
		if(m_dValue<0){m_dValue=0;}
		if(m_dValue>1){m_dValue=1;}		
		NOTIFY_EVENT(IGameGUISliderEvents,OnSliderValueChanged(this,m_dValue))
		if(m_bSoundsEnabled && m_piClickSound){m_piClickSound->Play();}
	}
	CGameGUIButton::OnKeyDown(nKey,pbProcessed);
}

void   CGameGUISlider::SetValue(double dValue){m_dValue=dValue;if(m_dValue<0){m_dValue=0;}if(m_dValue>1){m_dValue=1;}}
double CGameGUISlider::GetValue(){return m_dValue;}

void CGameGUISlider::OnDrawBackground(IGenericRender *piRender)
{
	if(!m_bActive)
	{
		CGameGUIButton::OnDrawBackground(piRender);
		return;
	}
		
	SGameRect sOriginalClipRect=m_rRealRect;
	if(piRender->IsClippingActive())
	{
		piRender->GetClipRect(&sOriginalClipRect.x,&sOriginalClipRect.y,&sOriginalClipRect.w,&sOriginalClipRect.h);
	}
	
	SGameRect sFullRect=m_rRealRect;
	SGameRect sEmptyRect=m_rRealRect;
	
	sFullRect.w=m_rRealRect.w*m_dValue;
	sEmptyRect.x=sFullRect.x+sFullRect.w;
	sEmptyRect.w=m_rRealRect.w-sFullRect.w;
		
	// Draw the default bk texture, that is, the emptypart
	piRender->SetClipRect(sEmptyRect.x,sEmptyRect.y,sEmptyRect.w,sEmptyRect.h);
	CGameGUIButton::OnDrawBackground(piRender);
	
	// Draw the full texture, that is, the full part
	piRender->SetClipRect(sFullRect.x,sFullRect.y,sFullRect.w,sFullRect.h);
	if((m_piGUIManager->HasMouseCapture(this) && m_bHoverEnabled) ||
		(m_bWantFocus && m_piGUIManager->HasFocus(this)))
	{
		if(m_HoverTexture.m_piTexture)
		{
			piRender->ActivateBlending();
			piRender->SetColor(m_vHoverBackgroundColor,m_dHoverBackgroundAlpha);
			piRender->SelectTexture(m_FullHoverTexture.m_piTexture,0);
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
		if(m_BackgroundTexture.m_piTexture)
		{
			piRender->ActivateBlending();
			piRender->SetColor(m_vBackgroundColor,m_dBackgroundAlpha);
			piRender->SelectTexture(m_FullTexture.m_piTexture,0);
			piRender->RenderTexture(CVector(m_rRealRect.w*0.5,m_rRealRect.h*0.5,0),m_rRealRect.w,m_rRealRect.h);
			piRender->UnselectTexture(0);
			piRender->DeactivateBlending();
		}
		else
		{
			if(m_dBackgroundAlpha!=0.0)
			{
				piRender->Clear(m_vBackgroundColor,m_dBackgroundAlpha);
			}
		}
	}
	piRender->SetClipRect(sOriginalClipRect.x,sOriginalClipRect.y,sOriginalClipRect.w,sOriginalClipRect.h);
}

void CGameGUISlider::OnMouseMove(double x,double y)
{
	if(m_piGUIManager->HasMouseCapture(this) && m_bClickInProgress)
	{
		m_dValue=x/m_rRealRect.w;
		if(m_dValue<0){m_dValue=0;}
		if(m_dValue>1){m_dValue=1;}
		NOTIFY_EVENT(IGameGUISliderEvents,OnSliderValueChanged(this,m_dValue))
	}
	CGameGUIButton::OnMouseMove(x,y);
}

void CGameGUISlider::OnMouseDown(int nButton,double x,double y)
{
	CGameGUIButton::OnMouseDown(nButton,x,y);
}

void CGameGUISlider::OnMouseUp(int nButton,double x,double y)
{
	if(nButton==GK_LBUTTON && m_piGUIManager->HasMouseCapture(this) && m_bClickInProgress)
	{
		SGamePos pos;
		pos.x=x+m_rRealRect.x;
		pos.y=y+m_rRealRect.y;
		
		m_dValue=x/m_rRealRect.w;
		if(m_dValue<0){m_dValue=0;}
		if(m_dValue>1){m_dValue=1;}

		// when the real rect contains the position, the button base plays the sound.
		if(!m_rRealRect.Contains(pos))
		{
			if(m_bSoundsEnabled && m_piClickSound){m_piClickSound->Play();}
		}
		NOTIFY_EVENT(IGameGUISliderEvents,OnSliderValueChanged(this,m_dValue))
	}
	
	/*if(nButton!=GK_LBUTTON){return;}

	m_bClickInProgress=false;

	if(m_piGUIManager->HasMouseCapture(this))
	{
		SGamePos pos;
		pos.x=x+m_rRealRect.x;
		pos.y=y+m_rRealRect.y;

		if(m_rRealRect.Contains(pos))
		{
			if(m_bSoundsEnabled && m_piClickSound){m_piClickSound->Play();}
			NOTIFY_EVENT(IGameGUISliderEvents,OnButtonClicked(this))
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
	}*/
	CGameGUIButton::OnMouseUp(nButton,x,y);
}



