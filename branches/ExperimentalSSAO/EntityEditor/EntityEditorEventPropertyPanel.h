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

class CEntityEditorEventPropertyPanel: public CGameWindowBase, virtual public IEntityEditorPropertyPanel, 
	virtual public IGameGUIButtonEvents, 
	virtual public IGameGUIEditEvents
{
	CAnimationEventTypeWrapper m_Object;

	void UpdateData();
	
	IGameGUIEdit *m_piEDName;
	IGameGUIEdit *m_piEDParams;
	
	IGameGUILabel *m_piSTTime;
	IGameGUIButton *m_piBTDecreaseTime;
	IGameGUIButton *m_piBTIncreaseTime;

	IGameGUIButton *m_piBTRemove;

protected:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("EventTime",m_piSTTime);
		CHILD_MAP_ENTRY_EX("EventDecreaseTime",m_piBTDecreaseTime,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EventIncreaseTime",m_piBTIncreaseTime,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("EventName",m_piEDName,IGameGUIEditEvents);
		CHILD_MAP_ENTRY_EX("EventParams",m_piEDParams,IGameGUIEditEvents);
		CHILD_MAP_ENTRY_EX("EventRemove",m_piBTRemove,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	void OnButtonClicked(IGameGUIButton *piControl);
	void OnTextChanged(IGameGUIEdit *piControl,std::string sNewText);

public:
	// IEntityEditorPropertyPanel

	bool SetObject(ISystemObject *piObject);
	
	CEntityEditorEventPropertyPanel(void);
	~CEntityEditorEventPropertyPanel(void);
};
