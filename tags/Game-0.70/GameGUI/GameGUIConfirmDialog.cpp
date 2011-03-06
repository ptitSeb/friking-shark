#include "./stdafx.h"
#include "GameGUILib.h"
#include "GameGUIConfirmDialog.h"

CGameGUIConfirmDialog::CGameGUIConfirmDialog(void)
{
	m_eType=eMessageDialogType_Info;
	InitializeChildren();
}

CGameGUIConfirmDialog::~CGameGUIConfirmDialog(void)
{
}

void CGameGUIConfirmDialog::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	if(m_piSTTitle)
	{
	  CVector vColor;
	  double dAlpha=0;
	  m_piSTTitle->GetBackgroundColor(&vColor,&dAlpha);
	  switch(m_eType)
	  {
		case eMessageDialogType_Warning:m_piSTTitle->SetBackgroundColor(m_vWarningBkColor,dAlpha);break;
		case eMessageDialogType_Error:m_piSTTitle->SetBackgroundColor(m_vErrorBkColor,dAlpha);break;
		case eMessageDialogType_Question:m_piSTTitle->SetBackgroundColor(m_vQuestionBKColor,dAlpha);break;
		case eMessageDialogType_Info:m_piSTTitle->SetBackgroundColor(m_vInfoBkColor,dAlpha);break;
		default:m_piSTTitle->SetBackgroundColor(m_vInfoBkColor,dAlpha);break;
	  };
	  m_piSTTitle->SetText(m_sTitle);
	}
	if(m_piSTText){m_piSTText->SetText(m_sText);}	
}

void CGameGUIConfirmDialog::OnEndDialog()
{
	CGameDialogBase::OnEndDialog();
}

void CGameGUIConfirmDialog::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTYes){EndDialog(DIALOG_OK);}
	if(piControl==m_piBTNo){EndDialog(DIALOG_CANCEL);}
}

void CGameGUIConfirmDialog::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_ESCAPE){EndDialog(DIALOG_CANCEL);*pbProcessed=true;}
	if(nKey==GK_RETURN){EndDialog(DIALOG_OK);*pbProcessed=true;}
}

bool CGameGUIConfirmDialog::Confirm(IGameWindow *piParent,std::string sText,std::string sTitle,EMessageDialogType eType)
{	
	m_eType=eType;
	m_sText=sText;
	m_sTitle=sTitle;
	return Execute(piParent)==DIALOG_OK;
}




