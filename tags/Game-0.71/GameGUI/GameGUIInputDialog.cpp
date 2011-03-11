#include "./stdafx.h"
#include "GameGUILib.h"
#include "GameGUIInputDialog.h"

CGameGUIInputDialog::CGameGUIInputDialog(void)
{
	InitializeChildren();
}

CGameGUIInputDialog::~CGameGUIInputDialog(void)
{
}

void CGameGUIInputDialog::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	if(m_piSTTitle){m_piSTTitle->SetText(m_sTitle);}
	if(m_piEDText){m_piEDText->SetText(m_sText);}
	if(m_piEDText){m_piGUIManager->SetFocus(m_piEDText);}
}

void CGameGUIInputDialog::OnEndDialog()
{
	CGameDialogBase::OnEndDialog();
}

void CGameGUIInputDialog::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTAccept){EndDialog(DIALOG_OK);}
	if(piControl==m_piBTCancel)
	{
		if(m_piEDText){m_sText=m_piEDText->GetText();}
		EndDialog(DIALOG_OK);
	}
}

void CGameGUIInputDialog::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_ESCAPE){EndDialog(DIALOG_CANCEL);*pbProcessed=true;}
	if(nKey==GK_RETURN)
	{
		if(m_piEDText){m_sText=m_piEDText->GetText();}
		EndDialog(DIALOG_OK);
		*pbProcessed=true;
	}
}

bool CGameGUIInputDialog::ShowInput(IGameWindow *piParent,std::string *psText,std::string sTitle)
{	
	m_sText=*psText;
	m_sTitle=sTitle;
	bool bOk=(Execute(piParent)==DIALOG_OK);
	if(bOk){*psText=m_sText;}
	return bOk;
}




