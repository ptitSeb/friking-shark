#pragma once

class CGameGUIFileDialog: public CGameDialogBase, virtual public IGameGUIFileDialog, virtual public IGameGUIButtonEvents
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
protected:

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("Title",m_piSTTitle);
		CHILD_MAP_ENTRY("Path",m_piEDPath);
		CHILD_MAP_ENTRY("Files",m_piLSFiles);
		CHILD_MAP_ENTRY_EX("Cancel",m_piBTCancel,IGameGUIButtonEvents);
		CHILD_MAP_ENTRY_EX("Select",m_piBTSelect,IGameGUIButtonEvents);
	END_CHILD_MAP()
	
	void OnButtonClicked(IGameGUIButton *piControl);
	void OnKeyDown(int nKey,bool *pbProcessed);
	
	void OnInitDialog();
	void OnEndDialog();

	void UpdateFiles();

public:
	// IGameGUIFileDialog

	bool OpenFile(IGameWindow *piParent,std::string sTitle,const char *psFilter,std::string *psFile);
	bool SaveFile(IGameWindow *piParent,std::string sTitle,const char *psFilter,std::string *psFile,bool bOverWriteWarn);

	CGameGUIFileDialog(void);
	~CGameGUIFileDialog(void);
};
