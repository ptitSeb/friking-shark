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

class CAudioOptions: virtual public CGameDialogBase,virtual public IGameGUIButtonEvents, virtual public IGameGUISliderEvents, virtual public IAudioOptions
{
	IGameGUISlider *m_piSLMaster;
	IGameGUISlider *m_piSLMusic;
	IGameGUISlider *m_piSLSounds;
	IGameGUIButton *m_piBTOk;
	IGameGUIButton *m_piBTCancel;
	
	unsigned int m_nOriginalMaster;
	unsigned int m_nOriginalMusic;
	unsigned int m_nOriginalSound;
	
	CSoundManagerWrapper m_SoundManager;
	/*
	BEGIN_PROP_MAP(CAudioOptions)
	PROP_CLASS_CHAIN(CGameDialogBase);
	END_PROP_MAP()*/
	
	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY_EX("Master",m_piSLMaster,IGameGUISliderEvents);
		CHILD_MAP_ENTRY_EX("Music",m_piSLMusic,IGameGUISliderEvents);
		CHILD_MAP_ENTRY_EX("Sounds",m_piSLSounds,IGameGUISliderEvents);
		CHILD_MAP_ENTRY_EX("Ok",m_piBTOk,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Cancel",m_piBTCancel,IGameGUIButtonEvents);
	END_CHILD_MAP()

	void UpdateGUI();
	
public:
	
	void OnInitDialog();
	void OnEndDialog();
	void OnKeyDown(int nKey,bool *pbProcessed);
	
	bool Show(IGameWindow *piParent);
	
	// IGameButtonEvents

	void OnButtonClicked(IGameGUIButton *piControl);
	
	// IGameSliderEvents
	
	void OnSliderValueChanged(IGameGUISlider *piControl,double dValue);
	
	CAudioOptions(void);
	~CAudioOptions(void);
};
