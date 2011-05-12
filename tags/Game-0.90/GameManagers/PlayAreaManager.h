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
#include "Route.h"

DECLARE_CUSTOM_WRAPPER1(CPlayAreaElementWrapper,IPlayAreaElement,m_piElement)
DECLARE_CUSTOM_WRAPPER1(CPlayAreaEntityWrapper,IPlayAreaEntity,m_piElement)

struct SEntityLayerData
{
	CEntityTypeWrapper	m_EntityType;
	SEntityLayer		m_LayerConfig;
};

BEGIN_STRUCT_PROPS(SEntityLayer)
	PROP(dSeparation,"Separation")
	PROP(dPositionJitter,"PositionJitter")
	PROP(dYawJitter,"YawJitter")
	PROP(vColor,"Color")
	PROP(dColorSaturationMargin,"ColorSaturationMargin")
	PROP(dColorValueMargin,"ColorValueMargin")
	PROP(sEntityType,"EntityType")
END_STRUCT_PROPS()

BEGIN_STRUCT_PROPS(SEntityLayerData)
	PROP(m_EntityType,"EntityType")
	PROP(m_LayerConfig,"Config")
END_STRUCT_PROPS()

class CPlayAreaManager: virtual public CSystemObjectBase,
                        virtual public CSystemSerializableBase,
                        virtual public IPlayAreaManager,
                        virtual public IGameManager,
						virtual public IPlayAreaDesign,
						virtual public IEntityEvents
{
	bool m_bStarted;

    vector<CPlayAreaElementWrapper>	m_vElements;
	vector<CPlayAreaEntityWrapper>	m_vEntityLayerElements;
	vector<CPlayAreaEntityWrapper>	m_vDynamicElements;
	
    double m_dCameraDistanceFromPlayer;
    double m_dCameraPitch;

	double m_dPlayAreaHeight;
    CVector m_vPlayMovementPos;
    double  m_dPlayMovementSpeed;
	double  m_dPlayMovementMaxHorzScroll;

    C3DSVector m_vPlayerRouteStart;
    C3DSVector m_vPlayerRouteEnd;

    SPlayAreaInfo   m_PlayArea;
    IEntity         *m_piPlayerEntity;
	IPlayer         *m_piPlayer;

    CGameControllerWrapper  m_GameControllerWrapper;
	CGenericCameraWrapper	m_CameraWrapper;
	CFrameManagerWrapper	m_FrameManagerWrapper;

    CVector m_vAirPlayAreaMins;
    CVector m_vAirPlayAreaMaxs;
    CVector m_vVisibleAirPlayAreaMins;
    CVector m_vVisibleAirPlayAreaMaxs;

	bool m_bScenarioCompleted;

	double  m_dCameraFollowFactor;
	double	m_dPlayMovementMaxRoll;
	double  m_dPlayMovementMaxForward;
	double  m_dPlayMovementMaxRight;
	double  m_dPlayMovementMinForward;
	double  m_dPlayMovementMinRight;
	double  m_dPlayMovementCurrentForward;
	double  m_dPlayMovementCurrentRight;
	double	m_dPlayMovementCurrentRoll;
	double	m_dPlayMovementRollVelocity;
	unsigned int	m_dwPlayMovementLastRollTime;

	CVector m_PlayerKilledVelocity;
	bool   m_bPlayerLandingEnabled;
	bool   m_bPlayerTakeOffEnabled;
	CRoute m_PlayerLandingRoute;
	CRoute m_PlayerTakeOffRoute;
	

	std::vector<SEntityLayerData>  m_vEntityLayers;

    void CalculateAirPlayArea();

public:

    bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    void Destroy();

	// Element management

	unsigned long	AddElement(std::string sType);
	void			RemoveElement(unsigned int nIndex);
	void			GetElement(unsigned int nIndex,IPlayAreaElement **ppiElement);
	unsigned int	GetElements();

	// IPlayAreaDesign Color based Entity layers

	unsigned long	AddEntityLayer(SEntityLayer *pLayer);
	void			UpdateEntityLayer(unsigned int nIndex,SEntityLayer *pLayer);
	void			RemoveEntityLayer(unsigned int nIndex);
	void			GetEntityLayer(unsigned int nIndex,SEntityLayer *pLayer,IEntityType **ppiEntityType);
	unsigned int	GetEntityLayers();

	bool			UpdateEntityLayers();

	// IDesign

	void DesignRender(IGenericRender *piRender);

    // IPlayAreaManager

	IGenericCamera *GetCamera();
	double          GetCameraSpeed();
	
	void	GetPlayAreaPlaneAt(CVector vPos,CVector *pPlayAreaMins,CVector *pPlayAreaMaxs);
    void    GetAirPlayPlane(CVector *pPlayAreaMins,CVector *pPlayAreaMaxs);
    void    GetVisibleAirPlayPlane(CVector *pVisiblePlayAreaMins,CVector *pVisiblePlayAreaMaxs);
	void    GetCurrentVisibleArea(CVector *pVisiblePlayAreaMins,CVector *pVisiblePlayAreaMaxs);
	
    CVector GetPlayMovementPosition();
    CVector GetPlayMovementForward();
    CVector GetPlayMovementRight();
    CVector GetPlayMovementUp();

	void	GetPlayerRoute(CVector *pvStart,CVector *pvEnd);

	void	EnumeratePlayAreaElements(IPlayAreaElementEnumerationCallback *piCallback);
	void	SetPlayMovementPosition(CVector vPosition);
	
	bool	IsScenarioCompleted();
	bool	IsVisible(CVector vPos,double dRadius,bool bWithScroll);
	
	void 	CreateDynamicEntityElement(IEntityType *piEntityType,CVector vPosition,CVector vAngles,IPlayAreaEntity **ppiElement);
		
    // IGameManager

	void CreateScenario();
	void LoadScenario(ISystemPersistencyNode *piNode);
	void SaveScenario(ISystemPersistencyNode *piNode);
	void CloseScenario();

    void Start();
    void Stop();
    void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

	void UpdatePlayCameraPosition();
	void MovePlayer(unsigned long nKey);
	void ProcessInput(IGameGUIManager *piManager);

	
	// IEntityEvents
	
	void OnRemoved(IEntity *piEntity);
	void OnKilled(IEntity *piEntity);
	
	// Edition method

	void	GetPlayAreaConfig(SPlayAreaConfig *pConfig);
	void	SetPlayAreaConfig(SPlayAreaConfig *pConfig);
	void	UpdatePlayArea();

	BEGIN_PROP_MAP(CPlayAreaManager)
        BEGIN_PROP_SUBMAP("ScenarioProps")
		  PROP(m_CameraWrapper,			"Camara");
          PROP(m_vElements,             "Elementos");
          PROP(m_vPlayerRouteStart,     "Origen");
          PROP(m_vPlayerRouteEnd,       "Destino");
		  PROP_FLAGS(m_PlayerTakeOffRoute,"TakeOffRoute",MRPF_NORMAL|MRPF_OPTIONAL);
		  PROP_FLAGS(m_PlayerLandingRoute,"LandingRoute",MRPF_NORMAL|MRPF_OPTIONAL);
		  PROP_VALUE_FLAGS(m_bPlayerLandingEnabled,"LandingEnabled",false,MRPF_NORMAL|MRPF_OPTIONAL);
		  PROP_VALUE_FLAGS(m_bPlayerTakeOffEnabled,"TakeOffEnabled",false,MRPF_NORMAL|MRPF_OPTIONAL);
		  PROP_VALUE(m_dCameraDistanceFromPlayer,"DistanciaCamara",115);
          PROP_VALUE(m_dPlayMovementSpeed,"VelocidadCamara",5);
		  PROP_VALUE(m_dCameraPitch,      "InclinacionVista",0);
		  PROP_VALUE(m_dPlayMovementMaxHorzScroll,    "MaximoScrollHorz",0);
		  PROP_VALUE_FLAGS(m_dPlayAreaHeight,    "AlturaJuego",20,MRPF_NORMAL|MRPF_OPTIONAL);
		  PROP_FLAGS(m_vEntityLayers,"EntityLayers",MRPF_NORMAL|MRPF_OPTIONAL)
        END_PROP_SUBMAP("ScenarioProps")
	END_PROP_MAP();

    CPlayAreaManager(void);
    ~CPlayAreaManager(void);
};
