#pragma once

class IFormationEditorObjectLabel:virtual public IGameWindow
{
public:
	virtual void SetObject(IDesignObject *piObject)=0;
};
