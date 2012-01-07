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


#pragma once

class CKeyCaptureDialog: virtual public CGameDialogBase,virtual public IKeyCaptureDialog
{
	bool m_bCapturingJoystickButton;

	unsigned int m_nKey;
	
	IGameGUILabel *m_piSTText;
	IGameGUILabel *m_piSTCancelText;
	
public:

	bool CaptureKey(IGameWindow *piParent,unsigned int *pKey);
	bool CaptureJoystickButton(IGameWindow *piParent,unsigned int *pButton);
	
	void OnInitDialog();
	void OnKeyDown(int nKey,bool *pbProcessed);
	
	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("Text",m_piSTText);
		CHILD_MAP_ENTRY("CancelText",m_piSTCancelText);
	END_CHILD_MAP()
	
	CKeyCaptureDialog(void);
	~CKeyCaptureDialog(void);
};
