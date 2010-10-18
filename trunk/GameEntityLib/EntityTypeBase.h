#pragma once

DECLARE_CUSTOM_WRAPPER1(CAnimationTypeWrapper,IAnimationType,m_piAnimationType);
DECLARE_CUSTOM_WRAPPER1(CWeaponTypeWrapper,IWeaponType,m_piWeaponType);

class CEntityBase;
class CEntityTypeBase: virtual public CSystemObjectBase,virtual public IEntityType 
{
    // Propiedades Persistentes

    deque<CAnimationTypeWrapper>  m_dAnimations;
    deque<CWeaponTypeWrapper>     m_dWeapons;
    deque<string>                 m_dAnimationNames;
    deque<string>                 m_dWeaponNames;
    CVector                       m_vBBoxMins;
    CVector                       m_vBBoxMaxs;
    bool                          m_bSolid;
	bool                          m_bUseBSP;
   bool                           m_bFlyMove;
    DWORD                         m_dwAlignment;

protected:

    virtual void	InitializeEntity(CEntityBase *piEntity,DWORD dwCurrentTime);

	void AttachAnimations();
	virtual void	GetBBox(CVector *pvMins,CVector *pvMaxs);
	virtual CVector	GetSize();
	virtual void	DesignRender(IGenericRender *piRender,CVector &vPosition,CVector &vAngles,bool bSelected);
	virtual void    DesignGetBBox(CVector *pvMins,CVector *pvMaxs);
	virtual double  DesignGetRadius();
	virtual CTraceInfo DesignGetTrace(const CVector &vPosition,const CVector &vAngles,const CVector &p1,const CVector &p2 );

public:

    BEGIN_PROP_MAP(CEntityTypeBase);
        PROP(m_dAnimationNames,"Animaciones")
        PROP_FLAGS(m_dWeaponNames,"Armas",MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_FLAGS(m_vBBoxMins,"BBoxMins",MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_FLAGS(m_vBBoxMaxs,"BBoxMaxs",MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_bSolid,"Solido",false,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bUseBSP,"UseBSP",false,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_bFlyMove,"MovimientoAereo",true,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dwAlignment,"Bando",ENTITY_ALIGNMENT_NEUTRAL,MRPF_NORMAL|MRPF_OPTIONAL)
    END_PROP_MAP();

    CEntityTypeBase(void);
    ~CEntityTypeBase(void);
};
