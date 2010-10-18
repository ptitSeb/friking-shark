#pragma once

#define MAX_BSP_JOINT_NODES 200

DECLARE_CUSTOM_WRAPPER2(CModelWrapper,IGenericModel,m_piModel,IOpenGLModel,m_piGLModel)

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

DECLARE_SERIALIZABLE(CWorldSector);

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