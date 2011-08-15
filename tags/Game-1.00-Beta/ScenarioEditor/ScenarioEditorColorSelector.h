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

class IScenarioEditorColorSelector: virtual public ISystemUnknown
{
public:

	virtual bool SelectColor(IGameWindow *piParent,IGenericTexture *piTexture,CVector *pOutColor)=0;

};

class CScenarioEditorColorSelector: public CGameDialogBase, virtual public IScenarioEditorColorSelector
{
	IGameGUILabel *m_piSTColorMap;
	IGenericTexture *m_piColorMapTexture;
	CVector m_vColor;

	void OnInitDialog();
	void OnMouseDown(int nButton,double x,double y);
public:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("ColorMap",m_piSTColorMap);
	END_CHILD_MAP()

	bool SelectColor(IGameWindow *piParent,IGenericTexture *piTexture,CVector *pOutColor);

	CScenarioEditorColorSelector(void);
	~CScenarioEditorColorSelector(void);
};
