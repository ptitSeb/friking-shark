#include "StdAfx.h"
#include "GameGUILib.h"
#include ".\gameguilabel.h"

CGameGUILabel::CGameGUILabel(void)
{
	m_dTextAlpha=1.0;
	m_bAutoResizeFont=false;
}

CGameGUILabel::~CGameGUILabel(void)
{
}

void	CGameGUILabel::GetTextColor(CVector *pvColor,double *pdAlpha)
{
	*pvColor=m_vTextColor;
	*pdAlpha=m_dTextAlpha;
}

void	CGameGUILabel::SetTextColor(CVector vColor,double dAlpha)
{
	m_vTextColor=vColor;
	m_dTextAlpha=dAlpha;
}

void CGameGUILabel::SetText(std::string sText)
{
	m_sText=sText;
}

std::string CGameGUILabel::GetText()
{
	return m_sText;
}

void CGameGUILabel::DrawText(IGenericRender *piRender,CVector &vColor,double dAlpha)
{
	if(m_sText.length())
	{
		std::string sFontName;
		double			dFontSize=0;

		IGenericFont *piFont=NULL;
		GetFont(&piFont,&dFontSize);
		if(piFont)
		{
			if(m_bAutoResizeFont)
			{
				dFontSize=m_rRealRect.h;
			}
			piRender->SetColor(vColor,dAlpha);
			piFont->RenderTextEx(dFontSize,0,0,m_rRealRect.w,m_rRealRect.h,m_sText.c_str(),m_eHorizontalAlignment,m_eVerticalAlignment);
		}
		REL(piFont);
	}
}

void CGameGUILabel::OnDraw(IGenericRender *piRender)
{
	DrawText(piRender,m_vTextColor,m_dTextAlpha);
}
void				CGameGUILabel::SetHorizontalAlignment(eTextAlignment eAlignment){m_eHorizontalAlignment=eAlignment;}
void				CGameGUILabel::SetVerticalAlignment(eTextAlignment eAlignment){m_eVerticalAlignment=eAlignment;}
eTextAlignment		CGameGUILabel::GetHorizontalAlignment(){return m_eHorizontalAlignment;}
eTextAlignment		CGameGUILabel::GetVerticalAlignment(){return m_eVerticalAlignment;}
