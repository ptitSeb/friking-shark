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

#include "GameGUILabel.h"

class CGameGUIEdit: virtual public CGameGUILabel, virtual public IGameGUIEdit
{
protected:
	int m_nEditionPos;
	int m_nSelectionPos;
	
	CVector m_vBorderColor;
	double  m_dBorderAlpha;
	
	CVector m_vSelectedTextColor;
	CVector m_vSelectedBackgroundColor;

	void DrawText(IGenericRender *piRender,CVector &vColor,double dAlpha);

	int GetCharacterFromCoordinates(double x,double y);
	
	void DeleteSelection();
public:

	BEGIN_PROP_MAP(CGameGUIEdit)
		PROP_CLASS_CHAIN(CGameGUILabel)
		PROP_VALUE_FLAGS(m_vBorderColor,"BorderColor",CVector(1,1,1),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dBorderAlpha,"BorderAlpha",1.0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_eHorizontalAlignment,"HorzAlign",eTextAlignment_Left,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_eVerticalAlignment,"VertAlign",eTextAlignment_Center,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_vSelectedTextColor,"SelectedTextColor",CVector(1,1,1),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vSelectedBackgroundColor,"SelectedBKColor",CVector(0,0,1),MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP()

	// IGameGUIEdit

	void	GetBorderColor(CVector *pvColor,double *pdAlpha);
	void	SetBorderColor(CVector vColor,double dAlpha);

	void	SetText(std::string sText);
	void 	SetCursor(int nIndex);
	// Base

	void 	OnCharacter( int nKey,bool *pbProcessed );
	void 	OnKeyDown(int nKey,bool *pbProcessed);
	void	OnDraw(IGenericRender *piRender);

	void OnMouseMove(double x,double y);
	void OnMouseDown(int nButton,double x,double y);
	void OnMouseUp(int nButton,double x,double y);
	void OnWantFocus(bool *pbWant);
	
   CGameGUIEdit(void);
	~CGameGUIEdit(void);
};
