// GBSFiles.cpp: implementation of the CGBSFiles class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GBSFiles.h"

SGBSHeader::SGBSHeader()
{
	dwVersion=GBS_FILE_VERSION;
	tTimeStamp.dwHighDateTime=0;
	tTimeStamp.dwLowDateTime=0;
	dwDataOffset=0;
	dwFlags=0;
	memcpy(sMagic,GBS_FILE_MAGIC,GBS_FILE_MAGIC_LENGTH);
}

struct SGBSFileNodeInfo
{
	float  vNormal[3];
	float  vDist;
	int    nContent;

	SGBSFileNodeInfo()
	{
		vNormal[0]=vNormal[1]=vNormal[2]=0;
		vDist=0;
		nContent=CONTENT_EMPTY;
	}
};

CGBSFileType::CGBSFileType(){}
CGBSFileType::~CGBSFileType(){}

bool CGBSFileType::Load(const char *pFile,CBSPNode **ppBSPNode,std::vector<CPolygon *> *pGeometricData)
{
	HANDLE hFile=CreateFile(pFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE){return false;}

	bool bHeaderOk=FALSE;
	DWORD dwRead=0;
	if(ReadFile(hFile,&m_Header,sizeof(m_Header),&dwRead,NULL))
	{
		if(memcmp(&m_Header.sMagic,GBS_FILE_MAGIC,GBS_FILE_MAGIC_LENGTH)==0 && m_Header.dwVersion<=GBS_FILE_VERSION)
		{
			bHeaderOk=true;
		}
		else
		{
			m_Header.dwVersion=GBS_FILE_VERSION;
			m_Header.tTimeStamp.dwHighDateTime=0;
			m_Header.tTimeStamp.dwLowDateTime=0;
			m_Header.dwDataOffset=0;
			m_Header.dwFlags=0;
			memcpy(m_Header.sMagic,GBS_FILE_MAGIC,GBS_FILE_MAGIC_LENGTH);
		}
		if(pGeometricData && m_Header.dwFlags&GBS_FILE_FLAG_CONTAINS_GEOMETRIC_DATA)
		{
			DWORD x=0,dwPolygonCount=0;
			ReadFile(hFile,&dwPolygonCount,sizeof(dwPolygonCount),&dwRead,NULL);
			for(x=0;x<dwPolygonCount;x++)
			{
				DWORD v=0,dwVertexCount=0;
				ReadFile(hFile,&dwVertexCount,sizeof(dwVertexCount),&dwRead,NULL);
				CPolygon *pPolygon=new CPolygon;
				pPolygon->m_nVertexes=dwVertexCount;
				if(pPolygon->m_nVertexes)
				{
					pPolygon->m_pVertexes=new CVector[pPolygon->m_nVertexes];
					for(v=0;v<dwVertexCount;v++)
					{
						ReadFile(hFile,pPolygon->m_pVertexes[v].c,sizeof(pPolygon->m_pVertexes[v].c),&dwRead,NULL);
					}
					pPolygon->CalcPlane();
				}
				pGeometricData->push_back(pPolygon);
			}
		}
	}
	SetFilePointer(hFile,m_Header.dwDataOffset,NULL,FILE_BEGIN);

	if(ppBSPNode){(*ppBSPNode)=ReadNode(hFile,NULL);}
	CloseHandle(hFile);
	return true;
}

CBSPNode *CGBSFileType::ReadNode(HANDLE hFile,CBSPNode *pParent)
{
	DWORD read=0;

	CBSPNode *pNode=NULL;
	SGBSFileNodeInfo info;
	if(!ReadFile(hFile,&info,sizeof(info),&read,NULL)){return pNode;}
	CPlane plane(CVector(info.vNormal[0],info.vNormal[1],info.vNormal[2]),info.vDist);
	pNode=new CBSPNode(pParent,plane,info.nContent);
	if(pNode->content==CONTENT_NODE)
	{
		pNode->pChild[0]=ReadNode(hFile,pNode);
		pNode->pChild[1]=ReadNode(hFile,pNode);
	}
	return pNode;
}

