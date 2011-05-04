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

class CEntityEditorModelPropertyPanel: public CGameWindowBase, virtual public IEntityEditorPropertyPanel, 
	virtual public IGameGUIButtonEvents
{
	CAnimationModelTypeWrapper m_Object;

	void UpdateData();

	IGameGUIButton *m_piBTLoop;
	IGameGUIButton *m_piBTCastShadow;
	IGameGUIButton *m_piBTReceiveShadows;
	
	IGameGUILabel *m_piSTStartTime;
	IGameGUIButton *m_piBTDecreaseStartTime;
	IGameGUIButton *m_piBTIncreaseStartTime;

	IGameGUILabel *m_piSTEndTime;
	IGameGUIButton *m_piBTDecreaseEndTime;
	IGameGUIButton *m_piBTIncreaseEndTime;

	IGameGUILabel *m_piSTFps;
	IGameGUIButton *m_piBTDecreaseFps;
	IGameGUIButton *m_piBTIncreaseFps;

	IGameGUIButton *m_piBTRemove;

protected:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY_EX("ModelLoop",m_piBTLoop,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ModelShadow",m_piBTCastShadow,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ModelShadowReception",m_piBTReceiveShadows,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("ModelStartTime",m_piSTStartTime);
		CHILD_MAP_ENTRY_EX("ModelDecreaseStartTime",m_piBTDecreaseStartTime,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ModelIncreaseStartTime",m_piBTIncreaseStartTime,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("ModelEndTime",m_piSTEndTime);
		CHILD_MAP_ENTRY_EX("ModelDecreaseEndTime",m_piBTDecreaseEndTime,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ModelIncreaseEndTime",m_piBTIncreaseEndTime,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("ModelFps",m_piSTFps);
		CHILD_MAP_ENTRY_EX("ModelDecreaseFps",m_piBTDecreaseFps,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ModelIncreaseFps",m_piBTIncreaseFps,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY_EX("ModelRemove",m_piBTRemove,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	void OnButtonClicked(IGameGUIButton *piControl);

public:
	// IEntityEditorPropertyPanel

	bool SetObject(ISystemObject *piObject);
	
	CEntityEditorModelPropertyPanel(void);
	~CEntityEditorModelPropertyPanel(void);
};
