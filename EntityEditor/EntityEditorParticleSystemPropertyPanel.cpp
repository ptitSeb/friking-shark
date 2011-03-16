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
