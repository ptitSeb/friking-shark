#pragma once

class CParticleWhirlpoolModifierType:public CParticleEmitterModifierType
{
public:

	CVector m_vRotationAxis;
	double m_dStartRadius;
	double m_dEndRadius;
	double m_dStartAngle;
	unsigned int m_nStartTime;
	unsigned int m_nEndTime;
	unsigned int m_nLoops;
	
    IParticleEmitterModifier *CreateInstance(unsigned int dwCurrentTime);

	// Propiedades Persistentes
	
	BEGIN_PROP_MAP(CParticleWhirlpoolModifierType)
		PROP_CLASS_CHAIN(CParticleEmitterModifierType)
		PROP_VALUE_FLAGS(m_dStartRadius,"StartRadius",5,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dEndRadius,"EndRadius",100,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nStartTime,"StartTime",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nEndTime,"EndTime",5000,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dStartAngle,"StartAngle",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nLoops,"Loops",4,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vRotationAxis,"RotationAxis",AxisPosY,MRPF_NORMAL|MRPF_OPTIONAL)
		END_PROP_MAP()

    CParticleWhirlpoolModifierType();
    ~CParticleWhirlpoolModifierType();
};

class CParticleWhirlpoolModifier:public IParticleEmitterModifier
{
	unsigned int m_nCreationTime;
	CParticleWhirlpoolModifierType *m_pType;
	map<IParticleEmitter *,unsigned int> m_mEmitters;
	
public:

	void AddEmitter(IParticleEmitter *pEmitter);
	
	void ProcessEmitter(IParticleEmitter *piEmitter,IParticleSystem *piSystem,unsigned int dwCurrentTime,double dInterval);

	CParticleWhirlpoolModifier(CParticleWhirlpoolModifierType *pType,unsigned long dwCurrentTime);
};
