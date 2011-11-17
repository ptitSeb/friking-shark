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
#include "EntityEditorObjectListSelector.h"

CEntityEditorObjectListSelector::CEntityEditorObjectListSelector(void)
{
	m_nSelectedObject=-1;
	m_pvObjects=NULL;
	
	InitializeChildren();
}

CEntityEditorObjectListSelector::~CEntityEditorObjectListSelector(void)
{
}

void CEntityEditorObjectListSelector::OnInitDialog()
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

void CEntityEditorObjectListSelector::EndDialog(int nResult)
{
	if(nResult==DIALOG_OK && m_nSelectedObject==-1){return;}	
	CGameDialogBase::EndDialog(nResult);
}

void CEntityEditorObjectListSelector::OnEndDialog()
{
	if(m_piLSObjects){m_piLSObjects->Clear();}
	
	CGameDialogBase::OnEndDialog();
}

bool CEntityEditorObjectListSelector::SelectObject(std::string sTitle,IGameWindow *piParent,std::vector<IDesignObject *> *vObjects,unsigned long *pnSelectedObject)
{
	m_sTitle=sTitle;
	m_pvObjects=vObjects;
	bool bOk=(Execute(piParent)==1);
	if(bOk && m_nSelectedObject!=-1 && pnSelectedObject){*pnSelectedObject=m_nSelectedObject;}
	return bOk;
}

void CEntityEditorObjectListSelector::OnSelectionChanged(IGameGUIList *piControl,int nElement,std::string sElement)
{
	m_nSelectedObject=nElement;
}

void CEntityEditorObjectListSelector::OnSelectionDoubleCliked(IGameGUIList *piControl,int nElement,std::string sElement)
{
	m_nSelectedObject=nElement;
	EndDialog(1);
}
