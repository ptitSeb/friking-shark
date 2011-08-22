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

class CGameGUIInputDialog: public CGameDialogBase, virtual public IGameGUIInputDialog, virtual public IGameGUIButtonEvents
{
	IGameGUIButton *m_piBTAccept;
	IGameGUIButton *m_piBTCancel;
	IGameGUILabel  *m_piSTTitle;
	IGameGUIEdit   *m_piEDText;
	
	std::string m_sText;
	std::string m_sTitle;
protected:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("Title",m_piSTTitle);
		CHILD_MAP_ENTRY("Text",m_piEDText);
		CHILD_MAP_ENTRY_EX("Accept",m_piBTAccept,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Cancel",m_piBTCancel,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	void OnButtonClicked(IGameGUIButton *piControl);
	void OnKeyDown(int nKey,bool *pbProcessed);
	void OnInitDialog();
	void OnEndDialog();

public:
	// IGameGUIButton

	bool ShowInput(IGameWindow *piParent,std::string *psText,std::string sTitle);

	CGameGUIInputDialog(void);
	~CGameGUIInputDialog(void);
};
