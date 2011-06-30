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

class CGameGUITranslationGizmo
{
	bool m_bInteractiveAxises[3];
	bool m_bVisibleAxises[3];

	int  m_nSelectedElement;
	int  m_nSelectionBase;

	double m_dSize;

	CVector m_vPosition;
	CVector m_vTranslationOrigin;
	CVector m_vTranslationStartPosition;

	void RenderAxis(IGenericRender *piRender,CVector vDir,CVector vUp,CVector vColor);
	void RenderAxisPlane(IGenericRender *piRender,CVector vDir,CVector vUp,CVector vColor);
	CVector	PositionFromRay(CLine *pMouseRay,IGenericCamera *piCamera);
public:

	void 	SetPosition(CVector vPosition);
	CVector GetPosition();

	void SetSize(double dSize);

	void SetInteractiveAxises(bool bX,bool bY,bool bZ);
	void SetVisibleAxises(bool bX,bool bY,bool bZ);

	void Render(IGenericRender *piRender, IGenericCamera *piCamera);
	int  SelectionRender(int nBaseId,IGenericRender *piRender, IGenericCamera *piCamera);
	void Select(int nElementId);

	bool 	BeginTranslation(CLine *pMouseRay,IGenericCamera *piCamera);
	CVector	ProcessTranslation(CLine *pMouseRay,IGenericCamera *piCamera);
	CVector EndTranslation();

	 CGameGUITranslationGizmo();
	~CGameGUITranslationGizmo();
};
