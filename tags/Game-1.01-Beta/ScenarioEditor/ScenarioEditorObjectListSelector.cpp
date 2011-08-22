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
#include "ScenarioEditorObjectListSelector.h"

CScenarioEditorObjectListSelector::CScenarioEditorObjectListSelector(void)
{
	m_nSelectedObject=0;
	m_pvObjects=NULL;
	
	InitializeChildren();
}

CScenarioEditorObjectListSelector::~CScenarioEditorObjectListSelector(void)
{
}

void CScenarioEditorObjectListSelector::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	if(m_piSTTitle)
	{
		m_piSTTitle->SetText(m_sTitle);
	}
	
	if(m_piLSObjects)
	{
		m_piLSObjects->Clear();
		for(unsigned long x=0;x<m_pvObjects->size();x++)
		{
			ISystemObject *piObject=QI(ISystemObject,(*m_pvObjects)[x]);
			if(piObject){m_piLSObjects->AddElement(piObject->GetName());}
			REL(piObject);
		}
		m_piLSObjects->SetSelectedElement(m_nSelectedObject);
		m_piGUIManager->SetFocus(m_piLSObjects);
	}
}

void CScenarioEditorObjectListSelector::OnEndDialog()
{
	if(m_piLSObjects){m_piLSObjects->Clear();}
	
	CGameDialogBase::OnEndDialog();
}

bool CScenarioEditorObjectListSelector::SelectObject(std::string sTitle,IGameWindow *piParent,std::vector<IDesignObject *> *vObjects,unsigned long *pnSelectedObject)
{
	if(pnSelectedObject){*pnSelectedObject=0;}
	m_sTitle=sTitle;
	m_pvObjects=vObjects;
	bool bOk=(Execute(piParent)==1);
	if(bOk && pnSelectedObject){*pnSelectedObject=m_nSelectedObject;}
	return bOk;
}

void CScenarioEditorObjectListSelector::OnSelectionChanged(IGameGUIList *piControl,int nElement,std::string sElement)
{
	m_nSelectedObject=nElement;
}

void CScenarioEditorObjectListSelector::OnSelectionDoubleCliked(IGameGUIList *piControl,int nElement,std::string sElement)
{
	m_nSelectedObject=nElement;
	EndDialog(1);
}
