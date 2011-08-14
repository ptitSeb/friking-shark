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
#include "GameRunTimeLib.h"

class ISound;
class ISoundType;

class ISoundType:virtual public ISystemUnknown
{
public:
	virtual ISound *CreateInstance()=0;

	virtual bool        Load(std::string sFileName,std::string sGroup)=0;
	virtual std::string GetFileName()=0;
	virtual std::string GetGroup()=0;
};

class ISound
{
public:
    virtual void Play()=0;
    virtual void Stop()=0;
    virtual bool IsPlaying()=0;
	virtual void DetachSource()=0;
	
	virtual void SetLoop(bool bLoop)=0;
	virtual void SetVolume(double nVolume)=0;
	virtual double GetVolume()=0;
	
	virtual void SetPosition(CVector vPosition)=0;
	virtual void SetOrientation(CVector vOrientation)=0;
	virtual void SetVelocity(CVector vVelocity)=0;
	
    virtual ~ISound(){}
};

class ISoundManager : virtual public ISystemUnknown
{
public:
  virtual bool Is3DSoundEnabled()=0;

  virtual unsigned int  GetMasterVolume()=0;
  virtual void			SetMasterVolume(unsigned int dVolume)=0;
  
  virtual unsigned int  GetGroupVolume(const std::string &sName)=0;
  virtual void			SetGroupVolume(const std::string &sName,unsigned int nVolume)=0;

  virtual void			SetMute(bool bOn)=0;

  virtual void SetListenerPosition(CVector vPosition)=0;
  virtual void SetListenerOrientation(CVector vOrientation)=0;
  virtual void SetListenerVelocity(CVector vVelocity)=0;
  
  virtual ~ISoundManager(){}
};

class ISoundManagerEvents : virtual public ISystemUnknown
{
public:

  virtual void OnMasterVolumeChanged(unsigned int dMasterVolume)=0;
  virtual void OnGroupVolumeChanged(const std::string &sName,unsigned int dGroupVolume)=0;
  virtual void OnMute(bool bOn)=0;

  virtual ~ISoundManagerEvents(){}
};


double DeciblesToPercentage(double dDb);
double PercentageToDecibels(double dPercentage);