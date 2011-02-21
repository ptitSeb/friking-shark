#include "./stdafx.h"
#include "FormationType.h"
#include "GameGraphics.h"

CFormationType::CFormationType()
{
}

CFormationType::~CFormationType()
{
    unsigned x;
    for(x=0;x<m_dElements.size();x++)
    {
        SFormationElement *pElement=&m_dElements[x];
        IEntityType *piEntityType=pElement->m_piEntityType;
        REL(piEntityType);
    }
    m_dElements.clear();
}

bool CFormationType::Unserialize(ISystemPersistencyNode *piNode)
{
	bool bOk=CSystemObjectBase::Unserialize(piNode);
	for(unsigned x=0;x<m_dElements.size();x++)
	{
		SFormationElement *pElement=&m_dElements[x];
		if(pElement->m_piEntityType==NULL)
		{
			CSystemObjectWrapper wrapper;
			wrapper.Attach("EntityTypes",pElement->m_sEntityType);
			if(wrapper.m_piObject){pElement->m_piEntityType=QI(IEntityType,wrapper.m_piObject);}
		}
	}
	return bOk;
}

IFormation *CFormationType::CreateInstance(CVector vPosition,DWORD dwCurrentTime)
{

    CFormation *pFormation=new CFormation(this,vPosition);
    return pFormation;
}

void CFormationType::DesignRender( IGenericRender *piRender,CVector &vPosition,CVector &vAngles ,bool bSelected)
{
	if(bSelected)
	{
		CVector vMins,vMaxs;
		g_PlayAreaManagerWrapper.AddRef();
		g_PlayAreaManagerWrapper.m_piInterface->GetPlayAreaPlaneAt(vPosition,&vMins,&vMaxs);
		g_PlayAreaManagerWrapper.Release();
		CVector vSize=vMaxs-vMins;
		// Centramos el bbox que representa el play area justo delante del punto de activacion
		// y lo hacemos plano
		vMins.c[0]-=vSize.c[0]*0.5;
		vMaxs.c[0]-=vSize.c[0]*0.5;
		piRender->PushState();
		piRender->DeactivateShadowReception();
		piRender->DeactivateHeightFog();
		piRender->RenderBBox(vMins,vMaxs,CVector(0.7,0.7,0.7),0xFFFF);
		piRender->PopState();
	}

	for(unsigned int x=0;x<m_dElements.size();x++)
	{
		SFormationElement *pElement=&m_dElements[x];
		if(pElement->m_piEntityType)
		{		
			piRender->PushState();
			piRender->DeactivateShadowReception();
			piRender->DeactivateHeightFog();
			for(unsigned int y=0;y<m_dElements[x].m_Route.GetPointCount()-1;y++)
			{
				CVector vPos1=m_dElements[x].m_Route.GetPlayAreaElementPoint(vPosition,y);
				CVector vPos2=m_dElements[x].m_Route.GetPlayAreaElementPoint(vPosition,y+1);
				if(bSelected)
				{
					piRender->RenderLine(vPos1,vPos2,CVector(1,1,1),0x8888);
				}
				else
				{
					piRender->RenderLine(vPos1,vPos2,CVector(0.5,0.5,0.5),0x8888);
				}
			}
			piRender->PopState();
			
			CVector vPos=m_dElements[x].m_Route.GetPlayAreaElementPoint(vPosition,0);
			CVector vAngles=AnglesFromVector(m_dElements[x].m_Route.GetDirection(0));
			pElement->m_piEntityType->DesignRender(piRender,vPos,vAngles,bSelected);
		}
	}
}

void CFormationType::DesignGetBBox( CVector *pvMins,CVector *pvMaxs )
{
	if(m_dElements.size())
	{
		// Se deducir las coordenadas a partir de los puntos de ruta con el minimo en -1,-1,1,1
		// De esta forma se incluyen todos los puntos de ruta que esten fuera del area de juego
		// pero como minimo siempre se muestra el area de juego

		SRoutePoint routeMins,routeMaxs;
		routeMins.bAbsolutePoint=false;
		routeMaxs.bAbsolutePoint=false;
		routeMins.vPosition=CVector(-1,-1,0);
		routeMaxs.vPosition=CVector(1,1,0);
		for(unsigned int x=0;x<m_dElements.size();x++)
		{
			for(unsigned int y=0;y<m_dElements[x].m_Route.GetPointCount();y++)
			{
				CVector vTempMins=routeMins.vPosition;
				CVector vTempMaxs=routeMaxs.vPosition;
				routeMins.vPosition.Mins(vTempMins,m_dElements[x].m_Route.GetRelativePoint(y));
				routeMaxs.vPosition.Maxs(vTempMaxs,m_dElements[x].m_Route.GetRelativePoint(y));
			}

		}
		CVector vMins=m_dElements[0].m_Route.GetPlayAreaElementRoutePoint(Origin,&routeMins);
		CVector vMaxs=m_dElements[0].m_Route.GetPlayAreaElementRoutePoint(Origin,&routeMaxs);
		pvMins->Mins(vMins,vMaxs);
		pvMaxs->Maxs(vMins,vMaxs);
	}
}

