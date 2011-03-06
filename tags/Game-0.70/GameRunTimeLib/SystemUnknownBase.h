#pragma once

class CSystemUnknownBase:virtual public ISystemUnknown
{
    unsigned int       m_dwReferences;

protected:
		virtual void FinalRelease();
public:

    virtual void AddReference();
    virtual void ReleaseReference();

    CSystemUnknownBase(void);
    virtual ~CSystemUnknownBase(void);
};