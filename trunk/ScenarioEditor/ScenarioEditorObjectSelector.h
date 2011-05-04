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

#define DEFAULT_OBJECT_SELECTOR_BUTTON_SIZE		64.0

class IScenarioEditorObjectSelector: virtual public ISystemUnknown
{
public:

	virtual bool SelectObject(IGameWindow *piParent,std::vector<IDesignObject *> *vObjects,unsigned long *pnSelectedObject,double dButtonSizeX=DEFAULT_OBJECT_SELECTOR_BUTTON_SIZE,double dButtonSizeY=DEFAULT_OBJECT_SELECTOR_BUTTON_SIZE)=0;

};

class CScenarioEditorObjectSelector: public CGameDialogBase, virtual public IScenarioEditorObjectSelector,virtual public IGameGUIButtonEvents
{
	double m_dButtonSizeX;
	double m_dButtonSizeY;

	IGameGUILabel *m_piSTObjectList;
	std::vector<IDesignObject *> *m_pvObjects;
	std::vector<CButtonWrapper> m_vButtons;
	std::vector<CObjectLabelWrapper> m_vLabels;
	unsigned long m_nSelectedObject;
	

	void OnInitDialog();
	void OnEndDialog();
public:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("ObjectSelectorList",m_piSTObjectList);
	END_CHILD_MAP()

	bool SelectObject(IGameWindow *piParent,std::vector<IDesignObject *> *vObjects,unsigned long *pnSelectedObject,double dButtonSizeX,double dButtonSizeY);

	void OnButtonClicked(IGameGUIButton *piControl);
	void OnDraw(IGenericRender *piRender);

	CScenarioEditorObjectSelector(void);
	~CScenarioEditorObjectSelector(void);
};
