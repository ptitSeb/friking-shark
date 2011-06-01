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
#include "TimeOutDialog.h"

CTimeOutDialog::CTimeOutDialog(void)
{
	m_nStartTime=0;
	m_bAlreadyFinished=false;
}

CTimeOutDialog::~CTimeOutDialog(void)
{
}

int	CTimeOutDialog::Execute(IGameWindow *piParent)
{
	m_nStartTime=GetTimeStamp();
	m_bAlreadyFinished=false;
	return CGameDialogBase::Execute(piParent);
}

void CTimeOutDialog::OnDraw(IGenericRender *piRender)
{
	CGameDialogBase::OnDraw(piRender);
	
	if(!m_bAlreadyFinished && (m_nStartTime+3*1000)<GetTimeStamp())
	{
		m_bAlreadyFinished=true;
		EndDialog(0);
	}
}

