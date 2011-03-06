#pragma once

class CPlayAreaCheckPoint: virtual public CPlayAreaElementBase,virtual public IPlayAreaCheckPoint
{
	C3DSVector          m_vPosition;

public:

	BEGIN_PROP_MAP(CPlayAreaCheckPoint)
		PROP(m_vPosition,"Posicion")
	END_PROP_MAP()

	void DesignRender(IGenericRender *piRender,bool bSelected);
	CTraceInfo DesignGetTrace(const CVector &p1,const CVector &p2);

	// IPlayAreaCheckPoint
	CVector GetCheckPointPosition();

	CPlayAreaCheckPoint();
	~CPlayAreaCheckPoint();
};
