#pragma once

DECLARE_CUSTOM_WRAPPER1(CParticleSystemTypeWrapper,IParticleSystemType,m_piParticleSystemType)

class CParticleSystemAnimationObjectType: public CAnimationObjectTypeBase, public virtual IParticleSystemAnimationObjectTypeDesign
{
public:

    CParticleSystemTypeWrapper  m_ParticleSystemType;
    CVector              		m_vPosition;
    unsigned int                m_nStartTime;
    unsigned int                m_nEndTime;
    bool                		m_bTrackEntity;

    IAnimationObject *CreateInstance(IAnimation *piAnimation,unsigned int dwCurrentTime);
	std::string 	  GetAnimationObjectDescription();
	
    BEGIN_PROP_MAP(CParticleSystemAnimationObjectType)
        PROP_CLASS_CHAIN(CAnimationObjectTypeBase)
        PROP(m_ParticleSystemType,"ParticleSystem")
        PROP_FLAGS(m_vPosition,"Position",MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_bTrackEntity,"TrackEntity",false,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_nStartTime,"StartTime",0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_nEndTime,"EndTime",0,MRPF_NORMAL|MRPF_OPTIONAL)
    END_PROP_MAP();

	
	// IParticleSystemAnimationObjectTypeDesignn
	
	void GetConfig(SParticleSystemAnimationObjectTypeConfig *pConfig);
	void SetConfig(SParticleSystemAnimationObjectTypeConfig *pConfig);
	void SetParticleSystemType(IParticleSystemType *piParticleSystemType);
	void GetParticleSystemType(IParticleSystemType **ppiParticleSystemType);
	
    CParticleSystemAnimationObjectType();
    ~CParticleSystemAnimationObjectType();
};


class CParticleSystemAnimationObject: public CAnimationObjectBase
{
    CParticleSystemAnimationObjectType  *m_pType;
    IParticleSystem                     *m_piParticleSystem;
    bool                                 m_bPositionAndAnglesInitialized;

    void UpdatePositionAndAngles();
    void CheckActivation(unsigned int dwCurrentTime);

public:

    void Activate(unsigned int dwCurrentTime);
    void Deactivate();

    bool ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval);
    void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera);

    CParticleSystemAnimationObject(CParticleSystemAnimationObjectType *pType,IAnimation *piAnimation);
    ~CParticleSystemAnimationObject();
};
