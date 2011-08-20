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
  m_nChannels=1;
  m_piSoundManager=NULL;
  m_iSoundBuffer=AL_NONE;
  m_dVolume=100;
}

CSoundType::~CSoundType(void)
{
}

bool CSoundType::Unserialize(ISystemPersistencyNode *piNode)
{
	bool bOk=CSystemObjectBase::Unserialize(piNode);
	if(!bOk){return false;}
	if(m_sFileName.length()){bOk=LoadFromFile();}
	return bOk;
}

bool CSoundType::Load(std::string sFileName,std::string sGroup)
{
	m_sFileName=sFileName;
	m_sGroup=sGroup;
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
	
	ReleaseAllSources();
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
		ALenum error=alGetError();
		if(error==AL_NO_ERROR)
		{
			alBufferData(m_iSoundBuffer, nFileFormat, pBuffer, nBufferSize, nFrequency);
		}
		else
		{
			bOk=false;
			RTTRACE("CSoundType::LoadOgg -> Failed to create buffer for file %s: Error %x:%s",m_sFileName.c_str(),error,alutGetErrorString(error));
		}
		
	}
	free(pBuffer);
	pBuffer=NULL;
	
	if(!bOk)
	{
		RTTRACE("CSoundType::LoadFromFile -> Failed to load sound %s.",m_sFileName.c_str());
	}
	return bOk;
}

bool CSoundType::LoadWav()
{
	bool bOk=true;

	ReleaseAllSources();
	if(m_iSoundBuffer){alDeleteBuffers(1,&m_iSoundBuffer);m_iSoundBuffer=AL_NONE;}
	
	m_iSoundBuffer=alutCreateBufferFromFile(m_sFileName.c_str());
	if(m_iSoundBuffer==AL_NONE)
	{
		ALenum aluterror=alutGetError();
		RTTRACE("CSoundType::LoadWav -> Failed to create sound buffer from file %s. Error %x:%s",m_sFileName.c_str(),aluterror,alutGetErrorString(aluterror));
		bOk=false;
	}
	if(!bOk)
	{
		RTTRACE("CSoundType::LoadWav -> Failed to load sound %s.",m_sFileName.c_str());
	}
	return bOk;
}

std::string CSoundType::GetFileName(){return m_sFileName;}
std::string CSoundType::GetGroup(){return m_sGroup;}

bool CSoundType::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
  bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);;
  if(bOk)
  {
    ISystemManager  *piSystemManager=GetSystemManager();
    ISystem         *piTempSystem=NULL;
    ISystemObject   *piObject=NULL;
    if(piSystemManager){piTempSystem=piSystemManager->GetSystem("GameGUI");}
    if(piTempSystem){piTempSystem->GetObject("SoundManager",&piObject);}
    if(piObject){m_piSoundManager=QI(ISoundManager,piObject);}
    REL(piSystemManager);
    REL(piTempSystem);
    REL(piObject);
    bOk=m_piSoundManager!=NULL;
  }
  return bOk;
}

void CSoundType::ReleaseAllSources()
{
	if(m_piSoundManager==NULL){return;}

	std::list<SSourceData>::iterator i;
	for(i=m_vCurrentSources.begin();i!=m_vCurrentSources.end();i++)
	{
		SSourceData sourceData=*i;
		m_piSoundManager->ReleaseSource(sourceData.nSource);
	}
	m_vCurrentSources.clear();
}

