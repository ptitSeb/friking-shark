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

class CIATestMainWindow;

class ITestScenario
{
public:
	virtual void Initialize(CIATestMainWindow *pWindow)=0;
	virtual void Finalize()=0;

	virtual std::string GetName()=0;

	virtual ~ITestScenario(){}
};

class CTestScenarioSeek:virtual public ITestScenario
{
	void Initialize(CIATestMainWindow *pWindow);
	void Finalize(){}

	std::string GetName(){return "Seek";}
};

class CTestScenarioFlee:virtual public ITestScenario
{
	void Initialize(CIATestMainWindow *pWindow);
	void Finalize(){}

	std::string GetName(){return "Flee";}
};

class CTestScenarioArrive:virtual public ITestScenario
{
	void Initialize(CIATestMainWindow *pWindow);
	void Finalize(){}

	std::string GetName(){return "Arrive";}
};

class CTestScenarioPursue:virtual public ITestScenario
{
	void Initialize(CIATestMainWindow *pWindow);
	void Finalize(){}

	std::string GetName(){return "Pursue";}
};

class CTestScenarioEvade:virtual public ITestScenario
{
	void Initialize(CIATestMainWindow *pWindow);
	void Finalize(){}

	std::string GetName(){return "Evade";}
};

class CTestScenarioWander:virtual public ITestScenario
{
	void Initialize(CIATestMainWindow *pWindow);
	void Finalize(){}

	std::string GetName(){return "Wander";}
};

class CTestScenarioInterpose:virtual public ITestScenario
{
	void Initialize(CIATestMainWindow *pWindow);
	void Finalize(){}

	std::string GetName(){return "Interpose";}
};

class CTestScenarioOffsetPursue:virtual public ITestScenario
{
	void Initialize(CIATestMainWindow *pWindow);
	void Finalize(){}

	std::string GetName(){return "Offset Pursue";}
};
