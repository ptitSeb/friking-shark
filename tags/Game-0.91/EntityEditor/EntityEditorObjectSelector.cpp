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
#include "EntityEditorObjectSelector.h"

#define BUTTON_INTERNAL_MARGIN	4.0
#define BUTTON_MARGIN			3.0

CEntityEditorObjectSelector::CEntityEditorObjectSelector(void)
{
	m_nSelectedObject=0;
	m_pvObjects=NULL;
	m_dButtonSizeX=DEFAULT_OBJECT_SELECTOR_BUTTON_SIZE;
	m_dButtonSizeY=DEFAULT_OBJECT_SELECTOR_BUTTON_SIZE;

	InitializeChildren();
}

CEntityEditorObjectSelector::~CEntityEditorObjectSelector(void)
{
}

void CEntityEditorObjectSelector::OnDraw(IGenericRender *piRender)
{
	if(m_piSTObjectList)
	{
		SGameRect sListRect;
		m_piSTObjectList->GetRealRect(&sListRect);
		double dCurrentX=BUTTON_MARGIN;
		double dCurrentY=sListRect.h-(m_dButtonSizeY+BUTTON_MARGIN);
		for(unsigned long x=0;x<m_vButtons.size();x++)
		{
			if(dCurrentX+m_dButtonSizeX+BUTTON_MARGIN>sListRect.w){dCurrentX=BUTTON_MARGIN;dCurrentY-=m_dButtonSizeY+BUTTON_MARGIN*2.0;}
			SGameRect rRect(dCurrentX,dCurrentY,m_dButtonSizeX,m_dButtonSizeY);
			m_vButtons[x].m_piButton->SetRect(&rRect);

			SGameRect rLabelRect(BUTTON_INTERNAL_MARGIN , BUTTON_INTERNAL_MARGIN , m_dButtonSizeX-(BUTTON_INTERNAL_MARGIN*2.0) , m_dButtonSizeY-(BUTTON_INTERNAL_MARGIN*2.0));
			m_vLabels[x].m_piLabel->SetRect(&rLabelRect);
			dCurrentX+=m_dButtonSizeX+BUTTON_MARGIN*2.0;
		}
	}
	CGameWindowBase::OnDraw(piRender);
}

void CEntityEditorObjectSelector::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	if(m_piSTTitle)
	{
		m_piSTTitle->SetText(m_sTitle);
	}
		
	if(m_piSTObjectList)
	{
		for(unsigned long x=0;x<m_pvObjects->size();x++)
		{
			CButtonWrapper button;
			button.Create(m_piSystem,"CGameGUIButton","");
			if(button.m_piButton)
			{
				button.m_piButton->InitWindow(m_piSTObjectList,false);
				button.m_piButton->SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
				button.m_piButton->SetBackgroundColor(CVector(1,1,1),0.8);
				button.m_piButton->Show(true);
				button.m_piButton->Activate(true);
				SUBSCRIBE_TO_CAST(button.m_piButton,IGameGUIButtonEvents);
				m_vButtons.push_back(button);
			}
			CObjectLabelWrapper label;
			label.Create(m_piSystem,"CEntityEditorObjectLabel","");
			if(button.m_piButton)
			{
				label.m_piLabel->InitWindow(button.m_piButton,false);
				label.m_piLabel->SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
				label.m_piLabel->SetBackgroundColor(CVector(0,0,0),0);
				label.m_piLabel->Show(true);
				label.m_piLabel->Activate(false);
				label.m_piLabel->SetObject((*m_pvObjects)[x]);
				m_vLabels.push_back(label);
			}
			label.Detach();
			button.Detach();
		}
	}
}

void CEntityEditorObjectSelector::OnEndDialog()
{
	for(unsigned long x=0;x<m_vButtons.size();x++)
	{
		m_vButtons[x].m_piButton->DestroyWindow();		
		m_vButtons[x].m_piObject->Destroy();
	}
	for(unsigned long x=0;x<m_vLabels.size();x++)
	{
		m_vLabels[x].m_piLabel->DestroyWindow();		
		m_vLabels[x].m_piObject->Destroy();
	}
	m_vButtons.clear();
	m_vLabels.clear();

	CGameDialogBase::OnEndDialog();
}

bool CEntityEditorObjectSelector::SelectObject(std::string sTitle,IGameWindow *piParent,std::vector<IDesignObject *> *vObjects,unsigned long *pnSelectedObject,double dButtonSizeX,double dButtonSizeY)
{
	if(pnSelectedObject){*pnSelectedObject=0;}
	m_sTitle=sTitle;
	m_pvObjects=vObjects;
	m_dButtonSizeX=dButtonSizeX;
	m_dButtonSizeY=dButtonSizeY;
	bool bOk=(Execute(piParent)==1);
	if(bOk && pnSelectedObject){*pnSelectedObject=m_nSelectedObject;}
	return bOk;
}

void CEntityEditorObjectSelector::OnButtonClicked(IGameGUIButton *piControl)
{
	for(unsigned long x=0;x<m_vButtons.size();x++)
	{
		if(piControl==m_vButtons[x].m_piButton)
		{
			m_nSelectedObject=x;
		}
	}
	EndDialog(1);
}
