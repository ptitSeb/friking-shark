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


#include "./stdafx.h"
#include "SoundSystems.h"
#include "SoundType.h"
#include "vorbis/vorbisfile.h"

#define OGG_PACKET_SIZE (4096)
#define OGG_REALLOC_SIZE (1024*512)

CSoundType::CSoundType(void)
{
  m_bLoop=false;
  m_nChannels=5;
  m_piSoundManager=NULL;
  m_iSoundBuffer=AL_NONE;
  m_dVolume=100;
}

CSoundType::~CSoundType(void)
{
  REL(m_piSoundManager);
}

bool CSoundType::Unserialize(ISystemPersistencyNode *piNode)
{
	bool bOk=CSystemObjectBase::Unserialize(piNode);
	if(!bOk){return false;}
	if(m_sFileName.length()){bOk=LoadFromFile();}
	return bOk;
}

bool CSoundType::Load(std::string sFileName)
{
	m_sFileName=sFileName;
	return LoadFromFile();
}

bool CSoundType::LoadFromFile()
{
	bool bOk=true;
	unsigned int nStartTime=GetTimeStamp();	
	char sExt[1024]={0};
	GetExtension(m_sFileName.c_str(),sExt);
	if(strcasecmp(sExt,".OGG")==0)
	{
		bOk=LoadOgg();
	}
	else
	{
		bOk=LoadWav();
	}
	RTTRACE("CSoundType::LoadFromFile -> %s loaded (%d ms)",m_sFileName.c_str(),GetTimeStamp()-nStartTime);
	return bOk;
}

bool CSoundType::LoadOgg()
{
	bool bOk=true;
	
	for(size_t x=0;x<m_dAvailableSources.size();x++)
	{
		ALuint nSource=m_dAvailableSources[x];
		if(nSource){alDeleteSources(1,&nSource);}
	}
	m_dAvailableSources.clear();
	if(m_iSoundBuffer){alDeleteBuffers(1,&m_iSoundBuffer);m_iSoundBuffer=AL_NONE;}
	char *pBuffer=NULL;
	unsigned int nBufferSize=0;
	unsigned int nFileFormat=0;
	long         nFrequency=0;
	FILE *pFile=fopen(m_sFileName.c_str(), "rb");
	bOk=(pFile!=NULL);
	if(bOk)
	{
		vorbis_info *pInfo=NULL;
		OggVorbis_File oggFile;
		bool bVorbisFileOpen=bOk=(ov_open(pFile, &oggFile, NULL, 0)==0);
		if(bOk)
		{
			pInfo = ov_info(&oggFile, -1);
			bOk=(pInfo!=NULL);
			if(pInfo)
			{
				nFileFormat=pInfo->channels == 1?AL_FORMAT_MONO16:AL_FORMAT_STEREO16;
				nFrequency=pInfo->rate;
			}
		}
		if(bOk)
		{
			int bitStream=0;
			unsigned int nDecodedBytes=0;
			unsigned int nAllocatedSize=0;
			char temp[OGG_PACKET_SIZE];
			do 
			{
				nDecodedBytes=ov_read(&oggFile, temp, OGG_PACKET_SIZE, 0, 2, 1, &bitStream);
				if(nDecodedBytes)
				{
					if(nAllocatedSize<nBufferSize+nDecodedBytes)
					{
						nAllocatedSize+=OGG_REALLOC_SIZE;
						pBuffer=(char*)realloc(pBuffer,nAllocatedSize);
					}
					memcpy(pBuffer+nBufferSize,temp,nDecodedBytes);
					nBufferSize+=nDecodedBytes;
				}
			} while (nDecodedBytes> 0 && bOk);
			bOk=(nBufferSize!=0);
		}
		if(bVorbisFileOpen)
		{
			ov_clear(&oggFile);
		}
	}
	if(bOk)
	{
		RTTRACE("CSoundType::LoadOgg -> Loaded Sound %s",m_sFileName.c_str());

		alGetError();
		alGenBuffers(1, &m_iSoundBuffer);
		if(m_iSoundBuffer!=AL_NONE)
		{
			alBufferData(m_iSoundBuffer, nFileFormat, pBuffer, nBufferSize, nFrequency);
			
			ALuint *pSources=new ALuint [m_nChannels];
			alGenSources(m_nChannels,pSources);
			if(alGetError()==AL_NO_ERROR)
			{
				for(unsigned int x=0;x<m_nChannels;x++)
				{
					alSourcei(pSources[x],AL_BUFFER,m_iSoundBuffer);
					alSourcef(pSources[x],AL_GAIN,(float)(m_dVolume/100.0));
					
					m_dAvailableSources.push_back(pSources[x]);
				}
			}
			else
			{
				RTTRACE("CSoundType::LoadOgg -> Failed to create sources. Error %d",alGetError());
			}
			delete [] pSources;
			pSources=NULL;
		}
		else
		{
			RTTRACE("CSoundType::LoadOgg -> Failed to create buffer for file %s: Error %d",m_sFileName.c_str(),alGetError());
		}
	}
	free(pBuffer);
	pBuffer=NULL;
	
	bOk=(m_dAvailableSources.size()!=0);
	if(!bOk)
	{
		RTTRACE("CSoundType::LoadFromFile -> Failed to load sound %s. Error %d:%s",m_sFileName.c_str(),alutGetError(),alutGetErrorString(alutGetError()));
	}
	return bOk;
}

