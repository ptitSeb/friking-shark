#include "./stdafx.h"
#include "GameGUILib.h"
#include "EntityEditorEntityPropertyPanel.h"

const char *g_ppBoundsTypeDescriptions[]={"None","BBox","BSP"};
const char *g_ppDamageTypeDescriptions[]={"None","Normal"};
const char *g_ppMovementTypeDescriptions[]={"None","Normal","Fly","Custom"};
const char *g_ppCollisionTypeDescriptions[]={"None","Stuck","Slide","Bounce","Through"};
const char *g_ppAlignmentDescriptions[]={"Neutral","Player","Enemy"};

CEntityEditorEntityPropertyPanel::CEntityEditorEntityPropertyPanel(void)
{
	InitializeChildren();
}

CEntityEditorEntityPropertyPanel::~CEntityEditorEntityPropertyPanel(void)
{
}

bool CEntityEditorEntityPropertyPanel::SetObject(ISystemObject *piObject)
{	
	m_Object.Detach();
	bool bOk=m_Object.Attach(piObject);
	UpdateData();
	return bOk;
}

void CEntityEditorEntityPropertyPanel::UpdateData()
{	
	if(m_Object.m_piEntityTypeDesign==NULL){return;}
	SEntityTypeConfig sConfig;
	m_Object.m_piEntityTypeDesign->GetEntityTypeConfig(&sConfig);
	
	char A[200];
	if(m_piSTHealth)       {sprintf(A,"Health:   %d",(int)sConfig.dMaxHealth);m_piSTHealth->SetText(A);}
	if(m_piSTVelocity)     {sprintf(A,"Velocity: %d",(int)sConfig.dMaxVelocity);m_piSTVelocity->SetText(A);}
	if(m_piBTAlignment)    {m_piBTAlignment->SetText(g_ppAlignmentDescriptions[sConfig.nAlignment]);}
	if(m_piBTBoundsType)   {m_piBTBoundsType->SetText(g_ppBoundsTypeDescriptions[sConfig.nBoundsType]);}
	if(m_piBTCollisionType){m_piBTCollisionType->SetText(g_ppCollisionTypeDescriptions[sConfig.nCollisionType]);}
	if(m_piBTMovementType) {m_piBTMovementType->SetText(g_ppMovementTypeDescriptions[sConfig.nMovementType]);}
	if(m_piBTDamageType)   {m_piBTDamageType->SetText(g_ppDamageTypeDescriptions[sConfig.nDamageType]);}
}

void CEntityEditorEntityPropertyPanel::OnButtonClicked(IGameGUIButton *piControl)
{
	if(m_Object.m_piEntityTypeDesign==NULL){return;}
	bool bUpdateConfig=false;
	SEntityTypeConfig sConfig;
	m_Object.m_piEntityTypeDesign->GetEntityTypeConfig(&sConfig);
	
	if(m_piBTHealthIncrease==piControl){bUpdateConfig=true;sConfig.dMaxHealth++;}
	if(m_piBTHealthDecrease==piControl){bUpdateConfig=true;sConfig.dMaxHealth--;if(sConfig.dMaxHealth<0){sConfig.dMaxHealth=0;}}
	if(m_piBTVelocityIncrease==piControl){bUpdateConfig=true;sConfig.dMaxVelocity++;}
	if(m_piBTVelocityDecrease==piControl){bUpdateConfig=true;sConfig.dMaxVelocity--;if(sConfig.dMaxVelocity<0){sConfig.dMaxVelocity=0;}}
	if(m_piBTDamageType==piControl){bUpdateConfig=true;sConfig.nDamageType++;if(sConfig.nDamageType>DAMAGE_TYPE_NORMAL){sConfig.nDamageType=DAMAGE_TYPE_NONE;}}
	if(m_piBTBoundsType==piControl){bUpdateConfig=true;sConfig.nBoundsType++;if(sConfig.nBoundsType>PHYSIC_BOUNDS_TYPE_BSP){sConfig.nBoundsType=PHYSIC_BOUNDS_TYPE_NONE;}}
	if(m_piBTMovementType==piControl){bUpdateConfig=true;sConfig.nMovementType++;if(sConfig.nMovementType>PHYSIC_MOVE_TYPE_CUSTOM){sConfig.nMovementType=PHYSIC_MOVE_TYPE_NONE;}}
	if(m_piBTCollisionType==piControl){bUpdateConfig=true;sConfig.nCollisionType++;if(sConfig.nCollisionType>PHYSIC_COLLISION_TYPE_THROUGH){sConfig.nCollisionType=PHYSIC_COLLISION_TYPE_NONE;}}
	if(m_piBTAlignment==piControl){bUpdateConfig=true;sConfig.nAlignment++;if(sConfig.nAlignment>ENTITY_ALIGNMENT_ENEMIES){sConfig.nAlignment=ENTITY_ALIGNMENT_NEUTRAL;}}
	
	if(bUpdateConfig)
	{
		m_Object.m_piEntityTypeDesign->SetEntityTypeConfig(&sConfig);
		UpdateData();
		NOTIFY_EVENT(IEntityEditorPropertyPanelEvents,OnObjectChanged(this,m_Object.m_piObject));
	}
}






