#include "StdAfx.h"
#include "GameRuntimeLib.h"
#include "GameGUILib.h"
#include ".\confirmationdialog.h"

CConfirmationDialog::CConfirmationDialog(void)
{
	m_piBTYes=NULL;
	m_piBTNo=NULL;
}

CConfirmationDialog::~CConfirmationDialog(void)
{
}

void CConfirmationDialog::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTYes)
	{
		EndDialog(eConfirmationDialogAction_Yes);
	}
	if(piControl==m_piBTNo)
	{
		EndDialog(eConfirmationDialogAction_No);
	}
}