bool CSoundType::LoadWav()
{
	bool bOk=true;
	
	for(size_t x=0;x<m_dAvailableSources.size();x++)
	{
		ALuint nSource=m_dAvailableSources[x];
		if(nSource){alDeleteSources(1,&nSource);}
	}
	m_dAvailableSources.clear();
	if(m_iSoundBuffer){alDeleteBuffers(1,&m_iSoundBuffer);m_iSoundBuffer=AL_NONE;}
	
	m_iSoundBuffer=alutCreateBufferFromFile(m_sFileName.c_str());
	if(m_iSoundBuffer!=AL_NONE)
	{
		RTTRACE("CSoundType::LoadWav -> Loaded Sound %s",m_sFileName.c_str());
		
		ALuint *pSources=new ALuint [m_nChannels];
		alGetError();
		alGenSources(m_nChannels,pSources);
		if(alGetError()==AL_NO_ERROR)
		{
			for(unsigned int x=0;x<m_nChannels;x++)
			{
				alSourcei(pSources[x],AL_BUFFER,m_iSoundBuffer);
				alSourcef(pSources[x],AL_GAIN,(float)(m_dVolume/100.0));
				
				m_dAvailableSources.push_back(pSources[x]);
			}
		}
		else
		{
			RTTRACE("CSoundType::LoadWav -> Failed to create sources. Error %d",alGetError());
		}
		delete [] pSources;
		pSources=NULL;
	}
	bOk=(m_dAvailableSources.size()!=0);
	if(!bOk)
	{
		RTTRACE("CSoundType::LoadWav -> Failed to load sound %s. Error %d:%s",m_sFileName.c_str(),alutGetError(),alutGetErrorString(alutGetError()));
	}
	return bOk;
}

std::string CSoundType::GetFileName(){return m_sFileName;}

bool CSoundType::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
  bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);;
  if(bOk)
  {
    ISystemManager  *piSystemManager=GetSystemManager();
    ISystem         *piTempSystem=NULL;
    ISystemObject   *piObject=NULL;
    if(piSystemManager){piTempSystem=piSystemManager->GetSystem("GameSystem");}
    if(piTempSystem){piTempSystem->GetObject("SoundManager",&piObject);}
    if(piObject){m_piSoundManager=QI(ISoundManager,piObject);}
    REL(piSystemManager);
    REL(piTempSystem);
    REL(piObject);
    bOk=m_piSoundManager!=NULL;
  }
  return bOk;
}

void CSoundType::Destroy()
{
  for(size_t x=0;x<m_dAvailableSources.size();x++)
  {
	ALuint nSource=m_dAvailableSources[x];
	if(nSource){alDeleteSources(1,&nSource);}
  }
  m_dAvailableSources.clear();
  
  if(m_iSoundBuffer){alDeleteBuffers(1,&m_iSoundBuffer);m_iSoundBuffer=AL_NONE;}

  CSystemObjectBase::Destroy();
}

