#pragma once

#include "SoundSystemManager.h"

#define DSBFREQUENCY_ORIGINAL 10

class CSoundType: virtual public CSystemObjectBase,virtual public ISoundType
{
  deque<ALuint>   m_dAvailableSources;
  bool            m_bSoundsAcquired;
  ALuint		  m_iSoundBuffer;
  
  bool Unserialize(ISystemPersistencyNode *piNode);
  
public:

    ISoundManager  *m_piSoundManager;

    // Propiedades Persistentes

    unsigned int    m_nChannels;
    bool    		m_bLoop;
    string  		m_sFileName;
    double  		m_dVolume;

    ALuint AcquireSoundSource();
    void   ReleaseSoundSource(ALuint nSource);

    ISound *CreateInstance(IEntity *piEntity,unsigned int dwCurrentTime);

    bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    void Destroy();

    BEGIN_PROP_MAP(CSoundType);
        PROP(m_sFileName,"File");
        PROP_VALUE_FLAGS(m_nChannels,"Channels",5,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_bLoop,"Loop",false,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dVolume,"Volume",100,MRPF_NORMAL|MRPF_OPTIONAL);
    END_PROP_MAP();

    CSoundType();
    ~CSoundType();
};

class CSound:virtual public CSystemObjectBase,public ISound
{
protected:

	ALuint 					m_nSource;
    CSoundType             *m_pType;
    bool                    m_bActive;
    IEntity                 *m_piEntity;

public:

    // ISound

    void Activate(unsigned int dwCurrentTime);
    void Deactivate();
    bool IsActive();

    bool HasFinished();

    bool ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval);

    CSound(CSoundType *pTyp,IEntity *piEntitye,unsigned int dwCurrentTimeBase);
    virtual ~CSound();
};
