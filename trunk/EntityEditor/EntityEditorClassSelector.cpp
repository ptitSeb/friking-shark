#include "./stdafx.h"
#include "GameGUILib.h"
#include "EntityEditorClassSelector.h"

CEntityEditorClassSelector::CEntityEditorClassSelector(void)
{
	InitializeChildren();
}

CEntityEditorClassSelector::~CEntityEditorClassSelector(void)
{
}

void CEntityEditorClassSelector::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	if(m_piSTTitle){m_piSTTitle->SetText(m_sTitle);}
	if(m_piLSClasses)
	{
		m_piGUIManager->SetFocus(m_piLSClasses);
		ISystemManager *piSystemManager=GetSystemManager();
		ISystem        *piSystem=piSystemManager?piSystemManager->GetSystem(m_sTargetSystem):NULL;
		if(piSystem)
		{
			std::vector<ISystemClass *> vClasses;
			piSystem->GetClasses(&vClasses);
			for(unsigned int x=0;x<vClasses.size();x++)
			{
				ISystemClass *piClass=vClasses[x];
				m_piLSClasses->AddElement(piClass->GetName());
				REL(piClass);
			}
		}	
		REL(piSystem);
		REL(piSystemManager);
	}
}

void CEntityEditorClassSelector::OnEndDialog()
{
	CGameDialogBase::OnEndDialog();
}

void CEntityEditorClassSelector::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTSelect){ProcessSelect();}
	if(piControl==m_piBTCancel){EndDialog(DIALOG_CANCEL);}
}

void CEntityEditorClassSelector::ProcessSelect()
{
	if(m_piLSClasses)
	{
		if(m_piLSClasses->GetSelectedElement()==-1)
		{
			MessageDialog((std::string)"Please select a class","Error",eMessageDialogType_Error);
			m_piGUIManager->SetFocus(m_piLSClasses);
			return;
		}
		else
		{
			m_sSelectedClass=m_piLSClasses->GetElement(m_piLSClasses->GetSelectedElement());
		}
	}
	EndDialog(DIALOG_OK);
}

void CEntityEditorClassSelector::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_RETURN){ProcessSelect();*pbProcessed=true;}
	if(nKey==GK_ESCAPE){EndDialog(DIALOG_CANCEL);*pbProcessed=true;}
}

bool CEntityEditorClassSelector::SelectClass(IGameWindow *piParent,std::string sTitle,std::string sSystem,std::string *psClass)
{	
	m_sSelectedClass="";
	m_sTitle=sTitle;
	m_sTargetSystem=sSystem;
	if(Execute(piParent)!=DIALOG_OK){return false;}
	*psClass=m_sSelectedClass;
	return true;
}

void CEntityEditorClassSelector::OnSelectionChanged(IGameGUIList *piControl,int nElement,std::string sElement)
{
	m_sSelectedClass=sElement;
}


void CEntityEditorClassSelector::OnSelectionDoubleCliked(IGameGUIList *piControl,int nElement,std::string sElement)
{
  ProcessSelect();
}






