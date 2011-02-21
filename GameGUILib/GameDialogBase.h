#pragma once
#include "GameDialogBase.h"

class CGameDialogBase :	virtual public CGameWindowBase, virtual public IGameDialog
{
	bool m_bExecuting;
	int	 m_nDialogResult;

protected:

	virtual void EndDialog(int nResult);

	virtual void OnInitDialog();
	virtual void OnEndDialog();

public:

	// IGameDialog

	int	Execute(IGameWindow *piParent);

	void OnKeyDown(int nKey,bool *pbProcessed);

	CGameDialogBase(void);
	~CGameDialogBase(void);
};
