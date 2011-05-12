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
#include "WorldSector.h"
#include "WorldManager.h"


CWorldSector::CWorldSector()
{
    m_pBSPTree=NULL;
    m_pWorldManager=NULL;
}

CWorldSector::~CWorldSector()
{
	if(m_pBSPTree){delete m_pBSPTree;m_pBSPTree=NULL;}
}
/*
void CWorldSector::Initialize(unsigned nSectorIndex,unsigned nSectorCount)
{
	m_Model.Attach("GameResources",m_sModel);

	if(m_Model.m_piModel!=NULL)
	{
		if(m_vBBoxMins==Origin && m_vBBoxMaxs==Origin)
		{
			m_Model.m_piModel->GetFrameBBox(0,0,&m_vBBoxMins,&m_vBBoxMaxs);
		}

		char sGBSFile[MAX_PATH]={0},sDrive[MAX_PATH]={0},sPath[MAX_PATH]={0},sFileName[MAX_PATH]={0},sExt[MAX_PATH]={0},sOldDirectory[MAX_PATH]={0};

		string sModelPath=m_Model.m_piModel->GetFileName();
		_splitpath(sModelPath.c_str(),sDrive,sPath,sFileName,sExt);
		if(strcmp(sDrive,"")==0)
		{
			sprintf(sGBSFile,"Models\\%s.gbs",sFileName);
		}
		else
		{
			sprintf(sGBSFile,"%s%s%s.gbs",sDrive,sPath,sFileName);
		}

		GetCurrentDirectory(MAX_PATH,sOldDirectory);

		CGBSFileType gbsFile;
		if(m_pWorldManager->m_bCalcSectorsBSPs)
		{
			int x=0;
			bool bRegenerate=false;
			bool bBSPFound=false;
			vector<CPolygon *> vModelGeometry;
			vector<CPolygon *> vBSPGeometry;

			//m_pWorldManager->m_GUIManagerWrapper.m_piGUIManager->SetLoadString("Loading %2d/%2d '%s' ...",nSectorIndex+1,nSectorCount,sGBSFile);
	
			m_Model.m_piModel->GetFrameGeometry(0,0,&vModelGeometry);

			gbsFile.Load(sGBSFile,&m_pBSPTree,m_pWorldManager->m_bCalcSectorsBSPs?&vBSPGeometry:NULL);
			if(m_pBSPTree)
			{
				bBSPFound=true;
				//m_pWorldManager->m_GUIManagerWrapper.m_piGUIManager->SetLoadString("Checking %2d/%2d '%s' ...",nSectorIndex+1,nSectorCount,sGBSFile);
				if(!CGBSFileType::CompareGeometricData(&vModelGeometry,&vBSPGeometry))
				{
					bRegenerate=true;
				}
			}
			else
			{
				bRegenerate=true;
			}
			if(bRegenerate)
			{
				if(bBSPFound)
				{
					//m_pWorldManager->m_GUIManagerWrapper.m_piGUIManager->SetLoadString("Updating BSP %2d/%2d '%s' ...",nSectorIndex+1,m_sModel.c_str(),sGBSFile);
				}
				else
				{
				// m_pWorldManager->m_GUIManagerWrapper.m_piGUIManager->SetLoadString("Generating BSP %2d/%2d '%s' ...",nSectorIndex+1,m_sModel.c_str(),sGBSFile);
				}
				if(m_pBSPTree){delete m_pBSPTree;m_pBSPTree=NULL;}
				m_pBSPTree=BSPFromPolygonVector(NULL,1,&vModelGeometry,CONTENT_NODE,NULL);
				if(m_pBSPTree)
				{
					CGBSFileType gbsFile;
					gbsFile.Save(sGBSFile,m_pBSPTree,&vModelGeometry);
				}
			}
			for(x=0;x<(int)vBSPGeometry.size();x++)
			{
				CPolygon *pPolygon=vBSPGeometry[x];
				delete pPolygon;
			}
			for(x=0;x<(int)vModelGeometry.size();x++)
			{
				CPolygon *pPolygon=vModelGeometry[x];
				delete pPolygon;
			}
			vBSPGeometry.clear();
			vModelGeometry.clear();
	        
		}
	}
}
*/
CWorldSectorSplitOffsetedBSPNode::CWorldSectorSplitOffsetedBSPNode(CBSPNode *parent,CBSPNode *pChild,double dChildOffset)
:CBSPNode(pParent)
{
    m_vChildOffset.c[0]=dChildOffset;
    m_pChild=pChild;
}

CWorldSectorSplitOffsetedBSPNode::~CWorldSectorSplitOffsetedBSPNode()
{
}

int	CWorldSectorSplitOffsetedBSPNode::GetContent(CVector position)
{
    return m_pChild->CBSPNode::GetContent(position-m_vChildOffset);
}

CTraceInfo	CWorldSectorSplitOffsetedBSPNode::GetTrace(CVector p1,CVector p2,CVector realp1,CVector realp2)
{
    CTraceInfo traceInfo=m_pChild->CBSPNode::GetTrace(p1-m_vChildOffset,p2-m_vChildOffset,realp1-m_vChildOffset,realp2-m_vChildOffset);
    traceInfo.m_vTracePos+=m_vChildOffset;
    return traceInfo;
}