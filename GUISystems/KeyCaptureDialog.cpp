//  Friking Shark, a Flying Shark arcade remake.
//  Copyright (C) 2011  Javier Martin Garcia (javiermartingarcia@gmail.com)
//	
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


#include "./stdafx.h"
#include "GameRunTimeLib.h"
#include "GameGUILib.h"
#include "GUISystems.h"
#include "KeyCaptureDialog.h"

CKeyCaptureDialog::CKeyCaptureDialog(void)
{
	m_nKey=0;
}

CKeyCaptureDialog::~CKeyCaptureDialog(void)
{
}

void CKeyCaptureDialog::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_ESCAPE)
	{
		CGameDialogBase::OnKeyDown(nKey,pbProcessed);
		return;	
	}
	
	m_nKey=nKey;
	EndDialog(DIALOG_OK);
}

bool CKeyCaptureDialog::CaptureKey(IGameWindow *piParent,unsigned int *pKey)
{
	m_nKey=0;
	Execute(piParent);
	*pKey=m_nKey;
	return m_nKey!=0;
}
