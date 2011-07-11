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
#include "EntityEditorGeneralPropertyPanel.h"

const char *g_ppBoundsTypeDescriptions[]={"None","BBox","BSP"};
const char *g_ppDamageTypeDescriptions[]={"None","Normal"};
const char *g_ppMovementTypeDescriptions[]={"None","Normal","Fly","Custom"};
const char *g_ppCollisionTypeDescriptions[]={"None","Stuck","Slide","Bounce","Through"};
const char *g_ppAlignmentDescriptions[]={"Neutral","Player","Enemy"};
const char *g_ppPlacementDescriptions[]={"Ground","Air","Water"};

CEntityEditorGeneralPropertyPanel::CEntityEditorGeneralPropertyPanel(void)
{
	InitializeChildren();
}

CEntityEditorGeneralPropertyPanel::~CEntityEditorGeneralPropertyPanel(void)
{
}

bool CEntityEditorGeneralPropertyPanel::SetObject(ISystemObject *piObject)
{	
	m_Object.Detach();
	bool bOk=m_Object.Attach(piObject);
	UpdateData();
	return bOk;
}

void CEntityEditorGeneralPropertyPanel::UpdateData()
{	
	if(m_Object.m_piEntityTypeDesign==NULL){return;}
	SEntityTypeConfig sConfig;
	m_Object.m_piEntityTypeDesign->GetEntityTypeConfig(&sConfig);
	
	char A[200];
	if(m_piSTHealth)       {sprintf(A,"Health:   %d",(int)sConfig.dMaxHealth);m_piSTHealth->SetText(A);}
	if(m_piSTPoints)       {sprintf(A,"Points:   %d",sConfig.nPoints);m_piSTPoints->SetText(A);}
	if(m_piSTVelocity)     {sprintf(A,"Velocity: %d",(int)sConfig.dMaxVelocity);m_piSTVelocity->SetText(A);}
	if(m_piBTAlignment)    {m_piBTAlignment->SetText(g_ppAlignmentDescriptions[sConfig.nAlignment]);}
	if(m_piBTPlacement)    {m_piBTPlacement->SetText(g_ppPlacementDescriptions[sConfig.nPlacement]);}
	if(m_piBTBoundsType)   {m_piBTBoundsType->SetText(g_ppBoundsTypeDescriptions[sConfig.nBoundsType]);}
	if(m_piBTCollisionType){m_piBTCollisionType->SetText(g_ppCollisionTypeDescriptions[sConfig.nCollisionType]);}
	if(m_piBTMovementType) {m_piBTMovementType->SetText(g_ppMovementTypeDescriptions[sConfig.nMovementType]);}
	if(m_piBTDamageType)   {m_piBTDamageType->SetText(g_ppDamageTypeDescriptions[sConfig.nDamageType]);}
}

void CEntityEditorGeneralPropertyPanel::OnButtonClicked(IGameGUIButton *piControl)
{
	if(m_Object.m_piEntityTypeDesign==NULL){return;}
	bool bUpdateConfig=false;
	SEntityTypeConfig sConfig;
	m_Object.m_piEntityTypeDesign->GetEntityTypeConfig(&sConfig);
	
	double  dMultiplier=1;
	if(m_piGUIManager->IsKeyDown(GK_LCONTROL)){dMultiplier=10;}
	if(m_piGUIManager->IsKeyDown(GK_LSHIFT)){dMultiplier=100;}
	
	if(m_piBTHealthI	ncrease==piControl){bUpdateConfig=true;sConfig.dMaxHealth+=1*dMultiplier;}
	if(m_piBTHealthDecrease==piControl){bUpdateConfig=true;sConfig.dMaxHealth-=1*dMultiplier;if(sConfig.dMaxHealth<0){sConfig.dMaxHealth=0;}}
	if(m_piBTPointsIncrease==piControl){bUpdateConfig=true;sConfig.nPoints+=10*dMultiplier;}
	if(m_piBTPointsDecrease==piControl){bUpdateConfig=true;sConfig.nPoints-=10*dMultiplier;if(sConfig.nPoints<0){sConfig.nPoints=0;}}
	if(m_piBTVelocityIncrease==piControl){bUpdateConfig=true;sConfig.dMaxVelocity+=1*dMultiplier;}
	if(m_piBTVelocityDecrease==piControl){bUpdateConfig=true;sConfig.dMaxVelocity-=1*dMultiplier;if(sConfig.dMaxVelocity<0){sConfig.dMaxVelocity=0;}}
	if(m_piBTDamageType==piControl){bUpdateConfig=true;sConfig.nDamageType++;if(sConfig.nDamageType>DAMAGE_TYPE_NORMAL){sConfig.nDamageType=DAMAGE_TYPE_NONE;}}
	if(m_piBTBoundsType==piControl){bUpdateConfig=true;sConfig.nBoundsType++;if(sConfig.nBoundsType>PHYSIC_BOUNDS_TYPE_BSP){sConfig.nBoundsType=PHYSIC_BOUNDS_TYPE_NONE;}}
	if(m_piBTMovementType==piControl){bUpdateConfig=true;sConfig.nMovementType++;if(sConfig.nMovementType>PHYSIC_MOVE_TYPE_CUSTOM){sConfig.nMovementType=PHYSIC_MOVE_TYPE_NONE;}}
	if(m_piBTCollisionType==piControl){bUpdateConfig=true;sConfig.nCollisionType++;if(sConfig.nCollisionType>PHYSIC_COLLISION_TYPE_THROUGH){sConfig.nCollisionType=PHYSIC_COLLISION_TYPE_NONE;}}
	if(m_piBTAlignment==piControl){bUpdateConfig=true;sConfig.nAlignment++;if(sConfig.nAlignment>ENTITY_ALIGNMENT_ENEMIES){sConfig.nAlignment=ENTITY_ALIGNMENT_NEUTRAL;}}
	if(m_piBTPlacement==piControl){bUpdateConfig=true;sConfig.nPlacement++;if(sConfig.nPlacement>ENTITY_PLACEMENT_WATER){sConfig.nPlacement=ENTITY_PLACEMENT_GROUND;}}
	
	if(bUpdateConfig)
	{
		m_Object.m_piEntityTypeDesign->SetEntityTypeConfig(&sConfig);
		UpdateData();
		NOTIFY_EVENT(IEntityEditorPropertyPanelEvents,OnObjectChanged(this,m_Object.m_piObject));
	}
}






