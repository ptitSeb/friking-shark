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

class CGameGUISlider: virtual public CGameGUIButton, virtual public IGameGUISlider
{
	CGenericTextureWrapper m_FullTexture;
	CGenericTextureWrapper m_FullHoverTexture;
	
	double m_dValue; // 0-1
	double m_dStep;
	
protected:

	BEGIN_PROP_MAP(CGameGUISlider)
		PROP_CLASS_CHAIN(CGameGUIButton)
		PROP_FLAGS(m_FullTexture,"FullTexture",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_FullHoverTexture,"FullHoverTexture",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dStep,"Step",0.1,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP()

	void OnMouseMove(double x,double y);
	void OnMouseDown(int nButton,double x,double y);
	void OnMouseUp(int nButton,double x,double y);
	void OnKeyDown(int nKey,bool *pbProcessed);
	
public:
	// IGameGUISlider
	
	void   SetValue(double dValue);
	double GetValue();
	
	void OnDrawBackground(IGenericRender *piRender);

	CGameGUISlider(void);
	~CGameGUISlider(void);
};
