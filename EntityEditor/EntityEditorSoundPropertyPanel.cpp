#include "./stdafx.h"
#include "GameGUILib.h"
#include "EntityEditorSoundPropertyPanel.h"

CEntityEditorSoundPropertyPanel::CEntityEditorSoundPropertyPanel(void)
{
	InitializeChildren();
}

CEntityEditorSoundPropertyPanel::~CEntityEditorSoundPropertyPanel(void)
{
}

bool CEntityEditorSoundPropertyPanel::SetObject(ISystemObject *piObject)
{	
	m_Object.Detach();
	bool bOk=m_Object.Attach(piObject);
	UpdateData();
	return bOk;
}

void CEntityEditorSoundPropertyPanel::UpdateData()
{	
	if(m_Object.m_piDesign==NULL){return;}
	SSoundAnimationObjectTypeConfig sConfig;
	m_Object.m_piDesign->GetConfig(&sConfig);

	char A[200];
	if(m_piSTVolume)   {sprintf(A,"Vol  : %.02f%",sConfig.nVolume);m_piSTVolume->SetText(A);}
	if(m_piSTStartTime){sprintf(A,"Start: %.02f s",((double)sConfig.nStartTime)/1000.0);m_piSTStartTime->SetText(A);}
	if(m_piSTEndTime)  {sprintf(A,"End  : %.02f s",((double)sConfig.nEndTime)/1000.0);m_piSTEndTime->SetText(A);}
	if(m_piBTLoop){m_piBTLoop->SetText(sConfig.bLoop?"Loop : On":"Loop : Off");}
}

void CEntityEditorSoundPropertyPanel::OnButtonClicked(IGameGUIButton *piControl)
{
	if(m_Object.m_piDesign==NULL){return;}
	bool bUpdateConfig=false;
	SSoundAnimationObjectTypeConfig sConfig;
	m_Object.m_piDesign->GetConfig(&sConfig);

	if(m_piBTLoop==piControl){bUpdateConfig=true;sConfig.bLoop=!sConfig.bLoop;}
	if(m_piBTIncreaseVolume==piControl){bUpdateConfig=true;sConfig.nVolume+=5;if(sConfig.nVolume>100){sConfig.nVolume=100;}}
	if(m_piBTDecreaseVolume==piControl){bUpdateConfig=true;sConfig.nVolume-=5;if(sConfig.nVolume<0){sConfig.nVolume=0;}}
	if(m_piBTIncreaseStartTime==piControl){bUpdateConfig=true;sConfig.nStartTime+=100;}
	if(m_piBTDecreaseStartTime==piControl){bUpdateConfig=true;sConfig.nStartTime-=100;if(sConfig.nStartTime<0){sConfig.nStartTime=0;}}
	if(m_piBTIncreaseEndTime==piControl){bUpdateConfig=true;sConfig.nEndTime+=100;}
	if(m_piBTDecreaseEndTime==piControl){bUpdateConfig=true;sConfig.nEndTime-=100;if(sConfig.nEndTime<0){sConfig.nEndTime=0;}}

	if(bUpdateConfig)
	{
		m_Object.m_piDesign->SetConfig(&sConfig);
		UpdateData();
		NOTIFY_EVENT(IEntityEditorPropertyPanelEvents,OnObjectChanged(this,m_Object.m_piObject));
	}
	if(piControl==m_piBTRemove)
	{
		if(!ConfirmDialog("Remove sound "+m_Object.m_piObject->GetName()+"?","Entity Editor",eMessageDialogType_Warning)){return;}

		ISystemObject *piObject=ADD(m_Object.m_piObject);
		m_Object.Detach();
		NOTIFY_EVENT(IEntityEditorPropertyPanelEvents,OnObjectRemoved(this,piObject));
		REL(piObject);
	}
}
