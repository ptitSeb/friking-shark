#pragma once

class CPlayAreaElementBase:virtual public CSystemObjectBase,virtual public IPlayAreaElement
{
protected:

	bool	m_bActive;
    DWORD   m_dwActivationTime;

public:


    virtual void Activate(DWORD dwCurrentTime);
    virtual void Deactivate();
    virtual bool IsActive();
	virtual void Reset();

		virtual bool ProcessFrame(CVector vPlayPosition,SPlayAreaInfo *pPlayAreaInfo,DWORD dwCurrentTime,double dInterval);

    CPlayAreaElementBase();
    virtual ~CPlayAreaElementBase();
};

bool Util_IsInPlayArea(CVector vPosition,SPlayAreaInfo *pInfo);
