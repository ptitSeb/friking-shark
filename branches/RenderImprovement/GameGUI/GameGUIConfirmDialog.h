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

class CGameGUIConfirmDialog: public CGameDialogBase, virtual public IGameGUIConfirmDialog, virtual public IGameGUIButtonEvents
{
	IGameGUIButton *m_piBTYes;
	IGameGUIButton *m_piBTNo;
	IGameGUILabel  *m_piSTTitle;
	IGameGUILabel  *m_piSTText;
	
	std::string m_sText;
	std::string m_sTitle;
	EMessageDialogType m_eType;
	
	CVector m_vWarningBkColor;
	CVector m_vErrorBkColor;
	CVector m_vInfoBkColor;
	CVector m_vQuestionBKColor;
	
protected:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY_FLAGS("Title",m_piSTTitle,CMEF_OPTIONAL);
		CHILD_MAP_ENTRY("Text",m_piSTText);
		CHILD_MAP_ENTRY_EX("Yes",m_piBTYes,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("No",m_piBTNo,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	BEGIN_PROP_MAP(CGameGUIConfirmDialog)
		PROP_CLASS_CHAIN(CGameDialogBase)
		PROP_VALUE_FLAGS(m_vWarningBkColor,"WarningBkColor",CVector(0.4,0.4,0),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vErrorBkColor,"ErrorBkColor",CVector(0.4,0,0),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vInfoBkColor,"InfoBkColor",CVector(0.4,0.4,0.4),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vQuestionBKColor,"QuestionBKColor",CVector(0,0,0.4),MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP()
	
	void OnButtonClicked(IGameGUIButton *piControl);
	void OnKeyDown(int nKey,bool *pbProcessed);
	void OnInitDialog();
	void OnEndDialog();

public:
	// IGameGUIButton

	bool Confirm(IGameWindow *piParent,std::string sText,std::string sTitle,EMessageDialogType eType);


	CGameGUIConfirmDialog(void);
	~CGameGUIConfirmDialog(void);
};
