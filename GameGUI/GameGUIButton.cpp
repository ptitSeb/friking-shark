#include "StdAfx.h"
#include "GameGUILib.h"
#include ".\gameguilabel.h"
#include ".\gameguibutton.h"

CGameGUIButton::CGameGUIButton(void)
{
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
	IGameWindow *piCapture=m_piGUIManager->GetMouseCapture();
	if(piCapture==this && m_bHoverEnabled)
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
	REL(piCapture);
}

void CGameGUIButton::OnDraw(IGenericRender *piRender)
{
	IGameWindow *piCapture=m_piGUIManager->GetMouseCapture();
	if(piCapture==this && m_bHoverEnabled)
	{
		DrawText(piRender,m_vHoverTextColor,m_dHoverTextAlpha);
	}
	else
	{
		CGameGUILabel::OnDraw(piRender);	
	}
	REL(piCapture);
}

void CGameGUIButton::OnMouseMove(double x,double y)
{
	IGameWindow *piCaptureWindow=m_piGUIManager->GetMouseCapture();
	if(piCaptureWindow==NULL)
	{
		m_piGUIManager->SetMouseCapture(this);
	}
	else if(piCaptureWindow==this && !m_bClickInProgress)
	{
		if(x<0 || x>m_rRealRect.w || y<0 || y>m_rRealRect.h)
		{
			m_piGUIManager->ReleaseMouseCapture();
		}
	}
	REL(piCaptureWindow);
}

void CGameGUIButton::OnMouseDown(int nButton,double x,double y)
{
	if(nButton!=MK_LBUTTON){return;}
	m_piGUIManager->SetMouseCapture(this);
	m_bClickInProgress=true;
}

void CGameGUIButton::OnMouseUp(int nButton,double x,double y)
{
	if(nButton!=MK_LBUTTON){return;}

	m_bClickInProgress=false;

	IGameWindow *piCapture=m_piGUIManager->GetMouseCapture();
	if(piCapture==this)
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
	REL(piCapture);
}
