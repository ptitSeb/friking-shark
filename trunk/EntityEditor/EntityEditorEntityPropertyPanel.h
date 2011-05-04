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

class CEntityEditorEntityPropertyPanel: public CGameWindowBase, virtual public IEntityEditorPropertyPanel, 
	virtual public IGameGUIButtonEvents
{
	CAnimationEntityTypeWrapper m_Object;

	void UpdateData();
	
	IGameGUILabel *m_piSTTime;
	IGameGUIButton *m_piBTDecreaseTime;
	IGameGUIButton *m_piBTIncreaseTime;

	IGameGUIButton *m_piBTRemove;

protected:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("EntityTime",m_piSTTime);
		CHILD_MAP_ENTRY_EX("EntityDecreaseTime",m_piBTDecreaseTime,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityIncreaseTime",m_piBTIncreaseTime,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EntityAnimationObjectRemove",m_piBTRemove,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	void OnButtonClicked(IGameGUIButton *piControl);

public:
	// IEntityEditorPropertyPanel

	bool SetObject(ISystemObject *piObject);
	
	CEntityEditorEntityPropertyPanel(void);
	~CEntityEditorEntityPropertyPanel(void);
};
