#pragma once

class CPlayAreaFormation: virtual public CPlayAreaElementBase,virtual public IPlayAreaFormation,virtual public IFormationEvents
{
	EPlayerStateCondition	 m_eConditionType;
	unsigned int 			 m_nConditionValue;
    CFormationTypeWrapper    m_FormationType;
	CEntityTypeWrapper		 m_BonusType;
	CFormationTypeWrapper    m_AlternativeFormationType;
	CEntityTypeWrapper		 m_AlternativeBonusType;
	C3DSVector               m_vPosition;
    IFormation              *m_piFormation;
    bool                     m_bAlreadyOverPoint;
	bool 					 m_bUsingAlternative;

public:
    
    BEGIN_PROP_MAP(CPlayAreaFormation)
        PROP(m_vPosition,"Posicion")
		PROP(m_FormationType,"Tipo")
		PROP_FLAGS(m_BonusType,"Bonus",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_AlternativeFormationType,"AlternativeType",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_AlternativeBonusType,"AlternativeBonus",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_eConditionType,"ConditionType",ePlayerStateCondition_None,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nConditionValue,"ConditionValue",0,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP()

    //ISystemObject
    bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    void Destroy();

    //IPlayAreaElement
    void Activate(unsigned int dwCurrentTime);
    void Deactivate();
	void Reset();

	void DesignRender(IGenericRender *piRender,bool bSelected);
	void DesignGetBBox(CVector *pvMins,CVector *pvMaxs);
	CTraceInfo DesignGetTrace(const CVector &p1,const CVector &p2);

    bool ProcessFrame(CVector vPlayPosition,SPlayAreaInfo *pAreaInfo,unsigned int dwCurrentTime,double dInterval);

    //IFormationEvents
    void OnFormationRemoved(ISystemObject *piFormation);
    void OnFormationKilled(ISystemObject *piFormation,IEntity *piLastEntity);

	//	IPlayAreaFormation

	void SetPosition(CVector &vPosition);
	void SetFormationType(IFormationType *piFormationType);
	void SetBonusType(IEntityType *piEntityType);
	void SetAlternativeFormationType(IFormationType *piFormationType);
	void SetAlternativeBonusType(IEntityType *piEntityType);
	void SetCondition(EPlayerStateCondition eConditionType,unsigned int nValue);

	CVector GetPosition();
	void	GetFormationType(IFormationType **piFormationType);
	void	GetBonusType(IEntityType **ppiEntityType);
	void	GetAlternativeFormationType(IFormationType **piFormationType);
	void	GetAlternativeBonusType(IEntityType **ppiEntityType);
	void    GetCondition(EPlayerStateCondition *peConditionType,unsigned int *pnValue);

    CPlayAreaFormation();
    ~CPlayAreaFormation();
};
