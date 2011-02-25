#pragma once

#include "SoundSystemManager.h"

class CSoundType: virtual public CSystemObjectBase,virtual public ISoundType
{
  deque<IDirectSoundBuffer8 *>   m_dAvailableBuffers;
  bool                           m_bSoundsAcquired;
  CSound                        *m_pSound;

public:

  ISoundManager                 *m_piSoundManager;

    // Propiedades Persistentes

    unsigned int   m_dwMaxSimultaneousSounds;
    bool    m_bCyclic;
    string  m_sFileName;
    int     m_dPan;
    int     m_dFrequency;
    int     m_dVolume;
    double  m_dMinDistance;
    double  m_dMaxDistance;

    IDirectSoundBuffer8 *AcquireSoundBuffer();
    void                 ReleaseSoundBuffer(IDirectSoundBuffer8 *);

    ISound *CreateInstance(IEntity *piEntity,unsigned int dwCurrentTime);

    bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    void Destroy();

    BEGIN_PROP_MAP(CSoundType);
        PROP(m_sFileName,"Archivo");
        PROP_VALUE_FLAGS(m_dwMaxSimultaneousSounds,"MaximoSonidosSimultaneos",5,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_bCyclic,"Repetir",false,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dPan,"PosicionEstereo(-100,100)",0,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dFrequency,"Frecuencia",DSBFREQUENCY_ORIGINAL,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dVolume,"Volumen(0-100)",100,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dMinDistance,"DistanciaMinima",30,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dMaxDistance,"DistanciaMaxima",100,MRPF_NORMAL|MRPF_OPTIONAL);
   END_PROP_MAP();

    CSoundType();
    ~CSoundType();
};

// No se puede llamar sound object porque colisiona con el helper de DirectSound
class CSoundObject:virtual public CSystemObjectBase,public ISound,public ISoundManagerEvents
{
protected:

    IDirectSoundBuffer8    *m_piSound;
    IDirectSound3DBuffer8  *m_piSound3D;
    CSoundType             *m_pType;
    bool                    m_bActive;
    IEntity                 *m_piEntity;

    void UpdateSound();

public:

    // ISound

    void Activate(unsigned int dwCurrentTime);
    void Deactivate();
    bool IsActive();

    bool HasFinished();

    bool ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval);

    // ISoundManagerEvents

    void OnMasterPanChanged(int dMasterPan);
    void OnMasterVolumeChanged(int dMasterVolume);

    CSoundObject(CSoundType *pTyp,IEntity *piEntitye,unsigned int dwCurrentTimeBase);
    virtual ~CSoundObject();
};
