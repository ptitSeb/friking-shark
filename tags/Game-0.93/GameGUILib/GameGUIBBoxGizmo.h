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

class CGameGUIBBoxGizmo
{
	int  m_nSelectedElement;
	int  m_nSelectionBase;

	double m_dArrowSize;
	
	CVector m_vMins;
	CVector m_vMaxs;
	CVector m_vCenter;
	CVector m_vPosition;
	CVector m_vTranslationOrigin;
	CVector m_vTranslationStartMins;
	CVector m_vTranslationStartMaxs;
	
	void RenderArrow(IGenericRender *piRender,CVector vPosition,CVector vDirection,CVector vUp,CVector vColor);
	CVector PositionFromRay(CLine *pMouseRay,IGenericCamera *piCamera);
public:

	void SetArrowSize(double dSize);
	
	void SetBounds(CVector vMins,CVector vMaxs);
	void GetBounds(CVector *pvMins,CVector *pvMaxs);
	
	void    SetPosition(CVector vPosition);
	CVector GetPosition();
	
	void Render(IGenericRender *piRender, IGenericCamera *piCamera);
	int  SelectionRender(int nBaseId,IGenericRender *piRender, IGenericCamera *piCamera);
	void Select(int nElementId);

	bool    BeginBBox(CLine *pMouseRay,IGenericCamera *piCamera);
	void	ProcessBBox(CLine *pMouseRay,IGenericCamera *piCamera);
	void 	EndBBox();

	 CGameGUIBBoxGizmo();
	~CGameGUIBBoxGizmo();
};
