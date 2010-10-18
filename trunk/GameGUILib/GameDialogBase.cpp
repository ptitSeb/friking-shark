#include "StdAfx.h"
#include ".\gamedialogbase.h"

CGameDialogBase::CGameDialogBase(void)
{
	m_bExecuting=false;
	m_nDialogResult=IDOK;
}

CGameDialogBase::~CGameDialogBase(void)
{
}
int	CGameDialogBase::Execute(IGameWindow *piParent)
{
	if(CGameWindowBase::InitWindow(piParent,true))
	{
		OnInitDialog();
		Show(true);
		m_piGUIManager->SetFocus(this);
		m_piGUIManager->EnterGUILoop();
		Show(false);
		OnEndDialog();
		CGameWindowBase::DestroyWindow();
		return m_nDialogResult;
	}
	else
	{
		return IDCANCEL;
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

void CGameDialogBase::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==VK_ESCAPE)
	{
		EndDialog(IDCANCEL);
		(*pbProcessed)=true;
	}
	else if(nKey==VK_RETURN)
	{
		EndDialog(IDOK);
		(*pbProcessed)=true;
	}
}
