// GameRender.cpp: implementation of the GameRender class.
//
//////////////////////////////////////////////////////////////////////

#include "./stdafx.h"
#include <float.h>
#include "GameRender.h"

void EntityOperation_RenderBBox(IEntity *piEntity,void *pParam1,void *pParam2)
{
    IGenericRender *pRender=(IGenericRender *)pParam1;
    SPhysicInfo *pPhysicInfo=piEntity->GetPhysicInfo();
    unsigned int dwAlignment=piEntity->GetAlignment();
	CVector vColor(1,1,0);
    switch(dwAlignment)
    {
	case ENTITY_ALIGNMENT_NEUTRAL:
		vColor=CVector(1,1,1);
		break;
    case ENTITY_ALIGNMENT_PLAYER:
      vColor=CVector(0,1,0);
      break;
    case ENTITY_ALIGNMENT_ENEMIES:
      vColor=CVector(1,0,0);
      break;
    }
    pRender->RenderBBox(pPhysicInfo->vPosition,pPhysicInfo->vAngles,pPhysicInfo->vMins,pPhysicInfo->vMaxs,vColor);
}

void EntityOperation_CustomRender(IEntity *piEntity,void *pParam1,void *pParam2)
{
	piEntity->Render((IGenericRender*)pParam1,(IGenericCamera*)pParam2);
}

CGameRender::CGameRender()
{
	m_dwMaxOpenGLSkinWidth=0;
	m_dwMaxOpenGLSkinHeight=0;

	m_dwFlags=RENDER_ENABLE_LIGHTING|RENDER_ENABLE_TEXTURES|RENDER_ENABLE_MODEL_LOD|RENDER_ENABLE_TRANSPARENCY|RENDER_ENABLE_SOLID|RENDER_ENABLE_MIPMAPS;
}

CGameRender::~CGameRender()
{

}

unsigned int	CGameRender::EnableFlags(unsigned int dwFlags){m_dwFlags|=dwFlags;return m_dwFlags;}
unsigned int	CGameRender::DisableFlags(unsigned int dwFlags){m_dwFlags&=~dwFlags;return m_dwFlags;}
unsigned int	CGameRender::GetFlag(unsigned int dwFlag){return m_dwFlags&dwFlag;}
unsigned int	CGameRender::ToggleFlag(unsigned int dwFlag){if(GetFlag(dwFlag)){DisableFlags(dwFlag);}else{EnableFlags(dwFlag);}return dwFlag;}


void CGameRender::Render(IGenericRender *piRender,IGenericCamera *piCamera)
{
	if(m_WorldManager.m_piTerrain==NULL)
	{
		m_WorldManager.Attach("GameSystem","WorldManager");
	}
	if(m_PlayAreaManager.m_piPlayAreaManager==NULL)
	{
		m_PlayAreaManager.Attach("GameSystem","PlayAreaManager");
	}
	IGenericModel *piModel=NULL;
	if(m_WorldManager.m_piTerrain)
	{
		m_WorldManager.m_piTerrain->GetTerrainModel(&piModel);
	}
	if(piModel)
	{
		piRender->PushState();
		piRender->ActivateDepth();
		piRender->StartStagedRendering();
		m_WorldManager.m_piWorldManager->SetupRenderingEnvironment(piRender);
		m_EntityManager.m_piEntityManager->PerformUnaryOperation(EntityOperation_CustomRender,piRender,piCamera);
		piRender->EndStagedRendering();
		piRender->PopState();
	}
	REL(piModel);
}

bool CGameRender::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
	bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);
	m_EntityManager.Attach("GameSystem","EntityManager");
	return bOk;
}

void CGameRender::Destroy()
{
	m_WorldManager.Detach();
	m_PlayAreaManager.Detach();
	m_EntityManager.Detach();
	CSystemObjectBase::Destroy();
}