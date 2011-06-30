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

CGameGUILabel::CGameGUILabel(void)
{
	m_dTextAlpha=1.0;
	m_bActive=false;
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
		
		
		//RTTRACE("CGameGUILabel::DrawText -> %s: %p %d",m_sText.c_str(),m_Font.m_piFont,(int)m_dFontSize);

		IGenericFont *piFont=NULL;
		GetFont(&piFont,&dFontSize);
		if(piFont)
		{
			if(m_bAutoResizeFont)
			{
				dFontSize=m_rRealRect.h;
			}
			piRender->SetColor(vColor,dAlpha);
			piFont->RenderTextEx(piRender,dFontSize,0,0,m_rRealRect.w,m_rRealRect.h,m_sText.c_str(),m_eHorizontalAlignment,m_eVerticalAlignment);
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
