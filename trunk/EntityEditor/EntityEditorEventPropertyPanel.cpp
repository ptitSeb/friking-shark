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
	if(m_piBTDecreaseTime==piControl){bUpdateConfig=true;sConfig.nTime-=100;if(sConfig.nTime<0){sConfig.nTime=0;}}
	if(bUpdateConfig)
	{
		m_Object.m_piDesign->SetConfig(&sConfig);
		UpdateData();

		NOTIFY_EVENT(IEntityEditorPropertyPanelEvents,OnObjectChanged(this,m_Object.m_piObject));
	}
	if(piControl==m_piBTRemove)
	{
		if(!ConfirmDialog("Remove event "+sConfig.sName+"?","Entity Editor",eMessageDialogType_Warning)){return;}

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
