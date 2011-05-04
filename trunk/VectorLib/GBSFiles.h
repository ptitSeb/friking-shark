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


// GBSFiles.h: interface for the CGBSFiles class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GBSFILES_H__82021058_670F_4D77_A597_FC956E4EE9A8__INCLUDED_)
#define AFX_GBSFILES_H__82021058_670F_4D77_A597_FC956E4EE9A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VectorLib.h"

#define GBS_FILE_VERSION		0x101 // 1.01
#define GBS_FILE_MAGIC			"<GBS>"
#define GBS_FILE_MAGIC_LENGTH	5

#define GBS_FILE_FLAG_CONTAINS_GEOMETRIC_DATA	0x0001

struct SGBSHeader
{
	unsigned int		dwVersion;
	unsigned char		sMagic[GBS_FILE_MAGIC_LENGTH];
	unsigned int		dwFlags;
	unsigned int		dwDataOffset;

	SGBSHeader();
};

struct SGBSFileNodeStats
{
	int    nNodes;
	int	   nLeafs;
	int	   nEmptyLeafs;
	int	   nSolidLeafs;
	int    nDepth;
	double dBalanceFactor;

	SGBSFileNodeStats()
	{
		nNodes=0;
		nLeafs=0;
		nEmptyLeafs=0;
		nSolidLeafs=0;
		nDepth=0;
		dBalanceFactor=0;
	}
};

class CGBSFileType
{
	SGBSHeader	 m_Header;

	CBSPNode	*ReadNode(FILE *pFile,CBSPNode *pParent);
	bool		 WriteNode(FILE *pFile,CBSPNode *pNode,SGBSFileNodeStats *pStats,int nCurrentDepth,double *pBalanceFactor);
public:
	unsigned int		GetVersion();
	
	bool		Load(const char *pFile,CBSPNode **ppBSPNode,std::vector<CPolygon *> *pGeometricData);
	bool		Save(const char *pFile,CBSPNode *pBSPNode,std::vector<CPolygon *> *pGeometricData,SGBSFileNodeStats *pStats=NULL);

	static bool CompareGeometricData(std::vector<CPolygon *> *pGeometricData1,std::vector<CPolygon *> *pGeometricData2);

	CGBSFileType();
	virtual ~CGBSFileType();

};

#endif // !defined(AFX_GBSFILES_H__82021058_670F_4D77_A597_FC956E4EE9A8__INCLUDED_)
