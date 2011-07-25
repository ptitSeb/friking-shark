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

BEGIN_STRUCT_PROPS(SHighScoreRow)
	PROP(sName,"Name")
	PROP(eDifficulty,"Difficulty")
	PROP(nScore,"Score")
END_STRUCT_PROPS()

class CHighScoresTable: virtual public CSystemObjectBase, virtual public IHighScoresTable
{
	unsigned int m_nMaxScores;
	
	std::vector<SHighScoreRow> m_vTable;
	BEGIN_PROP_MAP(CHighScoresTable)
		PROP_VALUE(m_nMaxScores,"MaxScores",100)
		PROP(m_vTable,"Table")
	END_PROP_MAP()
public:
	
	unsigned int	GetRowCount();
	SHighScoreRow	GetRow(unsigned int nIndex);
	int 			AddRow(SHighScoreRow &sRow);
	void			SetRow(unsigned int nIndex,SHighScoreRow &sRow);
	
	CHighScoresTable();
	~CHighScoresTable();
};
