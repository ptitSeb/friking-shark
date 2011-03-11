#pragma once

class IScenarioEditorObjectLabel:virtual public IGameWindow
{
public:
	virtual void SetObject(IDesignObject *piObject)=0;
};
