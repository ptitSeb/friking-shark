// WeaponTypes.cpp : Defines the entry point for the DLL application.
//
#include "./stdafx.h"
#include "WeaponTypes.h"
#include "ProjectileLauncherType.h"
#include "BulletProjectileType.h"
#include "BombProjectileType.h"
#include "HomingMissileProjectileType.h"

BEGIN_SYSTEM_MODULE()
  SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CProjectileLauncherType,"ProjectileLauncherType")
  SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CBulletProjectileType,"BulletType")
  SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CBombProjectileType,"BombType")
  SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CHomingMissileProjectileType,"HomingMissileType")
END_SYSTEM_MODULE()
