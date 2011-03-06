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
