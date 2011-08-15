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

#include <Route.h>
#include "SoundSystems.h"

DECLARE_CUSTOM_WRAPPER1(CSoundTypeWrapper,ISoundType,m_piSoundType)

class CMusicManager: virtual public CSystemObjectBase,
						virtual public CSystemSerializableBase,
						virtual public IGameManager,
                        virtual public IMusicManager,
						virtual public IMusicDesign,
						virtual public IFrameManagerEvents
{
	bool m_bStarted;
	
	CGameControllerWrapper  m_GameControllerWrapper;
	CPlayerManagerWrapper   m_PlayerManagerWrapper;
	CFrameManagerWrapper   m_FrameManagerWrapper;
	
	ISound *m_piMusicSound;
	ISound *m_piIntroMusicSound;
	ISound *m_piIntermissionMusicSound;
	
	CSoundTypeWrapper				m_IntroMusic;
	CSoundTypeWrapper				m_Music;
	CSoundTypeWrapper				m_IntermissionMusic;
	
	unsigned int m_nIntermissionFadeStart;
	unsigned int m_nIntroFadeStart;
	unsigned int m_nMusicFadeStart;
	
	void ProcessFadeOut(ISound *piSound, unsigned int nCurrentTime,unsigned int *pNextFadeStep);
	
public:

    bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    void Destroy();

    // IMusicManager
	
	// IMusicDesign
	
	bool SetIntroMusic(std::string sMusicFile);
	void GetIntroMusic(std::string *psMusicFile,ISoundType **ppiSoundType);
	
	bool SetMusic(std::string sMusicFile);
	void GetMusic(std::string *psMusicFile,ISoundType **ppiSoundType);
	
    // IGameManager

	void CreateScenario();
	void LoadScenario(ISystemPersistencyNode *piNode);
	void SaveScenario(ISystemPersistencyNode *piNode);
	void CloseScenario();

    void Start();
    void Stop();
    void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

	void OnResumed();
	void OnPaused();
	
	BEGIN_PROP_MAP(CMusicManager)
		PROP_FLAGS(m_IntermissionMusic,"IntermissionMusic",MRPF_NORMAL|MRPF_OPTIONAL)
		BEGIN_PROP_SUBMAP("ScenarioProps")
			PROP_FLAGS(m_Music,"Music",MRPF_NORMAL|MRPF_OPTIONAL)
			PROP_FLAGS(m_IntroMusic,"IntroMusic",MRPF_NORMAL|MRPF_OPTIONAL)
		END_PROP_SUBMAP("ScenarioProps")
	END_PROP_MAP();

    CMusicManager(void);
    ~CMusicManager(void);
};
