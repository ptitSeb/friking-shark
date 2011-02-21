#include "./stdafx.h"
#include "GameGUILib.h"
#include "GameGUIMessageDialog.h"

CGameGUIMessageDialog::CGameGUIMessageDialog(void)
{
	m_piBTAccept=NULL;
	m_piSTTitle=NULL;
	m_piSTText=NULL;
	m_eType=eMessageDialogType_Info;
}

CGameGUIMessageDialog::~CGameGUIMessageDialog(void)
{
}

void CGameGUIMessageDialog::OnInitDialog()
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

void CGameGUIMessageDialog::OnEndDialog()
{
	CGameDialogBase::OnEndDialog();
}

void CGameGUIMessageDialog::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTAccept){EndDialog(IDOK);}
}

void CGameGUIMessageDialog::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==VK_ESCAPE){EndDialog(IDOK);*pbProcessed=true;}
	if(nKey==VK_RETURN){EndDialog(IDOK);*pbProcessed=true;}
}

void CGameGUIMessageDialog::ShowMessage(IGameWindow *piParent,std::string sText,std::string sTitle,EMessageDialogType eType)
{	
	m_eType=eType;
	m_sText=sText;
	m_sTitle=sTitle;
	Execute(piParent);
}




