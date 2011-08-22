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
#include "EntityEditorEventPropertyPanel.h"

CEntityEditorEventPropertyPanel::CEntityEditorEventPropertyPanel(void)
{
	InitializeChildren();
}

CEntityEditorEventPropertyPanel::~CEntityEditorEventPropertyPanel(void)
{
}

bool CEntityEditorEventPropertyPanel::SetObject(ISystemObject *piObject)
{	
	m_Object.Detach();
	bool bOk=m_Object.Attach(piObject);
	UpdateData();
	return bOk;
}

void CEntityEditorEventPropertyPanel::UpdateData()
{	
	if(m_Object.m_piDesign==NULL){return;}
	SEventAnimationObjectTypeConfig sConfig;
	m_Object.m_piDesign->GetConfig(&sConfig);
	
	char A[200];
	if(m_piEDName)  {m_piEDName->SetText(sConfig.sName);}
	if(m_piEDParams){m_piEDParams->SetText(sConfig.sParams);}
	if(m_piSTTime)	{sprintf(A,"Time: %.02f s",((double)sConfig.nTime)/1000.0);m_piSTTime->SetText(A);}
}

void CEntityEditorEventPropertyPanel::OnButtonClicked(IGameGUIButton *piControl)
{
	if(m_Object.m_piDesign==NULL){return;}
	bool bUpdateConfig=false;
	SEventAnimationObjectTypeConfig sConfig;
	m_Object.m_piDesign->GetConfig(&sConfig);
	if(m_piBTIncreaseTime==piControl){bUpdateConfig=true;sConfig.nTime+=100;}
	if(m_piBTDecreaseTime==piControl){bUpdateConfig=true;sConfig.nTime=(sConfig.nTime<100)?0:sConfig.nTime-100;}
	if(bUpdateConfig)
	{
		m_Object.m_piDesign->SetConfig(&sConfig);
		UpdateData();

		NOTIFY_EVENT(IEntityEditorPropertyPanelEvents,OnObjectChanged(this,m_Object.m_piObject));
	}
	if(piControl==m_piBTRemove)
	{
		if(!ConfirmDialog("Remove event '"+m_Object.m_piDesign->GetAnimationObjectDescription()+"' ?","Entity Editor",eMessageDialogType_Warning)){return;}

		ISystemObject *piObject=ADD(m_Object.m_piObject);
		m_Object.Detach();
		NOTIFY_EVENT(IEntityEditorPropertyPanelEvents,OnObjectRemoved(this,piObject));
		REL(piObject);
	}
}

void CEntityEditorEventPropertyPanel::OnTextChanged(IGameGUIEdit *piControl,std::string sNewText)
{
	if(m_Object.m_piDesign==NULL){return;}
	SEventAnimationObjectTypeConfig sConfig;
	m_Object.m_piDesign->GetConfig(&sConfig);
	if(piControl==m_piEDName){sConfig.sName=sNewText;}
	if(piControl==m_piEDParams){sConfig.sParams=sNewText;}
	m_Object.m_piDesign->SetConfig(&sConfig);
	NOTIFY_EVENT(IEntityEditorPropertyPanelEvents,OnObjectChanged(this,m_Object.m_piObject));
}
