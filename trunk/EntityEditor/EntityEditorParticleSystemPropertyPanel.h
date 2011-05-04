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

class CEntityEditorParticleSystemPropertyPanel: public CGameWindowBase, virtual public IEntityEditorPropertyPanel, 
	virtual public IGameGUIButtonEvents
{
	CAnimationParticleSystemTypeWrapper m_Object;

	void UpdateData();

	IGameGUIButton *m_piBTTrackEntity;

	IGameGUILabel *m_piSTStartTime;
	IGameGUIButton *m_piBTDecreaseStartTime;
	IGameGUIButton *m_piBTIncreaseStartTime;

	IGameGUILabel *m_piSTEndTime;
	IGameGUIButton *m_piBTDecreaseEndTime;
	IGameGUIButton *m_piBTIncreaseEndTime;

	IGameGUIButton *m_piBTRemove;

protected:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY_EX("ParticleSystemTrackEntity",m_piBTTrackEntity,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("ParticleSystemStartTime",m_piSTStartTime);
		CHILD_MAP_ENTRY_EX("ParticleSystemDecreaseStartTime",m_piBTDecreaseStartTime,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ParticleSystemIncreaseStartTime",m_piBTIncreaseStartTime,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY("ParticleSystemEndTime",m_piSTEndTime);
		CHILD_MAP_ENTRY_EX("ParticleSystemDecreaseEndTime",m_piBTDecreaseEndTime,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("ParticleSystemIncreaseEndTime",m_piBTIncreaseEndTime,IGameGUIButtonEvents);

		CHILD_MAP_ENTRY_EX("ParticleSystemRemove",m_piBTRemove,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	void OnButtonClicked(IGameGUIButton *piControl);

public:
	// IEntityEditorPropertyPanel

	bool SetObject(ISystemObject *piObject);
	
	CEntityEditorParticleSystemPropertyPanel(void);
	~CEntityEditorParticleSystemPropertyPanel(void);
};
