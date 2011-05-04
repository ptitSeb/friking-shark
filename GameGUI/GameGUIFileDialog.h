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

class CGameGUIFileDialog: public CGameDialogBase, virtual public IGameGUIFileDialog, 
	virtual public IGameGUIButtonEvents,
	virtual public IGameGUIEditEvents,
	virtual public IGameGUIListEvents
{
	IGameGUIButton *m_piBTCancel;
	IGameGUIButton *m_piBTSelect;
	IGameGUILabel  *m_piSTTitle;
	IGameGUIEdit   *m_piEDPath;
	IGameGUIList   *m_piLSFiles;

	bool m_bOpenMode;
	bool m_bOverWriteWarn;

	std::string m_sFile;
	std::string m_sTitle;
	std::vector<std::string> m_vPatterns;
protected:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("Title",m_piSTTitle);
		CHILD_MAP_ENTRY_EX("Path",m_piEDPath,IGameGUIEditEvents);
		CHILD_MAP_ENTRY_EX("Files",m_piLSFiles,IGameGUIListEvents);
		CHILD_MAP_ENTRY_EX("Cancel",m_piBTCancel,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Select",m_piBTSelect,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	void OnButtonClicked(IGameGUIButton *piControl);
	void OnTextChanged(IGameGUIEdit *piControl,std::string sNewText);
	void OnSelectionChanged(IGameGUIList *piControl,int nElement,std::string sElement);
	void OnSelectionDoubleCliked(IGameGUIList *piControl,int nElement,std::string sElement);

	void OnKeyDown(int nKey,bool *pbProcessed);
	
	void OnInitDialog();
	void OnEndDialog();

	void UpdateFiles();

	void ProcessSelect();

	void AutoComplete();

public:
	// IGameGUIFileDialog

	bool OpenFile(IGameWindow *piParent,std::string sTitle,const char *psFilter,std::string *psFile);
	bool SaveFile(IGameWindow *piParent,std::string sTitle,const char *psFilter,std::string *psFile,bool bOverWriteWarn);

	CGameGUIFileDialog(void);
	~CGameGUIFileDialog(void);
};
