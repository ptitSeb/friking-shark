#pragma once

#define GAMESIDEPANEL_MAX_PLAYER_LIVES_TO_DISPLAY 4

class CGameSidePanel: virtual public CGameWindowBase
{
	struct CGameSidePanelPlayerData
	{
		IGameGUILabel *m_piSTPoints;
		IGameWindow		*m_piSTLives[GAMESIDEPANEL_MAX_PLAYER_LIVES_TO_DISPLAY];

		CGameSidePanelPlayerData()
		{
			m_piSTPoints=NULL;
			for(int x=0;x<GAMESIDEPANEL_MAX_PLAYER_LIVES_TO_DISPLAY;x++)
			{
				m_piSTLives[x]=NULL;
			}
		}
	};

	CGameSidePanelPlayerData m_Player1Data;
	CGameSidePanelPlayerData m_Player2Data;

	CEntityManagerWrapper		 m_EntityManagerWrapper;

	void UpdatePlayerData(CGameSidePanelPlayerData *pPlayerData,const char *pPlayerName);

public:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("Player1Points",m_Player1Data.m_piSTPoints);
		CHILD_MAP_ENTRY("Player1Live0",m_Player1Data.m_piSTLives[0]);
		CHILD_MAP_ENTRY("Player1Live1",m_Player1Data.m_piSTLives[1]);
		CHILD_MAP_ENTRY("Player1Live2",m_Player1Data.m_piSTLives[2]);
		CHILD_MAP_ENTRY("Player1Live3",m_Player1Data.m_piSTLives[3]);
	END_CHILD_MAP()

	void DestroyWindow();
	void OnDraw(IGenericRender *piRender);

	CGameSidePanel(void);
	~CGameSidePanel(void);
};
