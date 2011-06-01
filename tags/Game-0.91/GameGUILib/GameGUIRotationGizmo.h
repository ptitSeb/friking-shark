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

class CGameGUIRotationGizmo
{
	bool m_bInteractiveAngles[3];
	bool m_bVisibleAngles[3];

	int  m_nSelectedElement;
	int  m_nSelectionBase;

	double m_dRadius;

	CVector m_vPosition;
	CVector m_vAngles;
	CVector m_vRotationOrigin;
	CVector m_vRotationStartAngles;

	CVector m_vYawAxis1,m_vYawAxis2;
	CVector m_vPitchAxis1,m_vPitchAxis2;
	CVector m_vRollAxis1,m_vRollAxis2;

	void UpdateAxises();
	void RenderAngle(IGenericRender *piRender,IGenericCamera *piCamera,double dAngle,CVector vUp,CVector vRigth,CVector vColor,double dRadius,bool bSelection);
	CVector AnglesFromRay(CLine *pMouseRay,IGenericCamera *piCamera);
	
public:

	void 	SetPosition(CVector vPosition);
	void 	SetAngles(CVector vAngles);
	CVector GetAngles();

	void SetRadius(double dRadius);

	void SetInteractiveAngles(bool bYaw,bool bPitch,bool bRoll);
	void SetVisibleAngles(bool bYaw,bool bPitch,bool bRoll);

	void Render(IGenericRender *piRender, IGenericCamera *piCamera);
	int  SelectionRender(int nBaseId,IGenericRender *piRender, IGenericCamera *piCamera);
	void Select(int nElementId);

	bool 	BeginRotation(CLine *pMouseRay,IGenericCamera *piCamera);
	CVector	ProcessRotation(CLine *pMouseRay,IGenericCamera *piCamera);
	CVector EndRotation();

	 CGameGUIRotationGizmo();
	~CGameGUIRotationGizmo();
};
