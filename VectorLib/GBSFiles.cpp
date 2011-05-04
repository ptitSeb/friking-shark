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


// GBSFiles.cpp: implementation of the CGBSFiles class.
//
//////////////////////////////////////////////////////////////////////

#include "./StdAfx.h"
#include "GBSFiles.h"

SGBSHeader::SGBSHeader()
{
	dwVersion=GBS_FILE_VERSION;
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

bool CGBSFileType::Load(const char *pFileName,CBSPNode **ppBSPNode,std::vector<CPolygon *> *pGeometricData)
{
	FILE *pFile=fopen(pFileName,"rb");
	if(pFile==NULL){return false;}

	bool bHeaderOk=false;
	if(fread(&m_Header,sizeof(m_Header),1,pFile)==1)
	{
		if(memcmp(&m_Header.sMagic,GBS_FILE_MAGIC,GBS_FILE_MAGIC_LENGTH)==0 && m_Header.dwVersion<=GBS_FILE_VERSION)
		{
			bHeaderOk=true;
		}
		else
		{
			m_Header.dwVersion=GBS_FILE_VERSION;
			m_Header.dwDataOffset=0;
			m_Header.dwFlags=0;
			memcpy(m_Header.sMagic,GBS_FILE_MAGIC,GBS_FILE_MAGIC_LENGTH);
		}
		if(pGeometricData && m_Header.dwFlags&GBS_FILE_FLAG_CONTAINS_GEOMETRIC_DATA)
		{
			unsigned int x=0,dwPolygonCount=0;
			if(fread(&dwPolygonCount,sizeof(dwPolygonCount),1,pFile)==1)
			{
			  for(x=0;x<dwPolygonCount;x++)
			  {
				  unsigned int v=0,dwVertexCount=0;
				  if(fread(&dwVertexCount,sizeof(dwVertexCount),1,pFile)!=1){break;}
				  
				  CPolygon *pPolygon=new CPolygon;
				  pPolygon->m_nVertexes=dwVertexCount;
				  if(pPolygon->m_nVertexes)
				  {
					  pPolygon->m_pVertexes=new CVector[pPolygon->m_nVertexes];
					  for(v=0;v<dwVertexCount;v++)
					  {
						  if(fread(pPolygon->m_pVertexes[v].c,sizeof(pPolygon->m_pVertexes[v].c),1,pFile)!=1)
						  {
							break;
						  }
					  }
					  pPolygon->CalcPlane();
				  }
				  pGeometricData->push_back(pPolygon);
				}
			}
		}
	}
	fseek(pFile,m_Header.dwDataOffset,SEEK_SET);

	if(ppBSPNode){(*ppBSPNode)=ReadNode(pFile,NULL);}
	fclose(pFile);
	pFile=NULL;
	return true;
}

CBSPNode *CGBSFileType::ReadNode(FILE *pFile,CBSPNode *pParent)
{
	CBSPNode *pNode=NULL;
	SGBSFileNodeInfo info;
	if(fread(&info,sizeof(info),1,pFile)!=1){return pNode;}
	CPlane plane(CVector(info.vNormal[0],info.vNormal[1],info.vNormal[2]),info.vDist);
	pNode=new CBSPNode(pParent,plane,info.nContent);
	if(pNode->content==CONTENT_NODE)
	{
		pNode->pChild[0]=ReadNode(pFile,pNode);
		pNode->pChild[1]=ReadNode(pFile,pNode);
	}
	return pNode;
}

bool CGBSFileType::Save(const char *pFileName,CBSPNode *pBSPNode,std::vector<CPolygon *> *pGeometricData,SGBSFileNodeStats *pStats)
{
	FILE *pFile=fopen(pFileName,"wb");
	if(pFile==NULL){return false;}
	if(fwrite(&m_Header,sizeof(m_Header),1,pFile)!=1){fclose(pFile);pFile=NULL;return false;}
	if(pGeometricData)
	{
		m_Header.dwFlags|=GBS_FILE_FLAG_CONTAINS_GEOMETRIC_DATA;
		unsigned int x=0,dwPolygonCount=pGeometricData->size();
		if(fwrite(&dwPolygonCount,sizeof(dwPolygonCount),1,pFile)!=1){fclose(pFile);pFile=NULL;return false;}
		
		for(x=0;x<dwPolygonCount;x++)
		{
			CPolygon *pPolygon=(*pGeometricData)[x];
			unsigned int v=0,dwVertexCount=pPolygon->m_nVertexes;
			if(fwrite(&dwVertexCount,sizeof(dwVertexCount),1,pFile)!=1){fclose(pFile);pFile=NULL;return false;}
			for(v=0;v<dwVertexCount;v++)
			{
				if(fwrite(pPolygon->m_pVertexes[v].c,sizeof(pPolygon->m_pVertexes[v].c),1,pFile)!=1){fclose(pFile);pFile=NULL;return false;}
			}
		}
	}	

	SGBSFileNodeStats stats;
	if(pStats==NULL){pStats=&stats;}
	m_Header.dwDataOffset=ftell(pFile);

	WriteNode(pFile,pBSPNode,pStats,0,&pStats->dBalanceFactor);

	//Update header dwDataOffset
	fseek(pFile,0,SEEK_SET);
	if(fwrite(&m_Header,sizeof(m_Header),1,pFile)!=1){fclose(pFile);pFile=NULL;return false;}
	fclose(pFile);pFile=NULL;	
	return true;
}

bool CGBSFileType::WriteNode(FILE *pFile,CBSPNode *pNode,SGBSFileNodeStats *pStats,int nCurrentDepth,double *pBalanceFactor)
{
	pStats->nNodes++;
	if(nCurrentDepth>pStats->nDepth){pStats->nDepth=nCurrentDepth;}
	if(pNode->content!=CONTENT_NODE){pStats->nLeafs++;*pBalanceFactor=0.0;}
	if(pNode->content==CONTENT_EMPTY){pStats->nEmptyLeafs++;}
	if(pNode->content==CONTENT_SOLID){pStats->nSolidLeafs++;}

	SGBSFileNodeInfo info;
	info.vNormal[0]=(float)pNode->plane.c[0];
	info.vNormal[1]=(float)pNode->plane.c[1];
	info.vNormal[2]=(float)pNode->plane.c[2];
	info.vDist=(float)pNode->plane.d;
	info.nContent=pNode->content;
	if(fwrite(&info,sizeof(info),1,pFile)!=1){return false;}
	if(pNode->content==CONTENT_NODE)
	{
		double dChildFactor[2]={0};
		WriteNode(pFile,pNode->pChild[0],pStats,nCurrentDepth+1,&dChildFactor[0]);
		WriteNode(pFile,pNode->pChild[1],pStats,nCurrentDepth+1,&dChildFactor[1]);
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

unsigned int		CGBSFileType::GetVersion()
{
	return m_Header.dwVersion;
}

bool CGBSFileType::CompareGeometricData(std::vector<CPolygon *> *pGeometricData1,std::vector<CPolygon *> *pGeometricData2)
{
	if(pGeometricData1->size()!=pGeometricData2->size())
	{return false;}

	for(unsigned int x=0;x<pGeometricData1->size();x++)
	{
		CPolygon *pPoly1=(*pGeometricData1)[x],*pPoly2=(*pGeometricData2)[x];
		if(pPoly1->m_nVertexes!=pPoly2->m_nVertexes)
		{return false;}
		for(unsigned int v=0;v<pPoly1->m_nVertexes;v++)
		{
			if(pPoly1->m_pVertexes[v]!=pPoly2->m_pVertexes[v])
			{
				return false;
			}
		}
	}
	return true;
}
