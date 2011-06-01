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

#define MAX_BSP_JOINT_NODES 200

DECLARE_CUSTOM_WRAPPER1(CModelWrapper,IGenericModel,m_piModel)

class CWorldManager;

class CWorldSector
{
public:
    CWorldManager       *m_pWorldManager;
	CModelWrapper		m_Model;
    C3DSVector          m_vOffset;
    C3DSVector			m_vBBoxMins;
    C3DSVector			m_vBBoxMaxs;
    string              m_sModel;
    CBSPNode			*m_pBSPTree;

    void Initialize(unsigned nSectorIndex,unsigned nSectorCount);

    BEGIN_PROP_MAP(CWorldSector)
        PROP(m_sModel,"Modelo");
        PROP(m_vOffset,"Offset");
        PROP(m_vBBoxMins,"AbsMins");
        PROP(m_vBBoxMaxs,"AbsMaxs");
    END_PROP_MAP()

    CWorldSector();
    ~CWorldSector();
};

DECLARE_SERIALIZABLE(CWorldSector)

class CWorldSectorSplitOffsetedBSPNode:public CBSPNode
{
    CVector		m_vChildOffset;
    CBSPNode	*m_pChild;
public:

    CWorldSectorSplitOffsetedBSPNode(CBSPNode *parent,CBSPNode *pChild,double dChildOffset);
    ~CWorldSectorSplitOffsetedBSPNode();

    int	        GetContent(CVector position);
    CTraceInfo	GetTrace(CVector p1,CVector p2,CVector realp1,CVector realp2);
};