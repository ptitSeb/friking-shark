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
#include "resource.h"
#include "GameRunTimeLib.h"
#include "GameGUILib.h"
#include "IATestMainWindow.h"
#include "IATestUtils.h"
#include "TestScenarios.h"
extern float dBaseSize;

void CTestScenarioSeek::Initialize(CIATestMainWindow *pWindow)
{
	CIAEntityBase *pPrey=new CIAEntityBase;
	pPrey->SetPosition(CVector(dBaseSize*0.5,dBaseSize*0.5,0));
	pPrey->SetColor(CVector(0.5,0,0));
	pPrey->SetSize(20);

	CIAEntityBase *pHunter=new CIAEntityBase;
	pHunter->SetPosition(CVector(dBaseSize*0.3,dBaseSize*0.6,0));
	pHunter->SetColor(CVector(0,0,0.5));
	pHunter->SetSize(30);

	pHunter->SeekTarget(pPrey);

	pWindow->AddEntity("Hunter",pHunter,true);
	pWindow->AddEntity("Prey",pPrey,true);
}

void CTestScenarioFlee::Initialize(CIATestMainWindow *pWindow)
{
	CIAEntityBase *pPrey=new CIAEntityBase;
	pPrey->SetPosition(CVector(dBaseSize*0.5,dBaseSize*0.5,0));
	pPrey->SetColor(CVector(0.5,0,0));
	pPrey->SetSize(20);

	CIAEntityBase *pHunter=new CIAEntityBase;
	pHunter->SetPosition(CVector(dBaseSize*0.3,dBaseSize*0.6,0));
	pHunter->SetColor(CVector(0,0,0.5));
	pHunter->SetSize(30);

	pPrey->FleeTarget(pHunter);

	pWindow->AddEntity("Hunter",pHunter,true);
	pWindow->AddEntity("Prey",pPrey,true);
}


void CTestScenarioPursue::Initialize(CIATestMainWindow *pWindow)
{
	CIAEntityBase *pPrey=new CIAEntityBase;
	pPrey->SetPosition(CVector(dBaseSize*0.5,dBaseSize*0.5,0));
	pPrey->SetColor(CVector(0.5,0,0));
	pPrey->SetSize(20);

	CIAEntityBase *pHunter=new CIAEntityBase;
	pHunter->SetPosition(CVector(dBaseSize*0.3,dBaseSize*0.6,0));
	pHunter->SetColor(CVector(0,0,0.5));
	pHunter->SetSize(30);

	pPrey->Wander(true,200,100,5000);
	pHunter->PursueTarget(pPrey);

	pWindow->AddEntity("Hunter",pHunter,true);
	pWindow->AddEntity("Prey",pPrey,true);
}

void CTestScenarioEvade::Initialize(CIATestMainWindow *pWindow)
{
	CIAEntityBase *pPrey=new CIAEntityBase;
	pPrey->SetPosition(CVector(dBaseSize*0.5,dBaseSize*0.5,0));
	pPrey->SetColor(CVector(0.5,0,0));
	pPrey->SetSize(20);

	CIAEntityBase *pHunter=new CIAEntityBase;
	pHunter->SetPosition(CVector(dBaseSize*0.3,dBaseSize*0.6,0));
	pHunter->SetColor(CVector(0,0,0.5));
	pHunter->SetSize(30);

	pHunter->Wander(true,200,100,5000);
	pPrey->EvadeTarget(pHunter);

	pWindow->AddEntity("Hunter",pHunter,true);
	pWindow->AddEntity("Prey",pPrey,true);
}

void CTestScenarioArrive::Initialize(CIATestMainWindow *pWindow)
{
	CIAEntityBase *pPrey=new CIAEntityBase;
	pPrey->SetPosition(CVector(dBaseSize*0.5,dBaseSize*0.5,0));
	pPrey->SetColor(CVector(0.5,0,0));
	pPrey->SetSize(20);

	CIAEntityBase *pHunter=new CIAEntityBase;
	pHunter->SetPosition(CVector(dBaseSize*0.3,dBaseSize*0.6,0));
	pHunter->SetColor(CVector(0,0,0.5));
	pHunter->SetSize(30);

	pHunter->ArriveTarget(pPrey,eSBArriveSpeed_Normal);

	pWindow->AddEntity("Hunter",pHunter,true);
	pWindow->AddEntity("Prey",pPrey,true);
}

