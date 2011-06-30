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

class IScenarioEditorObjectListSelector: virtual public ISystemUnknown
{
public:
	
	virtual bool SelectObject(std::string sTitle,IGameWindow *piParent,std::vector<IDesignObject *> *vObjects,unsigned long *pnSelectedObject)=0;
	
};
class CScenarioEditorObjectListSelector: public CGameDialogBase, virtual public IScenarioEditorObjectListSelector,virtual public IGameGUIListEvents
{
	std::string m_sTitle;
	
	bool m_bShowList;
	
	IGameGUILabel *m_piSTTitle;
	IGameGUIList  *m_piLSObjects;
	
	std::vector<IDesignObject *>    *m_pvObjects;
	unsigned long m_nSelectedObject;
	
	
	void OnInitDialog();
	void OnEndDialog();
public:
	
	BEGIN_CHILD_MAP()
	CHILD_MAP_ENTRY("Title",m_piSTTitle);
	CHILD_MAP_ENTRY_EX("ObjectListSelectorList",m_piLSObjects,IGameGUIListEvents);
	END_CHILD_MAP()
	
	bool SelectObject(std::string sTitle,IGameWindow *piParent,std::vector<IDesignObject *> *vObjects,unsigned long *pnSelectedObject);
	
	void OnSelectionChanged(IGameGUIList *piControl,int nElement,std::string sElement);
	void OnSelectionDoubleCliked(IGameGUIList *piControl,int nElement,std::string sElement);
	
	CScenarioEditorObjectListSelector(void);
	~CScenarioEditorObjectListSelector(void);
};
