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
#include "EntityEditorParticleSystemPropertyPanel.h"

CEntityEditorParticleSystemPropertyPanel::CEntityEditorParticleSystemPropertyPanel(void)
{
	InitializeChildren();
}

CEntityEditorParticleSystemPropertyPanel::~CEntityEditorParticleSystemPropertyPanel(void)
{
}

bool CEntityEditorParticleSystemPropertyPanel::SetObject(ISystemObject *piObject)
{	
	m_Object.Detach();
	bool bOk=m_Object.Attach(piObject);
	UpdateData();
	return bOk;
}

void CEntityEditorParticleSystemPropertyPanel::UpdateData()
{	
	if(m_Object.m_piDesign==NULL){return;}
	SParticleSystemAnimationObjectTypeConfig sConfig;
	m_Object.m_piDesign->GetConfig(&sConfig);

	char A[200];
	if(m_piSTStartTime){sprintf(A,"Start: %.02f s",((double)sConfig.nStartTime)/1000.0);m_piSTStartTime->SetText(A);}
	if(m_piSTEndTime)  {sprintf(A,"End  : %.02f s",((double)sConfig.nEndTime)/1000.0);m_piSTEndTime->SetText(A);}
	if(m_piBTTrackEntity){m_piBTTrackEntity->SetText(sConfig.bTrackEntity?"Track: On":"Track: Off");}
}

void CEntityEditorParticleSystemPropertyPanel::OnButtonClicked(IGameGUIButton *piControl)
{
	if(m_Object.m_piDesign==NULL){return;}
	bool bUpdateConfig=false;
	SParticleSystemAnimationObjectTypeConfig sConfig;
	m_Object.m_piDesign->GetConfig(&sConfig);

	if(m_piBTTrackEntity==piControl){bUpdateConfig=true;sConfig.bTrackEntity=!sConfig.bTrackEntity;}
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
		if(!ConfirmDialog("Remove particle system '"+m_Object.m_piDesign->GetAnimationObjectDescription()+"' ?","Entity Editor",eMessageDialogType_Warning)){return;}

		ISystemObject *piObject=ADD(m_Object.m_piObject);
		m_Object.Detach();
		NOTIFY_EVENT(IEntityEditorPropertyPanelEvents,OnObjectRemoved(this,piObject));
		REL(piObject);
	}
}