bool CGBSFileType::Save(const char *pFile,CBSPNode *pBSPNode,std::vector<CPolygon *> *pGeometricData,SGBSFileNodeStats *pStats)
{
	DWORD written;
	int size=sizeof(SGBSFileNodeInfo);
	int sizeb=sizeof(float);
	CBSPNode *pNode=NULL;
	HANDLE hFile=CreateFile(pFile,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE){return false;}
	if(!WriteFile(hFile,&m_Header,sizeof(m_Header),&written,NULL)){return false;}
	if(pGeometricData)
	{
		m_Header.dwFlags|=GBS_FILE_FLAG_CONTAINS_GEOMETRIC_DATA;
		DWORD x=0,dwPolygonCount=pGeometricData->size();
		WriteFile(hFile,&dwPolygonCount,sizeof(dwPolygonCount),&written,NULL);
		for(x=0;x<dwPolygonCount;x++)
		{
			CPolygon *pPolygon=(*pGeometricData)[x];
			DWORD v=0,dwVertexCount=pPolygon->m_nVertexes;
			WriteFile(hFile,&dwVertexCount,sizeof(dwVertexCount),&written,NULL);
			for(v=0;v<dwVertexCount;v++)
			{
				WriteFile(hFile,pPolygon->m_pVertexes[v].c,sizeof(pPolygon->m_pVertexes[v].c),&written,NULL);
			}
		}
	}	

	SGBSFileNodeStats stats;
	if(pStats==NULL){pStats=&stats;}
	m_Header.dwDataOffset=SetFilePointer(hFile,0,NULL,FILE_CURRENT);

	WriteNode(hFile,pBSPNode,pStats,0,&pStats->dBalanceFactor);

	//Update header dwDataOffset
	SetFilePointer(hFile,0,NULL,FILE_BEGIN);
	WriteFile(hFile,&m_Header,sizeof(m_Header),&written,NULL);
	CloseHandle(hFile);
	return false;
}

bool CGBSFileType::WriteNode(HANDLE hFile,CBSPNode *pNode,SGBSFileNodeStats *pStats,int nCurrentDepth,double *pBalanceFactor)
{
	DWORD written=0;

	pStats->nNodes++;
	if(nCurrentDepth>pStats->nDepth){pStats->nDepth=nCurrentDepth;}
	if(pNode->content!=CONTENT_NODE){pStats->nLeafs++;*pBalanceFactor=0.0;}
	if(pNode->content==CONTENT_EMPTY){pStats->nEmptyLeafs++;}
	if(pNode->content==CONTENT_SOLID){pStats->nSolidLeafs++;}

	SGBSFileNodeInfo info;
	info.vNormal[0]=pNode->plane.c[0];
	info.vNormal[1]=pNode->plane.c[1];
	info.vNormal[2]=pNode->plane.c[2];
	info.vDist=pNode->plane.d;
	info.nContent=pNode->content;
	if(!WriteFile(hFile,&info,sizeof(info),&written,NULL)){return false;}
	if(pNode->content==CONTENT_NODE)
	{
		double dChildFactor[2]={0};
		WriteNode(hFile,pNode->pChild[0],pStats,nCurrentDepth+1,&dChildFactor[0]);
		WriteNode(hFile,pNode->pChild[1],pStats,nCurrentDepth+1,&dChildFactor[1]);
		if(pNode->pChild[0]->content!=CONTENT_NODE && pNode->pChild[1]->content!=CONTENT_NODE)
		{
			(*pBalanceFactor)=1.0;
		}
		else
		{
			(*pBalanceFactor)=(dChildFactor[0]+dChildFactor[1])/2;
		}
	}
	return true;
}

DWORD		CGBSFileType::GetVersion()
{
	return m_Header.dwVersion;
}

FILETIME	CGBSFileType::GetTimeStamp()
{
	return m_Header.tTimeStamp;
}

void		CGBSFileType::SetTimeStamp(FILETIME tTimeStamp)
{
	m_Header.tTimeStamp=tTimeStamp;
}

bool CGBSFileType::CompareGeometricData(std::vector<CPolygon *> *pGeometricData1,std::vector<CPolygon *> *pGeometricData2)
{
	if(pGeometricData1->size()!=pGeometricData2->size())
	{return false;}

	for(int x=0;x<pGeometricData1->size();x++)
	{
		CPolygon *pPoly1=(*pGeometricData1)[x],*pPoly2=(*pGeometricData2)[x];
		if(pPoly1->m_nVertexes!=pPoly2->m_nVertexes)
		{return false;}
		for(int v=0;v<pPoly1->m_nVertexes;v++)
		{
			if(pPoly1->m_pVertexes[v]!=pPoly2->m_pVertexes[v])
			{
				return false;
			}
		}
	}
	return true;
}
