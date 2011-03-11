#include "./stdafx.h"
#include "GameWindowBase.h"
#include "GameDialogBase.h"

CGameDialogBase::CGameDialogBase(void)
{
	m_bExecuting=false;
	m_nDialogResult=DIALOG_OK;
}

CGameDialogBase::~CGameDialogBase(void)
{
}

int	CGameDialogBase::Execute(IGameWindow *piParent)
{
	if(CGameWindowBase::InitWindow(piParent,true))
	{
		m_piGUIManager->SetFocus(this);
		OnInitDialog();
		Show(true);
		m_piGUIManager->EnterGUILoop();
		Show(false);
		OnEndDialog();
		CGameWindowBase::DestroyWindow();
		return m_nDialogResult;
	}
	else
	{
		return DIALOG_CANCEL;
	}
}

void CGameDialogBase::EndDialog(int nResult)
{
	m_nDialogResult=nResult;
	m_piGUIManager->ExitGUILoop();
}

void CGameDialogBase::OnInitDialog()
{
}

void CGameDialogBase::OnEndDialog()
{
}

void CGameDialogBase::OnWantFocus(bool *pbWant){*pbWant=true;}

void CGameDialogBase::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_ESCAPE)
	{
		EndDialog(DIALOG_CANCEL);
		(*pbProcessed)=true;
	}
	else if(nKey==GK_RETURN)
	{
		EndDialog(DIALOG_OK);
		(*pbProcessed)=true;
	}
}
