#pragma once

class CPlayAreaFormation: virtual public CPlayAreaElementBase,virtual public IPlayAreaFormation,virtual public IFormationEvents
{
    CFormationTypeWrapper    m_FormationType;
	CEntityTypeWrapper		 m_BonusType;
	C3DSVector               m_vPosition;
    IFormation              *m_piFormation;
    bool                     m_bAlreadyOverPoint;

public:
    
    BEGIN_PROP_MAP(CPlayAreaFormation)
        PROP(m_vPosition,"Posicion")
		PROP(m_FormationType,"Tipo")
        PROP_FLAGS(m_BonusType,"Bonus",MRPF_NORMAL|MRPF_OPTIONAL)
    END_PROP_MAP()

    //ISystemObject
    bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    void Destroy();

    //IPlayAreaElement
    void Activate(DWORD dwCurrentTime);
    void Deactivate();
	void Reset();

	void DesignRender(IGenericRender *piRender,bool bSelected);
	void DesignGetBBox(CVector *pvMins,CVector *pvMaxs);
	CTraceInfo DesignGetTrace(const CVector &p1,const CVector &p2);

    bool ProcessFrame(CVector vPlayPosition,SPlayAreaInfo *pAreaInfo,DWORD dwCurrentTime,double dInterval);

    //IFormationEvents
    void OnFormationRemoved(ISystemObject *piFormation);
    void OnFormationKilled(ISystemObject *piFormation,IEntity *piLastEntity);

	//	IPlayAreaFormation

	void SetPosition(CVector &vPosition);
	void SetFormationType(IFormationType *piFormationType);
	void SetBonusType(IEntityType *piEntityType);

	CVector GetPosition();
	void	GetFormationType(IFormationType **piFormationType);
	void	GetBonusType(IEntityType **ppiEntityType);

    CPlayAreaFormation();
    ~CPlayAreaFormation();
};
