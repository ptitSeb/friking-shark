#include "./stdafx.h"
#include "GameGUILib.h"
#include "EntityEditorNamedObjectSelector.h"

CEntityEditorNamedObjectSelector::CEntityEditorNamedObjectSelector(void)
{
	InitializeChildren();
}

CEntityEditorNamedObjectSelector::~CEntityEditorNamedObjectSelector(void)
{
}

void CEntityEditorNamedObjectSelector::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	if(m_piSTTitle){m_piSTTitle->SetText(m_sTitle);}
	if(m_piLSObjects)
	{
		m_piGUIManager->SetFocus(m_piLSObjects);
		ISystemManager *piSystemManager=GetSystemManager();
		ISystem        *piSystem=piSystemManager?piSystemManager->GetSystem(m_sTargetSystem):NULL;
		if(piSystem)
		{
			std::vector<ISystemObject *> vObjects;
			piSystem->GetObjects(&vObjects);
			for(unsigned int x=0;x<vObjects.size();x++)
			{
				ISystemObject *piObject=vObjects[x];
				m_piLSObjects->AddElement(piObject->GetName());
				REL(piObject);
			}
		}	
		REL(piSystem);
		REL(piSystemManager);
	}
}

void CEntityEditorNamedObjectSelector::OnEndDialog()
{
	CGameDialogBase::OnEndDialog();
}

void CEntityEditorNamedObjectSelector::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTSelect){ProcessSelect();}
	if(piControl==m_piBTCancel){EndDialog(DIALOG_CANCEL);}
}

void CEntityEditorNamedObjectSelector::ProcessSelect()
{
	if(m_piLSObjects)
	{
		if(m_piLSObjects->GetSelectedElement()==-1)
		{
			MessageDialog((std::string)"Please select a class","Error",eMessageDialogType_Error);
			m_piGUIManager->SetFocus(m_piLSObjects);
			return;
		}
		else
		{
			m_sSelectedObject=m_piLSObjects->GetElement(m_piLSObjects->GetSelectedElement());
		}
	}
	EndDialog(DIALOG_OK);
}

void CEntityEditorNamedObjectSelector::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_RETURN){ProcessSelect();*pbProcessed=true;}
	if(nKey==GK_ESCAPE){EndDialog(DIALOG_CANCEL);*pbProcessed=true;}
}

bool CEntityEditorNamedObjectSelector::SelectObject(IGameWindow *piParent,std::string sTitle,std::string sSystem,std::string *psSelectedObject)
{	
	m_sSelectedObject="";
	m_sTitle=sTitle;
	m_sTargetSystem=sSystem;
	if(Execute(piParent)!=DIALOG_OK){return false;}
	*psSelectedObject=m_sSelectedObject;
	return true;
}

void CEntityEditorNamedObjectSelector::OnSelectionChanged(IGameGUIList *piControl,int nElement,std::string sElement)
{
	m_sSelectedObject=sElement;
}


void CEntityEditorNamedObjectSelector::OnSelectionDoubleCliked(IGameGUIList *piControl,int nElement,std::string sElement)
{
	ProcessSelect();
}


