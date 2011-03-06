#pragma once

class CWorldManager;

class CWorldEntity:public CEntityBase
{
    CWorldManager *m_pManager;

public:
	CTraceInfo GetTrace(const CVector &p1,const CVector &p2);

	void OnDamage(double dDamage,IEntity *pAggresor);
    void Render(IGenericRender *piRender,IGenericCamera *piCamera);

    CWorldEntity(CWorldManager *pManager);
    ~CWorldEntity(void);
};