void CSoundType::Destroy()
{
  ReleaseAllSources();
  
  REL(m_piSoundManager);
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

unsigned int CSoundType::AcquireSoundSource(ISound *piSound)
{
  if(m_piSoundManager==NULL){return AL_NONE;}
  if(piSound==NULL){return AL_NONE;}
  if(m_vCurrentSources.size() && m_vCurrentSources.size()>=m_nChannels)
  {
	  SSourceData sourceData=m_vCurrentSources.front();
	  sourceData.piSound->ReclaimSource();
	  m_vCurrentSources.pop_front();

	  sourceData.piSound=piSound;
	  m_vCurrentSources.push_back(sourceData);
	  return sourceData.nSource;
  }
  else
  {
	  unsigned int nSource=m_piSoundManager->AcquireSource(this);
	  if(nSource!=AL_NONE)
	  {
		   alSourceStop(nSource);
		  alSourcei(nSource,AL_BUFFER,m_iSoundBuffer);
		  SSourceData sourceData;
		  sourceData.piSound=piSound;
		  sourceData.nSource=nSource;
		  m_vCurrentSources.push_back(sourceData);
	  }
	  return nSource;
  }
}

void CSoundType::ReleaseSoundSource(unsigned int nSource)
{
  if(m_piSoundManager==NULL){return;}
  if(nSource!=AL_NONE)
  {
	  std::list<SSourceData>::iterator i;
	  for(i=m_vCurrentSources.begin();i!=m_vCurrentSources.end();i++){if(nSource==i->nSource){break;}}
	  if(i!=m_vCurrentSources.end())
	  {
		  SSourceData sourceData=*i;
		  alSourceStop(nSource);
		  alSourcei(nSource,AL_BUFFER,AL_NONE);
		  m_piSoundManager->ReleaseSource(nSource);
		  m_vCurrentSources.erase(i);
	  }
  }
}

void CSoundType::ReclaimSource(unsigned int nSource)
{
	if(m_piSoundManager==NULL){return;}
	if(nSource!=AL_NONE)
	{
		std::list<SSourceData>::iterator i;
		for(i=m_vCurrentSources.begin();i!=m_vCurrentSources.end();i++){if(nSource==i->nSource){break;}}
		if(i!=m_vCurrentSources.end())
		{
			SSourceData sourceData=*i;
			sourceData.piSound->ReclaimSource();
			alSourceStop(nSource);
			alSourcei(nSource,AL_BUFFER,AL_NONE);
			m_vCurrentSources.erase(i);
		}
	}
}

CSound::CSound(CSoundType *pType)
{
  m_nSource=AL_NONE;
  m_pType=pType;
  m_bLoop=false;
  m_dVolume=100;
  m_dGroupVolume=pType->m_piSoundManager?pType->m_piSoundManager->GetGroupVolume(pType->m_sGroup):100;

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
    m_nSource=m_pType->AcquireSoundSource(this);
    if(m_nSource!=AL_NONE)
	{
		UpdateSource();
		alSourcePlay(m_nSource);
    }
  }
  else
  {
	alSourcePlay(m_nSource);
  }
}

void CSound::Pause()
{
	if(m_nSource!=AL_NONE)
	{	
		alSourcePause(m_nSource);
	}
}

void CSound::Resume()
{
	if(m_nSource!=AL_NONE)
	{	
		alSourcePlay(m_nSource);
	}
}

void CSound::ReclaimSource()
{
	m_nSource=AL_NONE;
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

bool CSound::IsPaused()
{
	if(m_nSource==AL_NONE){return false;}
	ALint nState=AL_STOPPED;
	alGetSourcei(m_nSource,AL_SOURCE_STATE,&nState);
	if(nState==AL_PAUSED){return true;}
	return false;
}

void CSound::UpdateSource()
{
	if(m_nSource==AL_NONE){return;}
	
	double dEffectiveVolume=(float)((m_dVolume*m_pType->m_dVolume)/(100.0));
	alSourcei(m_nSource,AL_LOOPING,m_bLoop?AL_TRUE:AL_FALSE);
	alSourcef(m_nSource,AL_GAIN,(float)((dEffectiveVolume*m_dGroupVolume)/(100.0*100.0)));
	
	if(m_pType->m_piSoundManager->Is3DSoundEnabled())
	{
		alSourcei(m_nSource,AL_ROLLOFF_FACTOR,1);
		alSourcei(m_nSource,AL_CONE_INNER_ANGLE,360);
		alSourcei(m_nSource,AL_CONE_OUTER_ANGLE,360);
		alSourcei(m_nSource,AL_SOURCE_RELATIVE,AL_FALSE);
		alSource3f(m_nSource,AL_POSITION,(float)m_vPosition.c[0],(float)m_vPosition.c[1],(float)m_vPosition.c[2]);
		alSource3f(m_nSource,AL_VELOCITY,(float)m_vVelocity.c[0],(float)m_vVelocity.c[1],(float)m_vVelocity.c[2]);
		alSource3f(m_nSource,AL_DIRECTION,(float)m_vOrientation.c[0],(float)m_vOrientation.c[1],(float)m_vOrientation.c[2]);
	}
	else
	{
		alSourcei(m_nSource,AL_ROLLOFF_FACTOR,0);
	}
}

void CSound::SetLoop(bool bLoop){m_bLoop=bLoop;UpdateSource();}
void CSound::SetVolume(double dVolume){m_dVolume=dVolume;UpdateSource();}
double CSound::GetVolume(){return m_dVolume;}

void CSound::SetPosition(CVector vPosition){m_vPosition=vPosition;UpdateSource();}
void CSound::SetOrientation(CVector vOrientation){m_vOrientation=vOrientation;UpdateSource();}
void CSound::SetVelocity(CVector vVelocity){m_vVelocity=vVelocity;UpdateSource();}

void CSound::OnMasterVolumeChanged(unsigned int dMasterVolume){}
void CSound::OnMute(bool bOn){}

void CSound::OnGroupVolumeChanged(const std::string &sName,unsigned int dGroupVolume)
{
	if(sName==m_pType->m_sGroup)
	{
		m_dGroupVolume=m_pType->m_piSoundManager?m_pType->m_piSoundManager->GetGroupVolume(m_pType->m_sGroup):100;
		UpdateSource();
	}
}

