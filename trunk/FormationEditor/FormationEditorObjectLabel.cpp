#include "./stdafx.h"
#include "FormationEditorObjectLabel.h"

CFormationEditorObjectLabel::CFormationEditorObjectLabel(void)
{
	m_piDesignObject=NULL;
}

CFormationEditorObjectLabel::~CFormationEditorObjectLabel(void)
{
}

void CFormationEditorObjectLabel::OnDrawBackground( IGenericRender *piRender )
{
	piRender->Clear(m_vBackgroundColor,m_dBackgroundAlpha);
	CVector vMaxs,vMins;
	if(m_piDesignObject){m_piDesignObject->DesignGetBBox(&vMins,&vMaxs);}
	CVector vSize=(vMaxs-vMins)*1.2;
	CVector vCenter=(vMaxs+vMins)*0.5;
	vCenter.c[1]=vMaxs.c[1]+10;
	double dMaxSize=max(vSize.c[0],vSize.c[2]);
	double dAspectRatio=m_rRealRect.h?m_rRealRect.w/m_rRealRect.h:0;
	piRender->SetOrthographicProjection(dMaxSize*dAspectRatio,dMaxSize);
	piRender->SetCamera(vCenter+CVector(0,vSize.c[1]+10,0),0,-90,0);
	piRender->PushState();
	piRender->ActivateDepth();
	if(m_piDesignObject){m_piDesignObject->DesignRender(piRender,Origin,m_vVisualizationAngles,false);}

	if(m_FrameManager.m_piFrameManager==NULL){m_FrameManager.Attach("GameSystem","FrameManager");}
	if(m_FrameManager.m_piFrameManager){m_vVisualizationAngles.c[YAW]+=30.0*m_FrameManager.m_piFrameManager->GetRealTimeFraction();}
	piRender->PopState();
}	

void CFormationEditorObjectLabel::SetObject(IDesignObject *piObject)
{
	REL(m_piDesignObject);
	m_piDesignObject=ADD(piObject);
}

void CFormationEditorObjectLabel::DestroyWindow()
{
	REL(m_piDesignObject);
	CGameWindowBase::DestroyWindow();
}

bool CFormationEditorObjectLabel::InitWindow( IGameWindow *piParent,bool bPopup )
{
	bool bOk=CGameWindowBase::InitWindow(piParent,bPopup);
	return bOk;
}