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


class CHighScoresDialog: virtual public CGameDialogBase,virtual public IHighScoresDialog, virtual public IGameGUIEditEvents
{
	CSoundTypeWrapper m_HighScoresMusic;
	ISound *m_piHighScoresMusicSound;
	
	IHighScoresTable *m_piTable;
	
	unsigned int m_nEditConfirmBlinkStart;
	unsigned int m_nEditCommittedStart;
	unsigned int m_nStartTime;
	bool m_bAlreadyFinished;
	
	int          m_nScoreToEdit;
	unsigned int m_nRowSize;
	unsigned int m_nRowCount;
		
	IGameGUIEdit   *m_piEDName;
	IGameWindow    *m_piGRColumns[eHighScoresColumn_Count];
	IGameGUILabel  *m_piTitle;
	
	CGenericFontWrapper m_EditionFonts[3];
	
	void OnInitDialog();
	void OnDraw(IGenericRender *piRender);
	void OnKeyDown(int nKey,bool *pbProcessed);
	void OnTextChanged(IGameGUIEdit *piControl,std::string sNewText);
	
	int	Execute(IGameWindow *piParent);
	bool Unserialize(ISystemPersistencyNode *piNode);
	void Destroy();

	BEGIN_CHILD_MAP()
		CHILD_MAP_ENTRY("ColumnRanking",m_piGRColumns[eHighScoresColumn_Ranking]);
		CHILD_MAP_ENTRY("ColumnScore",m_piGRColumns[eHighScoresColumn_Score]);
		CHILD_MAP_ENTRY("ColumnDifficulty",m_piGRColumns[eHighScoresColumn_Difficulty]);
		CHILD_MAP_ENTRY("ColumnName",m_piGRColumns[eHighScoresColumn_Name]);
		CHILD_MAP_ENTRY_EX("NameEditor",m_piEDName,IGameGUIEditEvents);
		CHILD_MAP_ENTRY("Title",m_piTitle);
	END_CHILD_MAP()
	
	
	BEGIN_PROP_MAP(CHighScoresDialog)
	PROP_CLASS_CHAIN(CGameDialogBase);
		PROP_FLAGS(m_HighScoresMusic,"Music",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nRowSize,"RowSize",20,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nRowCount,"RowCount",20,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_EditionFonts[0],"EditionFontA",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_EditionFonts[1],"EditionFontB",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_EditionFonts[2],"EditionFontC",MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP()
		
public:
	
	void ShowScores(IGameWindow *piParent,IHighScoresTable *piTable, int nScoreToEdit/*-1 to only show the scores*/);
	
	CHighScoresDialog(void);
	~CHighScoresDialog(void);
};