void CTestScenarioWander::Initialize(CIATestMainWindow *pWindow)
{
	CIAEntityBase *pHunter=new CIAEntityBase;
	pHunter->SetPosition(CVector(dBaseSize*0.3,dBaseSize*0.6,0));
	pHunter->SetColor(CVector(0,0,0.5));
	pHunter->SetSize(30);

	pHunter->Wander(true,200,100,5000);
	pWindow->AddEntity("Hunter",pHunter,true);
}

void CTestScenarioInterpose::Initialize(CIATestMainWindow *pWindow)
{
	CIAEntityBase *pPrey=new CIAEntityBase;
	pPrey->SetPosition(CVector(dBaseSize*0.5,dBaseSize*0.5,0));
	pPrey->SetColor(CVector(0.5,0,0));
	pPrey->SetSize(20);

	CIAEntityBase *pPreyDestination=new CIAEntityBase;
	pPreyDestination->SetPosition(CVector(dBaseSize*0.6,dBaseSize*0.3,0));
	pPreyDestination->SetColor(CVector(0.6,0.6,0.6));
	pPreyDestination->SetSize(20);

	CIAEntityBase *pHunter=new CIAEntityBase;
	pHunter->SetPosition(CVector(dBaseSize*0.3,dBaseSize*0.6,0));
	pHunter->SetColor(CVector(0,0,0.5));
	pHunter->SetSize(30);

	pHunter->Interpose(pPrey,pPreyDestination);
	pPreyDestination->Wander(true,200,100,5000);
	pPrey->SeekTarget(pPreyDestination);

	pWindow->AddEntity("Hunter",pHunter,true);
	pWindow->AddEntity("Prey",pPrey,true);
	pWindow->AddEntity("PreyDest",pPreyDestination,true);
}


void CTestScenarioOffsetPursue::Initialize(CIATestMainWindow *pWindow)
{
	CIAEntityBase *pTarget=new CIAEntityBase;
	pTarget->SetPosition(CVector(dBaseSize*0.3,dBaseSize*0.6,0));
	pTarget->SetColor(CVector(0.5,0.5,0.5));
	pTarget->SetSize(30);
	pWindow->AddEntity("Target",pTarget,true);

	CIAEntityBase *pLeader=new CIAEntityBase;
	pLeader->SetPosition(CVector(dBaseSize*0.3,dBaseSize*0.6,0));
	pLeader->SetColor(CVector(0,0,0.5));
	pLeader->SetSize(30);
	//pLeader->Wander(true,200,100,5000);
	pLeader->ArriveTarget(pTarget,eSBArriveSpeed_Normal);
    pWindow->AddEntity("Leader",pLeader,true);

	CVector pVectors[]={CVector(-30,-50,0),CVector(-30,50,0),CVector(-60,0,0),CVector(0,0,0)};
	int nFollowers=0;
	while(pVectors[nFollowers]!=CVector(0,0,0))
	{
		char sName[1024];
		CIAEntityBase *pFollower=new CIAEntityBase;
		pFollower->SetPosition(CVector(dBaseSize*(0.2+((double)nFollowers)*0.1),dBaseSize*(0.2+((double)nFollowers)*0.1),0));
		pFollower->SetColor(CVector(0.5,0,0));
		pFollower->SetSize(20);
		pFollower->SetMaxVelocity(pLeader->GetMaxVelocity()*0.7);
		pFollower->SetMaxForce(pLeader->GetMaxForce());
		pFollower->OffsetPursue(pLeader,pVectors[nFollowers]);
		pFollower->SetRenderFlags(RENDER_FLAGS_NONE);
		sprintf(sName,"Follower-%d",nFollowers+1);
		pWindow->AddEntity(sName,pFollower,true);
		nFollowers++;
	}
}
