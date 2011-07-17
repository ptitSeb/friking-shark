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

enum EPlayerManagerGameStage
{
	ePlayerManagerGameStage_TakeOff,
	ePlayerManagerGameStage_Moving,
	ePlayerManagerGameStage_Landing,
	ePlayerManagerGameStage_Finished
};

DECLARE_CUSTOM_WRAPPER1(CPlayerProfileWrapper,IPlayerProfile,m_piProfile)

class CPlayerManager: virtual public CSystemObjectBase,
                        virtual public CSystemSerializableBase,
                        virtual public IPlayerManager,
                        virtual public IGameManager,
						virtual public IEntityEvents,
						virtual public IPlayerProfileEvents
{
	bool m_bStarted;
	
	CGameControllerWrapper  m_GameControllerWrapper;
	CPlayAreaManagerWrapper m_PlayAreaManagerWrapper;
	
	CEntityTypeWrapper m_PlayerEntityType;
	IEntity         *m_piPlayerEntity;
	IPlayer         *m_piPlayer;
	CPlayerProfileWrapper m_PlayerProfile;
	
	double	m_dPlayMovementMaxRoll;
	double  m_dPlayMovementMaxForward;
	double  m_dPlayMovementMaxRight;
	double  m_dPlayMovementMinForward;
	double  m_dPlayMovementMinRight;
	double  m_dPlayMovementCurrentForward;
	double  m_dPlayMovementCurrentRight;
	double	m_dPlayMovementCurrentRoll;
	double	m_dPlayMovementRollVelocity;
	unsigned int m_dwPlayMovementLastRollTime;
	double  m_dCameraFollowFactor;
	
	EPlayerManagerGameStage m_eGameStage;
	
	CVector m_vPlayerStart;
	bool   m_bPlayerLandingEnabled;
	bool   m_bPlayerTakeOffEnabled;
	CRoute m_PlayerLandingRoute;
	CRoute m_PlayerTakeOffRoute;
	CVector m_PlayerKilledVelocity;
	double m_dScenarioDifficulty;
	
	double m_dBaseDifficulty;
	double m_dLevelDifficultyWeight;
	double m_dPlayerDifficultyWeight;
	
	std::map<std::string,SKeyMapping> m_KeyboardMapping;
	
	void PrepareResources();
	
	void OnRemoved(IEntity *piEntity);
	void OnKilled(IEntity *piEntity);
	
	void MovePlayer(unsigned long nKey,unsigned int dwCurrentTime,double dTimeFraction);
	
	bool CheckKey(IGameGUIManager *piGUIManager,const char *pKeyName);
	
public:

    bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    void Destroy();

    // IPlayerManager
	
	void SetPlayerStart(CVector vPosition);
	void SetPlayerProfile(IPlayerProfile *piProfile);
	
	void ProcessInput(IGameGUIManager *piManager,unsigned int dwCurrentTime,double dTimeFraction);
	bool IsScenarioCompleted();
	
	void GetPlayerConfig( SPlayerConfig *pConfig );
	void SetPlayerConfig( SPlayerConfig *pConfig );
	
	double GetEffectiveDifficulty();
	
	// IPlayerProfileEvents
	
	void OnPlayerNameChanged(const std::string &sName);
	void OnDifficultyChanged(double dDifficulty);
	void OnKeyboardMappingChanged();
	
    // IGameManager

	void CreateScenario();
	void LoadScenario(ISystemPersistencyNode *piNode);
	void SaveScenario(ISystemPersistencyNode *piNode);
	void CloseScenario();

    void Start();
    void Stop();
    void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);


	BEGIN_PROP_MAP(CPlayerManager)
		PROP_VALUE(m_dBaseDifficulty,"BaseDifficulty",0.8);
		PROP_VALUE(m_dLevelDifficultyWeight,"LevelDifficultyWeight",0.05);
		PROP_VALUE(m_dPlayerDifficultyWeight,"PlayerDifficultyWeight",0.05);
		BEGIN_PROP_SUBMAP("ScenarioProps")
		  PROP(m_PlayerEntityType,"EntityType");
		  PROP_FLAGS(m_PlayerTakeOffRoute,"TakeOffRoute",MRPF_NORMAL|MRPF_OPTIONAL);
		  PROP_FLAGS(m_PlayerLandingRoute,"LandingRoute",MRPF_NORMAL|MRPF_OPTIONAL);
		  PROP_VALUE_FLAGS(m_bPlayerLandingEnabled,"LandingEnabled",false,MRPF_NORMAL|MRPF_OPTIONAL);
		  PROP_VALUE_FLAGS(m_bPlayerTakeOffEnabled,"TakeOffEnabled",false,MRPF_NORMAL|MRPF_OPTIONAL);
		  PROP_VALUE_FLAGS(m_dScenarioDifficulty,"Difficulty",0,MRPF_NORMAL|MRPF_OPTIONAL);
		END_PROP_SUBMAP("ScenarioProps")
	END_PROP_MAP();

    CPlayerManager(void);
    ~CPlayerManager(void);
};
