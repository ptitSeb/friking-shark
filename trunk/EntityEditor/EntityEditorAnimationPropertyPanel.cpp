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
#include "EntityEditorAnimationPropertyPanel.h"

CEntityEditorAnimationPropertyPanel::CEntityEditorAnimationPropertyPanel(void)
{
	InitializeChildren();
}

CEntityEditorAnimationPropertyPanel::~CEntityEditorAnimationPropertyPanel(void)
{
}

bool CEntityEditorAnimationPropertyPanel::SetObject(ISystemObject *piObject)
{	
	m_Object.Detach();
	bool bOk=m_Object.Attach(piObject);
	UpdateData();
	return bOk;
}

void CEntityEditorAnimationPropertyPanel::UpdateData()
{	
	if(m_Object.m_piAnimationTypeDesign==NULL){return;}
	SAnimationTypeConfig sConfig;
	m_Object.m_piAnimationTypeDesign->GetAnimationTypeConfig(&sConfig);

	if(m_piBTLoop){m_piBTLoop->SetText(sConfig.bLoop?"Loop: On":"Loop: Off");}
}

void CEntityEditorAnimationPropertyPanel::OnButtonClicked(IGameGUIButton *piControl)
{
	if(m_Object.m_piAnimationTypeDesign==NULL){return;}
	bool bUpdateConfig=false;
	SAnimationTypeConfig sConfig;
	m_Object.m_piAnimationTypeDesign->GetAnimationTypeConfig(&sConfig);

	if(m_piBTLoop==piControl){bUpdateConfig=true;sConfig.bLoop=!sConfig.bLoop;}

	if(bUpdateConfig)
	{
		m_Object.m_piAnimationTypeDesign->SetAnimationTypeConfig(&sConfig);
		UpdateData();
		NOTIFY_EVENT(IEntityEditorPropertyPanelEvents,OnObjectChanged(this,m_Object.m_piObject));
	}
	if(piControl==m_piBTRemove)
	{
		if(!ConfirmDialog("Remove animation "+m_Object.m_piObject->GetName()+"?","Entity Editor",eMessageDialogType_Warning)){return;}

		ISystemObject *piObject=ADD(m_Object.m_piObject);
		m_Object.Detach();
		NOTIFY_EVENT(IEntityEditorPropertyPanelEvents,OnObjectRemoved(this,piObject));
		REL(piObject);
	}
}