ISound *CSoundType::CreateInstance()
{
  if(m_piSoundManager==NULL)
  {
    return NULL;
  }
  CSound *pSound=new CSound(this);
  pSound->SetLoop(m_bLoop);
  pSound->SetVolume(m_dVolume);
  return pSound;
}

ALuint CSoundType::AcquireSoundSource()
{
  if(m_dAvailableSources.size())
  {
    ALuint nSource=m_dAvailableSources.front();
    m_dAvailableSources.pop_front();
    return nSource;
  }
  return AL_NONE;
}

void CSoundType::ReleaseSoundSource(ALuint nSource)
{
  if(nSource!=AL_NONE)
  {
	m_dAvailableSources.push_back(nSource);
  }
}


CSound::CSound(CSoundType *pType)
{
  m_nSource=AL_NONE;
  m_pType=pType;
  m_bLoop=false;
  m_dVolume=100;
  
  SUBSCRIBE_TO_CAST(pType->m_piSoundManager,ISoundManagerEvents);
}

CSound::~CSound()
{
    if(m_nSource!=AL_NONE)
    {
	  alSourceStop(m_nSource);
      m_pType->ReleaseSoundSource(m_nSource);
	  m_nSource=AL_NONE;
    }
    UNSUBSCRIBE_FROM_CAST(m_pType->m_piSoundManager,ISoundManagerEvents);
}

void CSound::Play()
{
  if(m_nSource==AL_NONE)
  {
    m_nSource=m_pType->AcquireSoundSource();
    if(m_nSource!=AL_NONE)
	{
		UpdateSource();
		alSourcePlay(m_nSource);
    }
  }
}

void CSound::Stop()
{
	if(m_nSource!=AL_NONE)
	{
		alSourceStop(m_nSource);
		m_pType->ReleaseSoundSource(m_nSource);
		m_nSource=AL_NONE;
	}
}

bool CSound::IsPlaying()
{
  if(m_nSource==AL_NONE){return false;}
  ALint nState=AL_STOPPED;
  alGetSourcei(m_nSource,AL_SOURCE_STATE,&nState);
  if(nState==AL_PLAYING || nState==AL_INITIAL){return true;}
  return false;
}

void CSound::UpdateSource()
{
	if(m_nSource==AL_NONE){return;}
	
	alSourcei(m_nSource,AL_LOOPING,m_bLoop?AL_TRUE:AL_FALSE);
	alSourcef(m_nSource,AL_GAIN,(float)(m_dVolume/100.0));
	
	if(m_pType->m_piSoundManager->Is3DSoundEnabled())
	{
		alSourcei(m_nSource,AL_CONE_INNER_ANGLE,360);
		alSourcei(m_nSource,AL_CONE_OUTER_ANGLE,360);
		alSourcei(m_nSource,AL_SOURCE_RELATIVE,AL_FALSE);
		alSource3f(m_nSource,AL_POSITION,(float)m_vPosition.c[0],(float)m_vPosition.c[1],(float)m_vPosition.c[2]);
		alSource3f(m_nSource,AL_VELOCITY,(float)m_vVelocity.c[0],(float)m_vVelocity.c[1],(float)m_vVelocity.c[2]);
		alSource3f(m_nSource,AL_DIRECTION,(float)m_vOrientation.c[0],(float)m_vOrientation.c[1],(float)m_vOrientation.c[2]);
	}
}

void CSound::SetLoop(bool bLoop){m_bLoop=bLoop;UpdateSource();}
void CSound::SetVolume(double dVolume){m_dVolume=dVolume;UpdateSource();}

void CSound::SetPosition(CVector vPosition){m_vPosition=vPosition;UpdateSource();}
void CSound::SetOrientation(CVector vOrientation){m_vOrientation=vOrientation;UpdateSource();}
void CSound::SetVelocity(CVector vVelocity){m_vVelocity=vVelocity;UpdateSource();}

