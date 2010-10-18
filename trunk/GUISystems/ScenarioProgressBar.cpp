#include "StdAfx.h"
#include "GUISystems.h"
#include "GameRuntimeLib.h"
#include "GameGUILib.h"
#include ".\scenarioprogressbar.h"

CScenarioProgressBar::CScenarioProgressBar(void)
{
	m_dVerticalMargin=0;
}

CScenarioProgressBar::~CScenarioProgressBar(void)
{
}

void CScenarioProgressBar::OnDrawBackground(IGenericRender *piRender)
{
	if(m_PlayAreaManagerWrapper.m_piPlayAreaManager==NULL)
	{
		if(m_PlayAreaManagerWrapper.Attach("GameSystem","PlayAreaManager"))
		{
			m_PlayAreaManagerWrapper.m_piPlayAreaManager->EnumeratePlayAreaElements(this);
		}
	}
	if(m_PlayAreaManagerWrapper.m_piPlayAreaManager)
	{
		CVector vStart,vEnd;
		CVector vPos;
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayerRoute(&vStart,&vEnd);
		vPos=m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayMovementPosition();

		double dTotalDistance=vEnd-vStart;
		double dCompletedDistance=vPos-vStart;

		if(dCompletedDistance<0){dCompletedDistance=0;}
		if(dCompletedDistance>dTotalDistance){dCompletedDistance=dTotalDistance;}

		// Este es el factor de distancia recorrida de 0 a 1, que corresponde a la parte de barra real,
		// es decir sin margenes, que deberemos pintar como completa. El resto se debera pintar con la textura normal.

		double dCompleteFactor=dCompletedDistance/dTotalDistance;

		// Este es el factor de textura de 0 a 1, que debemos pintar, este comprende los margenes de la barra de progreso.
		double dTextureFactor=m_dVerticalMargin+(1.0-m_dVerticalMargin*2.0)*dCompleteFactor;

		SGameRect sDrawRect;

		sDrawRect.w=m_rRealRect.w;
		sDrawRect.h=m_rRealRect.h*dTextureFactor;

		piRender->SetColor(CVector(1,1,1),1);

		piRender->SelectTexture(m_CompleteTexture.m_piTexture,0);
		piRender->RenderTextureRect(0,0,sDrawRect.w,sDrawRect.h,0,0,1.0,dTextureFactor);
		piRender->UnselectTexture(0);

		sDrawRect.w=m_rRealRect.w;
		sDrawRect.y=m_rRealRect.h*dTextureFactor;
		sDrawRect.h=m_rRealRect.h*(1.0-dTextureFactor);

		piRender->SelectTexture(m_BackgroundTexture.m_piTexture,0);
		piRender->RenderTextureRect(0,sDrawRect.y,sDrawRect.w,sDrawRect.h,0,dTextureFactor,1.0,1.0-dTextureFactor);
		piRender->UnselectTexture(0);

		if(m_ThumbWindowWrapper.m_piWindow && m_ThumbTexture.m_piTexture)
		{
			SGameRect sThumbRect,rParentRect;
			unsigned dwTextureWidth=0,dwTextureHeight=0;
			m_ThumbTexture.m_piTexture->GetSize(&dwTextureWidth,&dwTextureHeight);
			m_piParent->GetRealRect(&rParentRect);

			sThumbRect.x=m_rRealRect.x-rParentRect.x+m_rRealRect.w*0.5-dwTextureWidth*0.5;
			sThumbRect.y=m_rRealRect.y-rParentRect.y+m_rRealRect.h*dTextureFactor-dwTextureHeight*0.5;
			sThumbRect.w=dwTextureWidth;
			sThumbRect.h=dwTextureHeight;

			m_ThumbWindowWrapper.m_piWindow->SetRect(&sThumbRect);
		}

		// Se pintan los checkpoints

		for(unsigned x=0;x<m_vCheckPoints.size();x++)
		{
			IPlayAreaCheckPoint *piCheckPoint=m_vCheckPoints[x];
			IGenericTexture *piTextureToDraw=NULL;
			CVector vCheckPointPos=piCheckPoint->GetCheckPointPosition();
			double dCheckPointDistance=vCheckPointPos-vStart;
			if(dCheckPointDistance<=dCompletedDistance)
			{
				piTextureToDraw=m_CheckPointCompleteTexture.m_piTexture;
			}
			else
			{
				piTextureToDraw=m_CheckPointTexture.m_piTexture;
			}
			double dCheckPointFactor=m_dVerticalMargin+(1.0-m_dVerticalMargin*2.0)*(dCheckPointDistance/dTotalDistance);

			SGameRect sCheckPointRect;
			unsigned dwTextureWidth=0,dwTextureHeight=0;
			piTextureToDraw->GetSize(&dwTextureWidth,&dwTextureHeight);

			sCheckPointRect.x=0;
			sCheckPointRect.y=m_rRealRect.h*dCheckPointFactor-m_rRealRect.w*0.5;
			sCheckPointRect.w=m_rRealRect.w;
			sCheckPointRect.h=m_rRealRect.w;

			piRender->SelectTexture(piTextureToDraw,0);
			piRender->RenderTextureRect(sCheckPointRect.x,sCheckPointRect.y,sCheckPointRect.w,sCheckPointRect.h,0,0,1,1);
			piRender->UnselectTexture(0);
		}
	}
}

bool CScenarioProgressBar::InitWindow(IGameWindow *piParent,bool bPopup)
{
	bool bResult=CGameWindowBase::InitWindow(piParent,bPopup);
	if(bResult)
	{
		if(m_ThumbTexture.m_piTexture)
		{
			if(m_ThumbWindowWrapper.Create(m_piSystem,"CGameGUILabel","ProgressBarThumb"))
			{
				m_ThumbWindowWrapper.m_piWindow->InitWindow(m_piParent,false);
				m_ThumbWindowWrapper.m_piWindow->Show(true);
				m_ThumbWindowWrapper.m_piWindow->SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
				m_ThumbWindowWrapper.m_piWindow->SetBackgroundTexture(m_ThumbTexture.m_piTexture);
				m_ThumbWindowWrapper.m_piWindow->SetBackgroundColor(CVector(1,1,1),1);
			}
		}
	}
	return bResult;
}

void CScenarioProgressBar::DestroyWindow()
{
	for(unsigned x=0;x<m_vCheckPoints.size();x++)
	{
		IPlayAreaCheckPoint *piCheckPoint=m_vCheckPoints[x];
		REL(piCheckPoint);
	}
	m_vCheckPoints.clear();

	if(m_ThumbWindowWrapper.m_piWindow)
	{
		m_ThumbWindowWrapper.m_piWindow->DestroyWindow();
	}
	m_ThumbWindowWrapper.Detach();
	m_PlayAreaManagerWrapper.Detach();
	CGameWindowBase::DestroyWindow();
}

void CScenarioProgressBar::ProcessEnumeratedPlayAreaElement(IPlayAreaElement *piElement,bool *pbStopEnumerating)
{
	IPlayAreaCheckPoint *piCheckPoint=QI(IPlayAreaCheckPoint,piElement);
	if(piCheckPoint)
	{
		m_vCheckPoints.push_back(piCheckPoint);
	}
}
