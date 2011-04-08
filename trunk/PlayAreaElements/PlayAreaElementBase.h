#pragma once

class CPlayAreaElementBase:virtual public CSystemObjectBase,virtual public IPlayAreaElement
{
protected:

	bool	m_bActive;
    unsigned int   m_nActivationTime;

public:


    virtual void Activate(unsigned int dwCurrentTime);
    virtual void Deactivate();
    virtual bool IsActive();
	virtual void Reset();

		virtual bool ProcessFrame(CVector vPlayPosition,SPlayAreaInfo *pPlayAreaInfo,unsigned int dwCurrentTime,double dInterval);

    CPlayAreaElementBase();
    virtual ~CPlayAreaElementBase();
};

bool Util_IsInPlayArea(CVector vPosition,SPlayAreaInfo *pInfo);
