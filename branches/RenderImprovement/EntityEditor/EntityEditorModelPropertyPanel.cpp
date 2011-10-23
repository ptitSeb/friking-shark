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
#include "GameGUILib.h"
#include "EntityEditorModelPropertyPanel.h"

CEntityEditorModelPropertyPanel::CEntityEditorModelPropertyPanel(void)
{
	InitializeChildren();
}

CEntityEditorModelPropertyPanel::~CEntityEditorModelPropertyPanel(void)
{
}

bool CEntityEditorModelPropertyPanel::SetObject(ISystemObject *piObject)
{	
	m_Object.Detach();
	bool bOk=m_Object.Attach(piObject);
	UpdateData();
	return bOk;
}

void CEntityEditorModelPropertyPanel::UpdateData()
{	
	if(m_Object.m_piDesign==NULL){return;}
	SModelAnimationObjectTypeConfig sConfig;
	m_Object.m_piDesign->GetConfig(&sConfig);

	char A[200];
	if(m_piSTFps)      {sprintf(A,"Fps  : %.02f",sConfig.dFps);m_piSTFps->SetText(A);}
	if(m_piSTStartTime){sprintf(A,"Start: %.02f s",((double)sConfig.nStartTime)/1000.0);m_piSTStartTime->SetText(A);}
	if(m_piSTEndTime)  {sprintf(A,"End  : %.02f s",((double)sConfig.nEndTime)/1000.0);m_piSTEndTime->SetText(A);}
	if(m_piBTLoop){m_piBTLoop->SetText(sConfig.bLoop?"Loop : On":"Loop : Off");}
	if(m_piBTCastShadow){m_piBTCastShadow->SetText(sConfig.bCastShadow?"Shadow Emission : On":"Shadow Emission : Off");}
	if(m_piBTReceiveShadows){m_piBTReceiveShadows->SetText(sConfig.bReceiveShadows?"Shadow Reception: On":"Shadow Reception: Off");}
}

void CEntityEditorModelPropertyPanel::OnButtonClicked(IGameGUIButton *piControl)
{
	if(m_Object.m_piDesign==NULL){return;}
	bool bUpdateConfig=false;
	SModelAnimationObjectTypeConfig sConfig;
	m_Object.m_piDesign->GetConfig(&sConfig);

	if(m_piBTCastShadow==piControl){bUpdateConfig=true;sConfig.bCastShadow=!sConfig.bCastShadow;}
	if(m_piBTReceiveShadows==piControl){bUpdateConfig=true;sConfig.bReceiveShadows=!sConfig.bReceiveShadows;}
	if(m_piBTLoop==piControl){bUpdateConfig=true;sConfig.bLoop=!sConfig.bLoop;}
	if(m_piBTIncreaseFps==piControl){bUpdateConfig=true;sConfig.dFps+=5.0;}
	if(m_piBTDecreaseFps==piControl){bUpdateConfig=true;sConfig.dFps-=5.0;if(sConfig.dFps<5.0){sConfig.dFps=5.0;}}
	if(m_piBTIncreaseStartTime==piControl){bUpdateConfig=true;sConfig.nStartTime+=100;}
	if(m_piBTDecreaseStartTime==piControl){bUpdateConfig=true;sConfig.nStartTime=(sConfig.nStartTime<100)?0:sConfig.nStartTime-100;}
	if(m_piBTIncreaseEndTime==piControl){bUpdateConfig=true;sConfig.nEndTime+=100;}
	if(m_piBTDecreaseEndTime==piControl){bUpdateConfig=true;sConfig.nEndTime=(sConfig.nEndTime<100)?0:sConfig.nEndTime-100;}

	if(bUpdateConfig)
	{
		m_Object.m_piDesign->SetConfig(&sConfig);
		UpdateData();
		NOTIFY_EVENT(IEntityEditorPropertyPanelEvents,OnObjectChanged(this,m_Object.m_piObject));
	}
	if(piControl==m_piBTRemove)
	{
		if(!ConfirmDialog("Remove model '"+m_Object.m_piDesign->GetAnimationObjectDescription()+"' ?","Entity Editor",eMessageDialogType_Warning)){return;}

		ISystemObject *piObject=ADD(m_Object.m_piObject);
		m_Object.Detach();
		NOTIFY_EVENT(IEntityEditorPropertyPanelEvents,OnObjectRemoved(this,piObject));
		REL(piObject);
	}
}
