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


#pragma once

DECLARE_CUSTOM_WRAPPER1(CFontWrapper,IGenericFont,m_piFont)

class CGameGUILabel: virtual public CGameWindowBase, virtual public IGameGUILabel
{
protected:
	CVector m_vTextColor;
	double  m_dTextAlpha;

	std::string m_sText;
	eTextAlignment m_eHorizontalAlignment;
	eTextAlignment m_eVerticalAlignment;
	bool m_bAutoResizeFont;

	void DrawText(IGenericRender *piRender,CVector &vColor,double dAlpha);
	void DrawText(IGenericFont *piFont,double dFontSize,IGenericRender *piRender,CVector &vColor,double dAlpha);
	
public:

	BEGIN_PROP_MAP(CGameGUILabel)
		PROP_CLASS_CHAIN(CGameWindowBase)
		PROP_VALUE_FLAGS(m_bAutoResizeFont,"AutoResizeFont",false,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vTextColor,"TextColor",CVector(0,0,0),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dTextAlpha,"TextAlpha",1.0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_sText,"Text","",MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_eHorizontalAlignment,"HorzAlign",eTextAlignment_Left,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_eVerticalAlignment,"VertAlign",eTextAlignment_Top,MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP()

	// IGameGUILabel

	void				SetText(std::string sText);
	std::string			GetText();

	void				SetHorizontalAlignment(eTextAlignment eAlignment);
	void				SetVerticalAlignment(eTextAlignment eAlignment);
	eTextAlignment		GetHorizontalAlignment();
	eTextAlignment		GetVerticalAlignment();

	void	GetTextColor(CVector *pvColor,double *pdAlpha);
	void	SetTextColor(CVector vColor,double dAlpha);

	// CGameWindowBase
	void				OnDraw(IGenericRender *piRender);

   CGameGUILabel(void);
	~CGameGUILabel(void);
};
