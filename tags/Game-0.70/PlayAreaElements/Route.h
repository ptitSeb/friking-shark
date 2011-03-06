#pragma once

BEGIN_STRUCT_PROPS(SRoutePoint)
    PROP(vPosition,"Posicion")
    PROP_VALUE_FLAGS(bAbsolutePoint,"Absoluto",true,MRPF_NORMAL|MRPF_OPTIONAL)
END_STRUCT_PROPS()

class CRoute:public IRoute
{
    vector<SRoutePoint>   m_vPoints;
    bool                  m_bCyclic;

	CVector GetAbsoluteRoutePoint(SRoutePoint *p);

public:

    CRoute();

	CVector     GetRelativePoint(unsigned int nIndex);
	CVector     GetAbsolutePoint(unsigned nIndex);
	unsigned    GetPointCount();
    CVector     GetDirection(unsigned nSection);
    unsigned    GetNextPointIndex(unsigned nIndex);
    void        ProcessPoint(IEntity *piEntity,unsigned int dwCurrentFrame,double dTimeFraction);

	bool 		 GetPoint(unsigned int nIndex,SRoutePoint *psPoint);
	bool 		 AddPoint(unsigned int nIndex,const SRoutePoint &sPoint);
	bool 		 SetPoint(unsigned int nIndex,const SRoutePoint &sPoint);
	void		 RemovePoint(unsigned int nIndex);
	void 		 Clear();

    BEGIN_PROP_MAP(CRoute)
        PROP(m_vPoints,"Puntos")
        PROP(m_bCyclic,"Ciclica")
    END_PROP_MAP()
	CVector GetPlayAreaElementRoutePoint(CVector vPlayAreaElementPos,SRoutePoint *pPoint);
	CVector GetPlayAreaElementPoint(CVector vPlayAreaElementPos,unsigned nIndex);
};

DECLARE_SERIALIZABLE(CRoute)
