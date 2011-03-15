#pragma once

class CSoundSystemManager:virtual public CSystemObjectBase, virtual public ISoundManager
{
	unsigned int m_nMasterVolume;// 0,100
	bool m_bEnable3DSound;

	ALCcontext *m_pContext;
	ALCdevice *m_pDevice;

	CVector m_vListenerPosition;
	CVector m_vListenerOrientation;
	CVector m_vListenerVelocity;
	
	void UpdateListener();
	
public:
	
	// ISoundManager
	
	void SetListenerPosition(CVector vPosition);
	void SetListenerOrientation(CVector vOrientation);
	void SetListenerVelocity(CVector vVelocity);

	bool Is3DSoundEnabled();

	unsigned int  GetMasterVolume();
	void SetMasterVolume(unsigned int dVolume);

	BEGIN_PROP_MAP(CSoundSystemManager)
		PROP_VALUE_FLAGS(m_nMasterVolume,"Volume",100,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bEnable3DSound,"3DSound",false,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP();

	bool Init(std::string sClass,std::string sName,ISystem *piSystem);
	void Destroy();

	bool Unserialize(ISystemPersistencyNode *piNode);
	
	CSoundSystemManager();
	virtual ~CSoundSystemManager();
};
