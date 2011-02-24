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
	DWORD		dwVersion;
	BYTE		sMagic[GBS_FILE_MAGIC_LENGTH];
	DWORD		dwFlags;
	DWORD		dwDataOffset;

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
	DWORD		GetVersion();
	
	bool		Load(const char *pFile,CBSPNode **ppBSPNode,std::vector<CPolygon *> *pGeometricData);
	bool		Save(const char *pFile,CBSPNode *pBSPNode,std::vector<CPolygon *> *pGeometricData,SGBSFileNodeStats *pStats=NULL);

	static bool CompareGeometricData(std::vector<CPolygon *> *pGeometricData1,std::vector<CPolygon *> *pGeometricData2);

	CGBSFileType();
	virtual ~CGBSFileType();

};

#endif // !defined(AFX_GBSFILES_H__82021058_670F_4D77_A597_FC956E4EE9A8__INCLUDED_)
