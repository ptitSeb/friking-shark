#pragma once

DECLARE_CUSTOM_WRAPPER1(CSoundTypeWrapper,ISoundType,m_piSoundType)

class CSoundAnimationObjectType: public CAnimationObjectTypeBase, public virtual ISoundAnimationObjectTypeDesign
{
public:

    CSoundTypeWrapper    m_SoundType;
    unsigned int         m_nStartTime;
    unsigned int         m_nEndTime;
	double               m_dVolume;
	bool                 m_bLoop;

    IAnimationObject *CreateInstance(IAnimation *piAnimation,unsigned int dwCurrentTime);

    BEGIN_PROP_MAP(CSoundAnimationObjectType)
        PROP(m_SoundType,"Sound")
        PROP_VALUE_FLAGS(m_nStartTime,"StartTime",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nEndTime,"EndTime",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dVolume,"Volume",100,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bLoop,"Loop",false,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_CLASS_CHAIN(CAnimationObjectTypeBase)
    END_PROP_MAP();
	
	// ISoundAnimationObjectTypeDesignn
	void GetConfig(SSoundAnimationObjectTypeConfig *pConfig);
	void SetConfig(SSoundAnimationObjectTypeConfig *pConfig);
	void SetSound(ISoundType *piSoundType);
	void GetSound(ISoundType **ppiSoundType);
	
    CSoundAnimationObjectType();
    ~CSoundAnimationObjectType();
};


class CSoundAnimationObject: public CAnimationObjectBase
{
    CSoundAnimationObjectType  *m_pType;
    ISound                     *m_piSound;
	
	void CheckActivation(unsigned int dwCurrentTime);
	
public:

    void Activate(unsigned int dwCurrentTime);
    void Deactivate();

    bool ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval);
    void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera);

    CSoundAnimationObject(CSoundAnimationObjectType *pType,IAnimation *piAnimation);
    ~CSoundAnimationObject();
};
