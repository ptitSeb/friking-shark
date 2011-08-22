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
#include "GameGUI.h"
#include "GUISystems.h"
#include "HighScoresTable.h"

unsigned int	CHighScoresTable::GetRowCount(){return m_vTable.size();}
SHighScoreRow 	CHighScoresTable::GetRow(unsigned int nIndex)
{
	if(nIndex>=m_vTable.size()){return SHighScoreRow();}
	return m_vTable[nIndex];
}

void CHighScoresTable::SetRow(unsigned int nIndex,SHighScoreRow &row)
{
	if(nIndex>=m_vTable.size()){return ;}
	m_vTable[nIndex]=row;
}

int CHighScoresTable::AddRow(SHighScoreRow &row)
{
	while(m_vTable.size()>m_nMaxScores){m_vTable.pop_back();}
	
	if(row.nScore==0){return -1;}

	std::vector<SHighScoreRow>::iterator i=m_vTable.begin();
	unsigned int x=0;
	for(x=0;x<m_vTable.size();x++,i++)
	{
		if(m_vTable[x].nScore<row.nScore)
		{
			break;
		}
	}
	if(x>=(m_nMaxScores)){return -1;}
	m_vTable.insert(i,row);
	return x;
}

CHighScoresTable::CHighScoresTable()
{
	m_nMaxScores=100;
	for(int x=0;x<20;x++)
	{
		SHighScoreRow sRow(".............",eGameDifficulty_Easy,(x+1)*5000);
		AddRow(sRow);
	}	
}

CHighScoresTable::~CHighScoresTable(){}

