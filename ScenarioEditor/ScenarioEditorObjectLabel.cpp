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


#include "./stdafx.h"
#include "ScenarioEditorObjectLabel.h"

CScenarioEditorObjectLabel::CScenarioEditorObjectLabel(void)
{
	m_piDesignObject=NULL;
}

CScenarioEditorObjectLabel::~CScenarioEditorObjectLabel(void)
{
}

void CScenarioEditorObjectLabel::OnDrawBackground( IGenericRender *piRender )
{
	piRender->Clear(m_vBackgroundColor,m_dBackgroundAlpha);
	CVector vMaxs,vMins;
	if(m_piDesignObject){m_piDesignObject->DesignGetAABBox(Origin,Origin,&vMins,&vMaxs);}
	CVector vSize=(vMaxs-vMins)*1.2;
	CVector vCenter=(vMaxs+vMins)*0.5;
	vCenter.c[1]=vMaxs.c[1]+10;
	double dMaxSize=max(vSize.c[0],vSize.c[2]);
	double dAspectRatio=m_rRealRect.h?m_rRealRect.w/m_rRealRect.h:0;
	piRender->SetOrthographicProjection(dMaxSize*dAspectRatio,dMaxSize);
	piRender->SetCamera(vCenter+CVector(0,vSize.c[1]+10,0),0,-90,0);
	piRender->PushState();
	piRender->ActivateDepth();
	if(m_piDesignObject){m_piDesignObject->DesignRender(piRender,Origin,m_vVisualizationAngles,false);}

	if(m_FrameManager.m_piFrameManager==NULL){m_FrameManager.Attach("GameSystem","FrameManager");}
	if(m_FrameManager.m_piFrameManager){m_vVisualizationAngles.c[YAW]+=30.0*m_FrameManager.m_piFrameManager->GetRealTimeFraction();}
	piRender->PopState();
}	

void CScenarioEditorObjectLabel::SetObject(IDesignObject *piObject)
{
	REL(m_piDesignObject);
	m_piDesignObject=ADD(piObject);
}

void CScenarioEditorObjectLabel::DestroyWindow()
{
	REL(m_piDesignObject);
	CGameWindowBase::DestroyWindow();
}

bool CScenarioEditorObjectLabel::InitWindow( IGameWindow *piParent,bool bPopup )
{
	bool bOk=CGameWindowBase::InitWindow(piParent,bPopup);
	return bOk;
}