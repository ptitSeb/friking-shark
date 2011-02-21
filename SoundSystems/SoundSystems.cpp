// SoundSystems.cpp : Defines the entry point for the DLL application.
//

#include "./stdafx.h"
#include "SoundSystems.h"
#include "SoundSystemManager.h"
#include "SoundType.h"

BEGIN_SYSTEM_MODULE()
  SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CSoundSystemManager,"CSoundSystemManager");
  SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CSoundType,"SoundType");
END_SYSTEM_MODULE();

double DeciblesToPercentage(double dDb)
{
  double dTotalLog=log10(1000.0);
  double dLog=log10(dDb);
  double dValue=100.0*dLog/dTotalLog;
  return dValue;
}

double PercentageToDecibels(double dPercentage)
{
  double dTotalLog=log10(10000.0);
  double dValue=dTotalLog*dPercentage/100.0;
  return dValue==0?0:pow(10.0,dValue);
}