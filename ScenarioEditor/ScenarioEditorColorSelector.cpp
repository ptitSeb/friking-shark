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
#include "ScenarioEditorColorSelector.h"

CScenarioEditorColorSelector::CScenarioEditorColorSelector(void)
{
	m_piColorMapTexture=NULL;
	InitializeChildren();
}

CScenarioEditorColorSelector::~CScenarioEditorColorSelector(void)
{
}

void CScenarioEditorColorSelector::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	if(m_piSTColorMap)
	{
		m_piSTColorMap->SetBackgroundTexture(m_piColorMapTexture);
		m_piSTColorMap->Activate(false);
	}
}

bool CScenarioEditorColorSelector::SelectColor(IGameWindow *piParent,IGenericTexture *piColorMap,CVector *pOutColor)
{
	m_vColor=*pOutColor;
	m_piColorMapTexture=piColorMap;
	bool bOk=(Execute(piParent)==1);
	if(bOk){*pOutColor=m_vColor;}
	m_piColorMapTexture=NULL;
	return bOk;
}

void CScenarioEditorColorSelector::OnMouseDown( int nButton,double x,double y )
{
	if(m_piSTColorMap==NULL||m_piColorMapTexture==NULL)
	{
		EndDialog(0);
		return;
	}

	unsigned int nTextureWidth=0,nTextureHeight=0;
	m_piColorMapTexture->GetSize(&nTextureWidth,&nTextureHeight);
	SGameRect sRect,sDialogRect;
	GetRealRect(&sDialogRect);
	m_piSTColorMap->GetRealRect(&sRect);
	x+=sDialogRect.x;
	y+=sDialogRect.y;

	if(sRect.Contains(SGamePos(x,y)))
	{
		// Remove position.
		x-=sRect.x;
		y-=sRect.y;
		// clamp to 0-1
		x/=sRect.w;
		y/=sRect.h;
		// 0-1 to texture size
		x*=(double)nTextureWidth;
		y*=(double)nTextureHeight;
		m_vColor=m_piColorMapTexture->GetPixelColor((unsigned long)x,(unsigned long)y);
		EndDialog(1);
	}
}