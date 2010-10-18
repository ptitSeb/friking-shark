// BonusTypes.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "BonusTypes.h"
#include "WeaponUpgradeBonusType.h"
#include "SpeedUpgradeBonusType.h"
#include "PointBonusType.h"
#include "LiveBonusType.h"

BEGIN_SYSTEM_MODULE()
  SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CWeaponUpgradeBonusType,"WeaponUpgradeBonus");
  SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CSpeedUpgradeBonusType,"SpeedUpgradeBonus");
  SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CPointBonusType,"PointBonus");
  SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CLiveBonusType,"LiveBonus");
END_SYSTEM_MODULE();
