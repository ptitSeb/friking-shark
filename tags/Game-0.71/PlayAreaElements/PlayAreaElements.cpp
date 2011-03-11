// AnimationSystems.cpp : Defines the entry point for the DLL application.
//

#include "./stdafx.h"
#include "PlayAreaElementBase.h"
#include "PlayAreaEntity.h"
#include "PlayAreaFormation.h"
#include "PlayAreaCheckPoint.h"
#include "FormationType.h"

BEGIN_SYSTEM_MODULE()
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CFormationType,"CFormationType")
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CPlayAreaEntity,"CPlayAreaEntity")
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CPlayAreaFormation,"CPlayAreaFormation")
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CPlayAreaCheckPoint,"CPlayAreaCheckPoint")
END_SYSTEM_MODULE()