double CFormationType::DesignGetRadius()
{
	CVector vMins,vMaxs;
	DesignGetBBox(&vMins,&vMaxs);
	return GetBBoxRadius(vMins,vMaxs);
}

CTraceInfo CFormationType::DesignGetTrace( const CVector &vPosition,const CVector &vAngles,const CVector &p1,const CVector &p2 )
{
	CTraceInfo info;
	info.m_dTraceFraction=1.0;
	info.m_vTracePos=p2;
	return info;
}

CFormation::CFormation(CFormationType *pType,CVector vPosition)
{
    g_PlayAreaManagerWrapper.AddRef();
    m_bAllUnitsCreated=true;
    m_pType=pType;
    m_vPosition=vPosition;
    
    m_vElementRunTimeInfo.resize(m_pType->m_dElements.size());
    for(unsigned x=0;x<m_pType->m_dElements.size();x++)
    {
        SFormationElementRunTimeInfo runtimeElement;
        runtimeElement.m_dwCreatedEntities=0;
        runtimeElement.m_dwLastEntityTime=0;
        runtimeElement.m_pFormationTypeElement=&pType->m_dElements[x];        
        m_vElementRunTimeInfo[x]=runtimeElement;
        if(runtimeElement.m_pFormationTypeElement->m_dwEntityCount){m_bAllUnitsCreated=false;}
    }
}

CFormation::~CFormation()
{
    set<IEntity *>::iterator i;
    for(i=m_sEntities.begin();i!=m_sEntities.end();i++)
    {
        IEntity *piEntity=*i;
        UNSUBSCRIBE_FROM_CAST(piEntity,IEntityEvents);
    }
    m_sEntities.clear();
    g_PlayAreaManagerWrapper.Release();
}

void CFormation::AddEntity(IEntity *piEntity)
{
    m_sEntities.insert(piEntity);
    SUBSCRIBE_TO_CAST(piEntity,IEntityEvents);
}
void CFormation::OnRemoved(IEntity *piEntity)
{
    UNSUBSCRIBE_FROM_CAST(piEntity,IEntityEvents);
    m_sEntities.erase(piEntity);
    // La referencia de mas esta para evitar que se borre este objeto al liberarlo el CPlayAreaFormation
    if(m_sEntities.size()==0 && m_bAllUnitsCreated){AddReference();NOTIFY_EVENT(IFormationEvents,OnFormationRemoved(this));ReleaseReference();}
}
void CFormation::OnKilled(IEntity *piEntity)
{
    UNSUBSCRIBE_FROM_CAST(piEntity,IEntityEvents);
    m_sEntities.erase(piEntity);
    // La referencia de mas esta para evitar que se borre este objeto al liberarlo el CPlayAreaFormation
    if(m_sEntities.size()==0 && m_bAllUnitsCreated){AddReference();NOTIFY_EVENT(IFormationEvents,OnFormationKilled(this,piEntity));ReleaseReference();}
}

bool CFormation::ProcessFrame(DWORD dwCurrentTime,double dInterval)
{
    bool bFinished=false;
    m_bAllUnitsCreated=true;
    for(unsigned x=0;x<m_vElementRunTimeInfo.size();x++)
    {
        SFormationElementRunTimeInfo *pElement=&m_vElementRunTimeInfo[x];
        if(pElement->m_pFormationTypeElement->m_piEntityType==NULL){continue;}
        if(pElement->m_dwCreatedEntities==pElement->m_pFormationTypeElement->m_dwEntityCount){continue;}

        if(pElement->m_dwCreatedEntities==0 || dwCurrentTime>(pElement->m_dwLastEntityTime+pElement->m_pFormationTypeElement->m_dwTimeBetweenEntities))
        {
            IEntity *piEntity=pElement->m_pFormationTypeElement->m_piEntityType->CreateInstance(NULL,dwCurrentTime);
            piEntity->GetPhysicInfo()->vPosition=pElement->m_pFormationTypeElement->m_Route.GetAbsolutePoint(0);
            AnglesFromVector(pElement->m_pFormationTypeElement->m_Route.GetDirection(0),&piEntity->GetPhysicInfo()->vAngles);
            piEntity->SetRoute(&pElement->m_pFormationTypeElement->m_Route);
            AddEntity(piEntity);
            pElement->m_dwLastEntityTime=dwCurrentTime;
            pElement->m_dwCreatedEntities++;
        }
        if(pElement->m_dwCreatedEntities<pElement->m_pFormationTypeElement->m_dwEntityCount)
        {
            m_bAllUnitsCreated=false;
        }
    }
    bFinished=(m_sEntities.size()==0 && m_bAllUnitsCreated);
    // La referencia de mas esta para evitar que se borre este objeto al liberarlo el CPlayAreaFormation
    if(bFinished){AddReference();NOTIFY_EVENT(IFormationEvents,OnFormationRemoved(this));ReleaseReference();}
    return !bFinished;
}
