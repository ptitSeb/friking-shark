//  Friking Shark, a Flying Shark arcade remake.
//  Copyright (C) 2011  Javier Martin Garcia (javiermartingarcia@gmail.com)
//	
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


#pragma once

#include "SoundSystemManager.h"
#include <deque>
#include <map>


class CSoundType: virtual public CSystemObjectBase,virtual public ISoundType
{
  struct SSourceData
  {
  	ISound *piSound;
	unsigned int nSource;

	SSourceData(){nSource=AL_NONE;piSound=NULL;}
  };
  std::list<SSourceData> m_vCurrentSources;

  ALuint		  m_iSoundBuffer;
  
  void ReleaseAllSources();

  bool Unserialize(ISystemPersistencyNode *piNode);
  
  bool LoadFromFile();
  bool LoadOgg();
  bool LoadWav();
  
public:

    ISoundManager  *m_piSoundManager;

    // Propiedades Persistentes

    unsigned int    m_nChannels;
    bool    		m_bLoop;
    std::string  	m_sFileName;
	std::string  	m_sGroup;
    double  		m_dVolume;

    unsigned int AcquireSoundSource(ISound *piSound);
    void         ReleaseSoundSource(unsigned int nSource);
	void         ReclaimSource(unsigned int nSource);

    ISound *CreateInstance();

    bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    void Destroy();
	
	bool        Load(std::string sFileName,std::string sGroup);
	std::string GetFileName();
	std::string GetGroup();

    BEGIN_PROP_MAP(CSoundType);
        PROP(m_sFileName,"File");
		PROP_VALUE_FLAGS(m_sGroup,"Group","General",MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_nChannels,"Channels",1,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_bLoop,"Loop",false,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dVolume,"Volume",100,MRPF_NORMAL|MRPF_OPTIONAL);
    END_PROP_MAP();

    CSoundType();
    ~CSoundType();
};

class CSound:virtual public CSystemObjectBase,public ISound,virtual public ISoundManagerEvents
{
protected:

	ALuint 					m_nSource;
    CSoundType             *m_pType;
	
	bool    				m_bLoop;
	double  				m_dVolume;

	double					m_dGroupVolume;
    
	CVector m_vPosition;
	CVector m_vOrientation;
	CVector m_vVelocity;
	
	void UpdateSource();
public:

    // ISound

    void Play();
	void Pause();
	void Resume();
	void Stop();
    bool IsPlaying();
	bool IsPaused();
	void ReclaimSource();
	
	void SetLoop(bool bLoop);
	void SetVolume(double nVolume);
	double GetVolume();
	
	void SetPosition(CVector vPosition);
	void SetOrientation(CVector vOrientation);
	void SetVelocity(CVector vVelocity);

	// ISoundManagerEvents

	void OnMasterVolumeChanged(unsigned int dMasterVolume);
	void OnGroupVolumeChanged(const std::string &sName,unsigned int dGroupVolume);
	void OnMute(bool bOn);

    CSound(CSoundType *pType);
    virtual ~CSound();
};
