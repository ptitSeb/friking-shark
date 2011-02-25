#include "./stdafx.h"
#include "GameRunTimeLib.h"
#include "GameGUILib.h"
#include "GameSidePanel.h"

CGameSidePanel::CGameSidePanel(void)
{
}

CGameSidePanel::~CGameSidePanel(void)
{
}

void CGameSidePanel::OnDraw(IGenericRender *piRender)
{
	if(m_EntityManagerWrapper.m_piEntityManager==NULL)
	{
		m_EntityManagerWrapper.Attach("GameSystem","EntityManager");
	}

	UpdatePlayerData(&m_Player1Data,"Player");
	UpdatePlayerData(&m_Player2Data,"Player2");

	CGameWindowBase::OnDraw(piRender);
}

void CGameSidePanel::DestroyWindow()
{
	m_EntityManagerWrapper.Detach();
	CGameWindowBase::DestroyWindow();
}

void CGameSidePanel::UpdatePlayerData(CGameSidePanelPlayerData *pPlayerData,const char *pPlayerName)
{
	unsigned int nPoints=0;
	unsigned int nLivesLeft=0;

	IEntity	*piPlayerEntity=NULL;
	IPlayer *piPlayer=NULL;

	if(m_EntityManagerWrapper.m_piEntityManager!=NULL)
	{
		piPlayerEntity=m_EntityManagerWrapper.m_piEntityManager->FindEntity(pPlayerName);
		if(piPlayerEntity)
		{
			piPlayer=dynamic_cast<IPlayer*>(piPlayerEntity);
		}
	}

	if(piPlayer)
	{
		nPoints=piPlayer->GetPoints();
		nLivesLeft=piPlayer->GetLivesLeft();
	}
	if(pPlayerData->m_piSTPoints)
	{
		char sTemp[200]={0};
		sprintf(sTemp,"%d",nPoints);
		pPlayerData->m_piSTPoints->SetText(sTemp);
	}
	for(unsigned int x=0;x<GAMESIDEPANEL_MAX_PLAYER_LIVES_TO_DISPLAY;x++)
	{
		if(pPlayerData->m_piSTLives[x])
		{
			pPlayerData->m_piSTLives[x]->Show(nLivesLeft && x<(nLivesLeft-1));
		}
	}
}
